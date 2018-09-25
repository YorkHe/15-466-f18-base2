//
// Created by 何宇 on 2018/9/24.
//

#include "Player.h"

#include <algorithm>
#include <iostream>

Player::Player(glm::vec2 position) {
    this->position = position;
    this->hook_position = position;
}

void Player::update(float time) {
    if (!drop) {
        if (direction == 1) {
            position.x -= 4 * time;
            position.x = std::max(position.x, -0.5f * 20.0f + 0.5f * 2.0f);
            position.x = std::min(position.x, 0.5f * 20.0f - 0.5f * 2.0f);

            hook_position.x = position.x;
        } else if (direction == 2) {
            position.x += 4 * time;
            position.x = std::max(position.x, -0.5f * 20.0f + 0.5f * 2.0f);
            position.x = std::min(position.x, 0.5f * 20.0f - 0.5f * 2.0f);

            hook_position.x = position.x;
        }
    } else {
        hook_position.y += hook_direction * 4 * time;

        if (hook_position.y > position.y) {
            hook_position.y = position.y;
            hook_direction = 0;
            drop = false;
        }

        if (hook_position.y < -5.5) {
            hook_direction = 1;
        }
    }
}


