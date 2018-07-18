#pragma once

#include "ai/Goal.h"
#include <SFML/System/Clock.hpp>

class GoalWait : public Goal
{
public:
    GoalWait(Person* owner, float duration);
    virtual ~GoalWait();

    virtual void activate() override;
    virtual State process() override;
    virtual void terminate() override;

private:
    float mDuration;
    sf::Clock mClock;
};
