//
// Created by 何宇 on 2018/9/24.
//

#include "Target.h"

Target::Target(Scene::Transform transform, int velocity) {
    this->velocity = velocity;
    this->transform = transform;
    this->position.x = transform.position.x;
    this->position.y = transform.position.y;
}

void Target::update(float time) {
    transform.position.x += velocity * 4 * time;
    position.x = transform.position.x;
}

bool Target::hit_detect(glm::vec2 hook_position) {
    float distance = glm::distance(position, hook_position);

    if (distance < 0.1) {
        return true;
    }

    return false;
}