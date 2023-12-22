#include "ActionBus.h"
#include "../YawVR.h"

//#include <queue> 

#include "ActionBus.h"

void ActionBus::Tick()
{
	/*
    lock (actionQueue)
    {
        while (actionQueue.size > 0)
        {
            Action action = actionQueue.front();
            action();
            actionQueue.pop();
        }
    }
	*/
}

void ActionBus::Add(Action action)
{
	/*
    lock (actionQueue)
    {
        actionQueue.push(action);
    }
	*/
}
