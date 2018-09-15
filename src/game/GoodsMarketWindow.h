#pragma once

#include "message/Mailbox.h"
#include "gui/GuiWindow.h"
#include "city/Market.h"
#include "city/Good.h"

class MessageBus;
class StylesheetManager;
class GuiTable;
class Building;

class GoodsMarketWindow : public GuiWindow
{
public:
    GoodsMarketWindow(MessageBus* messageBus, StylesheetManager* stylesheetManager, std::array<Market<Good>*, 3> markets);
    ~GoodsMarketWindow();

    virtual void setUp() override;

    void update();

private:
    using Key = std::tuple<const Building*, Good::Type, Money>;

    MessageBus* mMessageBus;
    StylesheetManager* mStylesheetManager;
    Mailbox mMailbox;
    std::array<Market<Good>*, 3> mMarkets;
    std::map<std::tuple<VMarket::Type, Id>, Key> mItems;
    std::vector<std::pair<Key, int>> mCounts;
    GuiTable* mTable;

    // Items
    void addItem(VMarket::Type type, Id itemId);
    void removeItem(VMarket::Type type, Id itemId);

    // Rows
    std::size_t getRow(const Key& key) const;
    void addRow(const Building* building, Good::Type goodType, Money price, int count);
    void updateRow(std::size_t i, int count);

    // Util
    Market<Good>* getMarket(VMarket::Type type);
};
