#include "city/Housing.h"
#include "city/City.h"
#include "city/Company.h"
#include "city/Market.h"

Housing::Housing(const std::string& name, Type type, unsigned int nbStairs, std::size_t nbInhabitants, float comfort) :
    Building(name, type, nbStairs), mLeases(nbInhabitants, Lease(this)), mComfort(comfort)
{
    //ctor
}

Housing::~Housing()
{
    //dtor
}

std::unique_ptr<Tile> Housing::clone() const
{
    return std::unique_ptr<Tile>(new Housing(mTextureName, mType, mNbStairs, mLeases.size(), mComfort));
}

void Housing::update()
{
    // Read messages
    while (!mMailbox.isEmpty())
    {
        Message message = mMailbox.get();
        if (message.type == MessageType::MARKET)
        {
            const Market<Lease>::Event& event = message.getInfo<const Market<Lease>::Event>();
            switch (event.type)
            {
                case Market<Lease>::Event::Type::ITEM_ADDED:
                    mLeasesInMarket.insert(event.itemId);
                    break;
                case Market<Lease>::Event::Type::SALE:
                    mLeasesInMarket.erase(event.sale.itemId);
                    break;
                default:
                    break;
            }
        }
    }
}

void Housing::tearDown()
{
    update();
    // Remove everything from markets
    const Market<Lease>* market = static_cast<const Market<Lease>*>(mOwner->getCity()->getMarket(VMarket::Type::RENT));
    for (Id id : mLeasesInMarket)
        mOwner->getMessageBus()->send(Message::create(market->getMailboxId(), MessageType::MARKET, market->createRemoveItemEvent(id)));
}

void Housing::setOwner(Company* owner)
{
    Building::setOwner(owner);
    for (Lease& lease : mLeases)
        lease.setOwner(mOwner);
}

std::vector<Lease>& Housing::getLeases()
{
    return mLeases;
}

const std::vector<Lease>& Housing::getLeases() const
{
    return mLeases;
}

float Housing::getComfort() const
{
    return mComfort;
}
