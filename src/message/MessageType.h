#pragma once

enum class MessageType
{
    INPUT,
    PUSH_GAME_STATE,
    POP_GAME_STATE,
    CHANGE_GAME_STATE,
};

inline std::ostream& operator<<(std::ostream& os, const MessageType& type)
{
    switch (type)
    {
        case MessageType::INPUT:
            os << "input";
            break;
        case MessageType::PUSH_GAME_STATE:
            os << "push game state";
            break;
        case MessageType::POP_GAME_STATE:
            os << "pop game state";
            break;
        case MessageType::CHANGE_GAME_STATE:
            os << "change game state";
            break;
    }
    return os;
}