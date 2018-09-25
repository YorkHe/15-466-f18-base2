//
// Created by 何宇 on 2018/9/24.
//

#include "Target.h"
#include <iostream>

Target::Target(Scene::Transform transform, int velocity) {
    this->velocity = velocity;
    this->transform = transform;
    this->position.x = transform.position.x;
    this->position.y = transform.position.y;
}

void Target::update(float time) {

    if (clingOn != nullptr) {

        if (transform.position.y > 3.8) {
            transform.position.x = clingOn->position.x;
            transform.position.y = clingOn->position.y;
            std::cerr << "CLINGON" << clingOn << std::endl;
        } else {
            std::cerr << "CLINGON" << clingOn << std::endl;
            transform.position.x = clingOn->hook_position.x;
            std::cerr << "CLINGON" << clingOn << std::endl;
            transform.position.y = clingOn->hook_position.y;
            std::cerr << "CLINGON" << clingOn << std::endl;
        }

    } else {
        transform.position.x += velocity * time;
    }

    position.x = transform.position.x;
    position.y = transform.position.y;
}

bool Target::hit_detect(glm::vec2 hook_position) {
    float distance = glm::distance(position, hook_position);
    if (distance < 0.5) {
        std::cerr << "Hit" << distance << std::endl;
        return true;
    }
    return false;
}