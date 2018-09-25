#pragma once

#include "Player.h"
#include "Target.h"

#include <glm/glm.hpp>

#include <vector>

struct Game {

    Game() {}

    Player player;
    Player opponent;

    std::vector<Target *> target_list;


    int order = 1.0f;


	void update(float time);

    static constexpr const float FrameWidth = 20.0f;
	static constexpr const float FrameHeight = 8.0f;
    static constexpr const float PlayerWidth = 2.0f;
    static constexpr const float PlayerHeight = 0.4f;
	static constexpr const float BallRadius = 0.5f;
};
