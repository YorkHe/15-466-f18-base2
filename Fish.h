//
// Created by 何宇 on 2018/9/24.
//

#ifndef FISHOFF_FISH_H
#define FISHOFF_FISH_H

#include "Target.h"

struct Fish : public Target {
    Fish(Scene::Transform transform, int velocity) : Target(transform, velocity) {}

    void on_hit(Player &player) override;
};


#endif //FISHOFF_FISH_H
