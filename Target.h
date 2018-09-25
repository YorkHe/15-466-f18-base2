//
// Created by 何宇 on 2018/9/24.
//

#ifndef FISHOFF_TARGET_H
#define FISHOFF_TARGET_H

#include "Scene.hpp"
#include "Player.h"

#include <glm/glm.hpp>
#include <iostream>

struct Target {

    Target() {
        transform = Scene::Transform();
    }

    Target(Scene::Transform transform, int velocity);

    Scene::Transform transform = Scene::Transform();
    glm::vec2 position = glm::vec2(0, 0);
    int velocity = 0;
    int type = 0;
    int id = 0;

    Player *clingOn = nullptr;



    void update(float time);

    bool hit_detect(glm::vec2 hook_position);

    virtual void on_hit(Player &player) {
        std::cerr << "HITHITHITHIT" << std::endl;
        clingOn = &(player);
    };
};


#endif //FISHOFF_TARGET_H
