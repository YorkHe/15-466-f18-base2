//
// Created by 何宇 on 2018/9/24.
//

#pragma once

#include "Mode.hpp"
#include "Connection.hpp"
#include "Game.hpp"
#include "GL.hpp"


struct FishMode : public Mode {
    FishMode(Client &client);

    virtual ~FishMode();

    bool handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) override;

    void update(float elapsed) override;

    void draw(glm::uvec2 const &drawable_size) override;

    void addTarget(int type);

    Game state;

    Client &client;
};


