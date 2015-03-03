#ifndef GAMETIME_H
#define GAMETIME_H

#include <chrono>
#include <iostream>

#include "BrickwareCoreDLL.h"

class GameTime
{
	friend class Game;

public:
	static BRICKWARE_CORE_API float GetDeltaTime();
	static BRICKWARE_CORE_API long long GetMillisSinceStart();

private:
	static long long startTime;
	static long long frameStartTime;

	static float deltaTime;

	static void frameStart();
	static void frameEnd();
};

#endif