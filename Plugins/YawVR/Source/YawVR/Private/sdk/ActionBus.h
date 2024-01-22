#pragma once

#include <queue>

//TODO find a proper equivilant for Action
// https://stackoverflow.com/questions/12451554/c-way-of-c-sharp-action-function
using f_void = void(*)(...);
//template<class T>
class Action {
public:
	f_void callback;
	Action(f_void c) : callback(c) {

	}
	void run() {
		callback();
	}
};

class ActionBus
{
public:
    static std::queue<Action> actionQueue;
    static TSharedPtr<ActionBus> instance;
    ActionBus () {};

    void Tick(); //void Update();

    void Add(Action action);
};
