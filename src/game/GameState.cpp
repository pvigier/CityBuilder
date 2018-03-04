#include "game/GameState.h"
#include "message/MessageBus.h"

MessageBus* GameState::sMessageBus = nullptr;
Id GameState::sGameId = UNDEFINED;
RenderEngine* GameState::sRenderEngine = nullptr;
InputEngine* GameState::sInputEngine = nullptr;
TextureManager* GameState::sTextureManager = nullptr;
StylesheetManager* GameState::sStylesheetManager = nullptr;
GuiManager* GameState::sGuiManager = nullptr;

GameState::GameState()
{
    // Register the mailbox
    sMessageBus->addMailbox(mMailbox);
}

GameState::~GameState()
{
    sMessageBus->removeMailbox(mMailbox);
}

void GameState::setMessageBus(MessageBus* messageBus)
{
    sMessageBus = messageBus;
}

void GameState::setGameId(Id gameId)
{
    sGameId = gameId;
}

void GameState::setRenderEngine(RenderEngine* renderEngine)
{
    sRenderEngine = renderEngine;
}

void GameState::setInputEngine(InputEngine* inputEngine)
{
    sInputEngine = inputEngine;
}

void GameState::setTextureManager(TextureManager* textureManager)
{
    sTextureManager = textureManager;
}

void GameState::setStylesheetManager(StylesheetManager* stylesheetManager)
{
    sStylesheetManager = stylesheetManager;
}

void GameState::setGuiManager(GuiManager* guiManager)
{
    sGuiManager = guiManager;
}
