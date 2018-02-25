#include "game/GameStateStart.h"
#include "message/MessageBus.h"
#include "render/RenderEngine.h"
#include "input/InputEngine.h"
#include "resource/TextureManager.h"
#include "resource/StylesheetManager.h"
#include "gui/GuiButton.h"
#include "gui/GuiImage.h"
#include "gui/GuiText.h"
#include "gui/GuiVBoxLayout.h"

GameStateStart::GameStateStart() :
    mGui(sf::Vector2f(sRenderEngine->getWindow().getSize()))
{
    // Gui
    createGui();

    // Subscribe to inputs
    sInputEngine->subscribe(mMailbox.getId());
}

GameStateStart::~GameStateStart()
{
    sInputEngine->unsubscribe(mMailbox.getId());
}

void GameStateStart::draw(const float dt)
{
    sRenderEngine->draw(mGui);
}

void GameStateStart::update(const float dt)
{

}

void GameStateStart::handleMessages()
{
    mGui.update();
    while (!mMailbox.isEmpty())
    {
        Message message = mMailbox.get();
        if (message.type == MessageType::INPUT)
        {
            sf::Event event = message.getInfo<sf::Event>();
            switch (event.type)
            {
                case sf::Event::Closed:
                    sRenderEngine->getWindow().close();
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape)
                        sRenderEngine->getWindow().close();
                    else if (event.key.code == sf::Keyboard::Space)
                        loadGame();
                    break;
                default:
                    break;
            }
        }
        if (message.type == MessageType::GUI)
        {
            std::string info = message.getInfo<std::string>();
            if (info == "load_game")
                loadGame();
            else if (info == "exit")
                sRenderEngine->getWindow().close();
        }
    }
}

void GameStateStart::createGui()
{
    // Background
    auto background = mGui.createRoot<GuiImage>("background", sTextureManager->getTexture("background"));

    // Load button
    auto loadGameButton = mGui.create<GuiButton>("loadGameButton", sStylesheetManager->getStylesheet("button"),
        "Load Game", sf::Vector2f(192, 32), 24, Message::create<std::string>(MessageType::GUI, "load_game"));

    // Exit button
    auto exitButton = mGui.create<GuiButton>("exitButton", sStylesheetManager->getStylesheet("button"),
        "Exit", sf::Vector2f(192, 32), 24, Message::create<std::string>(MessageType::GUI, "exit"));

    auto menu = mGui.createRoot<GuiWidget>("menu");
    menu->setSize(sf::Vector2f(sRenderEngine->getWindow().getSize()));
    menu->add(loadGameButton);
    menu->add(exitButton);
    std::unique_ptr<GuiVBoxLayout> menuLayout(new GuiVBoxLayout(GuiLayout::HAlignment::Center, GuiLayout::VAlignment::Center));
    menuLayout->setSpacing(16.0f);
    menu->setLayout(std::move(menuLayout));

    // Register to events
    loadGameButton->subscribe(mMailbox.getId());
    exitButton->subscribe(mMailbox.getId());
}

void GameStateStart::loadGame()
{
    sMessageBus->send(Message::create(sGameId, MessageType::PUSH_GAME_STATE, GameStateName::EDITOR));
}
