#include "GoalWait.h"
#include "city/City.h"
#include "city/Person.h"

GoalWait::GoalWait(Person* owner, float nbHours) :
    Goal(owner), mDuration(mOwner->getCity()->toHumanTime(nbHours))
{
    //ctor
}

GoalWait::~GoalWait()
{
    //dtor
}

void GoalWait::activate()
{
    mState = State::ACTIVE;
    mClock.restart();
}

Goal::State GoalWait::process()
{
    activateIfInactive();

    if (mClock.getElapsedTime().asSeconds() >= mDuration)
        mState = State::COMPLETED;

    return mState;
}

void GoalWait::terminate()
{

}
