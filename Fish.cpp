//
// Created by 何宇 on 2018/9/24.
//

#include "Fish.h"
#include <iostream>

void Fish::on_hit(Player &player) {
    std::cerr << "HITHITHIT" << std::endl;
    clingOn = &(player);
}