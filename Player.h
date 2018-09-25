//
// Created by 何宇 on 2018/9/24.
//

#ifndef FISHOFF_PLAYER_H
#define FISHOFF_PLAYER_H

#include "Scene.hpp"
#include <glm/glm.hpp>

struct Player {

    Player() {}

    Player(glm::vec2 position);

    glm::vec2 position = glm::vec2(-5.0f, 4.0f);
    glm::vec2 hook_position = glm::vec2(-5.0f, 4.0f);

    Scene::Transform *player_transformation = nullptr;
    Scene::Transform *hook_transformation = nullptr;

    int direction = 0;
    int hook_direction = 0;
    bool drop = false;

    void update(float time);
};


#endif //FISHOFF_PLAYER_H
