//
// Created by 何宇 on 2018/9/24.
//

#ifndef FISHOFF_CRAB_H
#define FISHOFF_CRAB_H


#include "Target.h"

struct Crab : public Target {
    Crab() {}

    Crab(Scene::Transform transform, int velocity) : Target(transform, velocity) {}

    void on_hit(Player &player) override;
};


#endif //FISHOFF_CRAB_H
