#include "city/Business.h"
#include "message/MessageBus.h"
#include "city/City.h"
#include "city/Company.h"
#include "city/Market.h"

Tile::Type Business::getBusinessType(Good good)
{
    switch (good)
    {
        case Good::NECESSARY:
            return Tile::Type::GROCERY;
        case Good::NORMAL:
            return Tile::Type::MALL;
        case Good::LUXURY:
            return Tile::Type::BOUTIQUE;
        default:
            return Tile::Type::GROCERY;
    }
}

Business::Business(const std::string& name, Type type, unsigned int nbStairs, Good good, unsigned int maxSizeStock,
    std::size_t nbEmployees, Work::Type employeeType) :
    Building(name, type, nbStairs), mGood(good), mMaxSizeStock(maxSizeStock), mStock(0), mStockCost(0.0),
    mPrice(0.0)
{
    mEmployees.push_back(Work(Work::Type::MANAGER, this));
    mEmployees.resize(nbEmployees + 1, Work(employeeType, this));
}

Business::~Business()
{
    //dtor
}

std::unique_ptr<Tile> Business::clone() const
{
    return std::unique_ptr<Tile>(new Business(mTextureName, mType, mNbStairs, mGood, mMaxSizeStock, mEmployees.size() - 1, mEmployees.back().getType()));
}

void Business::setOwner(Company* owner)
{
    Building::setOwner(owner);
    for (Work& employee : mEmployees)
        employee.setEmployer(mOwner);
}

void Business::update()
{
    // Read messages
    bool priceDirty = false;
    bool desiredQuantityDirty = false;
    while (!mMailbox.isEmpty())
    {
        Message message = mMailbox.get();
        if (message.type == MessageType::MARKET)
        {
            const VMarket::EventBase& eventBase = message.getInfo<VMarket::EventBase>();
            if (eventBase.marketType == VMarket::Type::WORK)
            {
                const Market<Work>::Event& event = static_cast<const Market<Work>::Event&>(eventBase);
                switch (event.type)
                {
                    case Market<Work>::Event::Type::ITEM_ADDED:
                        mWorksInMarket.insert(event.itemId);
                        break;
                    case Market<Work>::Event::Type::SALE:
                        mWorksInMarket.erase(event.sale.itemId);
                        break;
                    default:
                        break;
                }
            }
            else
            {
                const Market<const Building>::Event& event = static_cast<const Market<const Building>::Event&>(eventBase);
                if (event.type == Market<const Building>::Event::Type::PURCHASE)
                {
                    // To do : save the building to fetch the good later
                    ++mStock;
                    mStockCost += event.sale.value;
                    priceDirty = true;
                    desiredQuantityDirty = true;
                    mOwner->getMessageBus()->send(Message::create(mOwner->getCity()->getBank().getMailboxId(), MessageType::BANK, mOwner->getCity()->getBank().createTransferMoneyEvent(mOwner->getAccount(), event.sale.sellerAccount, event.sale.value)));
                }
            }
        }
        else if (message.type == MessageType::BUSINESS)
        {
            const Event& event = message.getInfo<Event>();
            if (event.type == Event::Type::RESERVATION)
            {
                if (hasPreparedGoods())
                {
                    mOwner->getMessageBus()->send(Message::create(mMailbox.getId(), message.sender, MessageType::BUSINESS, Event{Event::Type::RESERVATION_ACCEPTED, mOwner->getAccount(), mPrice}));
                    --mStock;
                    mStockCost -= mPrice;
                    mPreparedGoods -= 1.0;
                    desiredQuantityDirty = true;
                }
                else
                    mOwner->getMessageBus()->send(Message::create(mMailbox.getId(), message.sender, MessageType::BUSINESS, Event{Event::Type::RESERVATION_REFUSED, {}, {}}));
            }
        }
    }

    // Update price
    if (priceDirty)
        updatePrice();

    // Update desired quantity
    if (desiredQuantityDirty)
        updateDesiredQuantity();
}

void Business::tearDown()
{
    update();
    // Remove everything from markets
    const Market<Work>* market = static_cast<const Market<Work>*>(mOwner->getCity()->getMarket(VMarket::Type::WORK));
    for (Id id : mWorksInMarket)
        mOwner->getMessageBus()->send(Message::create(market->getMailboxId(), MessageType::MARKET, market->createRemoveItemEvent(id)));
    // Remove bids for market
    mOwner->getMessageBus()->send(Message::create(mMailbox.getId(), market->getMailboxId(), MessageType::MARKET, market->createSetQuantityEvent(0)));
}

Good Business::getGood() const
{
    return mGood;
}

unsigned int Business::getStock() const
{
    return mStock;
}

bool Business::hasPreparedGoods() const
{
    return mStock > 0 && mPreparedGoods >= 1.0;
}

Money Business::getPrice() const
{
    return mPrice;
}

Work& Business::getManager()
{
    return mEmployees[0];
}

const Work& Business::getManager() const
{
    return mEmployees[0];
}

std::vector<Work>& Business::getEmployees()
{
    return mEmployees;
}

const std::vector<Work>& Business::getEmployees() const
{
    return mEmployees;
}

void Business::onNewMonth()
{
    prepareGoods();
    buyGoods();
}

void Business::prepareGoods()
{
    double preparedGoods = 0.0;
    for (std::size_t i = 1; i < mEmployees.size(); ++i)
    {
        if (mEmployees[i].hasWorkedThisMonth())
            preparedGoods += 1.0;
    }
    if (getManager().hasWorkedThisMonth())
        preparedGoods *= 2.0;
    mPreparedGoods += preparedGoods;
}

void Business::buyGoods()
{
    updateDesiredQuantity();
    const Market<const Building>* market = getMarket();
    for (const Market<const Building>::Item* item : market->getItems())
    {
        if (mOwner->getCity()->getMap().isReachableFrom(this, item->good))
            mOwner->getMessageBus()->send(Message::create(mMailbox.getId(), market->getMailboxId(), MessageType::MARKET, market->createBidEvent(item->id, item->reservePrice)));
    }
}

const Market<const Building>* Business::getMarket()
{
    switch (mGood)
    {
        case Good::NECESSARY:
            return static_cast<const Market<const Building>*>(mOwner->getCity()->getMarket(VMarket::Type::NECESSARY_GOOD));
        case Good::NORMAL:
            return static_cast<const Market<const Building>*>(mOwner->getCity()->getMarket(VMarket::Type::NORMAL_GOOD));
        case Good::LUXURY:
            return static_cast<const Market<const Building>*>(mOwner->getCity()->getMarket(VMarket::Type::LUXURY_GOOD));
        default:
            return nullptr;
    };
}

void Business::updatePrice()
{
    // Compute payroll
    Money payroll(0.0);
    for (const Work& employee : mEmployees)
    {
        if (employee.hasWorkedThisMonth())
            payroll += employee.getSalary();
    }
    // Update price
    mPrice = (mStockCost + payroll) / mStock * mOwner->getRetailMargin(mGood);
}

void Business::updateDesiredQuantity()
{
    const Market<const Building>* market = getMarket();
    unsigned int desiredQuantity = std::max(0, static_cast<int>(mMaxSizeStock) - static_cast<int>(mStock));
    mOwner->getMessageBus()->send(Message::create(mMailbox.getId(), market->getMailboxId(), MessageType::MARKET, market->createSetQuantityEvent(desiredQuantity)));
}
