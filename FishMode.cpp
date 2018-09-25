//
// Created by 何宇 on 2018/9/24.
//
#include "FishMode.h"
#include "gl_errors.hpp"
#include "Load.hpp"
#include "MeshBuffer.hpp"
#include "Scene.hpp"
#include "vertex_color_program.hpp"
#include "data_path.hpp"
#include "Fish.h"
#include "Crab.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


Load<MeshBuffer> meshes(LoadTagDefault, []() {
    return new MeshBuffer(data_path("fishing.pnc"));
});

Load<GLuint> meshes_for_vertex_color_program(LoadTagDefault, []() {
    return new GLuint(meshes->make_vao_for_program(vertex_color_program->program));
});

Scene::Transform *player_one_transformation = nullptr;
Scene::Transform *player_one_hook_transformation = nullptr;
Scene::Transform *player_two_transformation = nullptr;
Scene::Transform *player_two_hook_transformation = nullptr;
Scene::Transform *crab_transformation = nullptr;
Scene::Transform *fish_left_transformation = nullptr;
Scene::Transform *fish_right_transformation = nullptr;


Scene::Camera *camera = nullptr;

auto newObject = [](Scene &s, Scene::Transform *t, std::string const &m) {
    Scene::Object *obj = s.new_object(t);
    obj->program = vertex_color_program->program;

    obj->program_mvp_mat4 = vertex_color_program->object_to_clip_mat4;
    obj->program_mv_mat4x3 = vertex_color_program->object_to_light_mat4x3;
    obj->program_itmv_mat3 = vertex_color_program->normal_to_light_mat3;

    MeshBuffer::Mesh const &mesh = meshes->lookup(m);
    obj->vao = *meshes_for_vertex_color_program;
    obj->start = mesh.start;
    obj->count = mesh.count;
    std::cerr << "OBJECT " << m << std::endl;
    std::cerr << "MESH " << obj->start << "," << obj->count << std::endl;
};

Load<Scene> scene(LoadTagDefault, []() {
    Scene *ret = new Scene;
    ret->load(data_path("fishing.scene"), newObject);

    for (Scene::Transform *t = ret->first_transform; t != nullptr; t = t->alloc_next) {
        if (t->name == "Player_1") {
            player_one_transformation = t;
        }
        if (t->name == "Hook") {
            player_one_hook_transformation = t;
        }
        if (t->name == "Player_2") {
            player_two_transformation = t;
        }
        if (t->name == "Hook2") {
            player_two_hook_transformation = t;
        }
        if (t->name == "Fish_Left") {
            fish_left_transformation = t;
        }
        if (t->name == "Fish_Right") {
            fish_right_transformation = t;
        }
        if (t->name == "Crab") {
            crab_transformation = t;
        }
    }

    for (Scene::Camera *c = ret->first_camera; c != nullptr; c = c->alloc_next) {
        if (c->transform->name == "Camera") {
            if (camera) throw std::runtime_error("Multiple 'Camera' objects in scene. ");
            camera = c;
        }
    }

    if (!camera) throw std::runtime_error("No camera in scene");
    return ret;
});

FishMode::FishMode(Client &client) : client(client) {
    this->client.connection.send_raw("h", 1);
    state.player.player_transformation = player_one_transformation;
    state.player.hook_transformation = player_one_hook_transformation;
    addTarget(1);
}

FishMode::~FishMode() {
}

bool FishMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
    if (evt.type == SDL_KEYDOWN && evt.key.repeat) {
        return false;
    }

    if (evt.type == SDL_KEYDOWN) {
        if (evt.key.keysym.scancode == SDL_SCANCODE_A) {
            state.player.direction = 1;
        }

        if (evt.key.keysym.scancode == SDL_SCANCODE_D) {
            state.player.direction = 2;
        }

        if (evt.key.keysym.scancode == SDL_SCANCODE_SPACE) {
            std::cerr << state.player.hook_direction << std::endl;
            state.player.drop = true;
            if (state.player.hook_direction == 0) {
                state.player.hook_direction = -1;
            } else if (state.player.hook_direction == -1) {
                state.player.hook_direction = -state.player.hook_direction;
            }
        }
    }

    if (evt.type == SDL_KEYUP) {
        state.player.direction = 0;
    }

    return false;
}

void FishMode::addTarget(int type) {
    if (type == 0) {
        if (rand() % 2 == 0) {
            Fish *fish = new Fish(*fish_left_transformation, 1);
            state.target_list.emplace_back(fish);

            newObject(const_cast<Scene &>(*scene), &(fish->transform), "Fish_Left");
            std::cerr << fish->position.x << "," << fish->position.y << std::endl;
        } else {
            Fish *fish = new Fish(*fish_right_transformation, -1);
            state.target_list.emplace_back(fish);

            newObject(const_cast<Scene &>(*scene), &(fish->transform), "Fish_Right");
            std::cerr << fish->position.x << "," << fish->position.y << std::endl;
        }
    } else if (type == 1) {
        Crab *crab = new Crab(*crab_transformation, 1);
        state.target_list.emplace_back(crab);
        newObject(const_cast<Scene &>(*scene), &(crab->transform), "Crab");
        std::cerr << crab->position.x << "," << crab->position.y << std::endl;
    }
}

void FishMode::update(float elapsed) {
//    state.update(elapsed);

    if (state.order == 0) return;

    if (state.order == 1) {
        state.player.player_transformation = player_one_transformation;
        state.player.hook_transformation = player_one_hook_transformation;
        state.opponent.player_transformation = player_two_transformation;
        state.opponent.hook_transformation = player_two_hook_transformation;
    } else {
        state.player.player_transformation = player_two_transformation;
        state.player.hook_transformation = player_two_hook_transformation;
        state.opponent.player_transformation = player_one_transformation;
        state.opponent.hook_transformation = player_one_hook_transformation;
    }

    if (client.connection && (state.player.direction != 0 || state.player.drop)) {
//        state.order += 1;
        client.connection.send_raw("s", 1);
        client.connection.send_raw(&state.order, sizeof(int));
        client.connection.send_raw(&(state.player.position.x), sizeof(float));
        client.connection.send_raw(&(state.player.position.y), sizeof(float));
        client.connection.send_raw(&(state.player.hook_position.x), sizeof(float));
        client.connection.send_raw(&(state.player.hook_position.y), sizeof(float));
    }

    client.poll([&](Connection *c, Connection::Event event) {
        if (event == Connection::OnOpen) {
        } else if (event == Connection::OnClose) {

        } else {
            assert(event == Connection::OnRecv);
            if (c->recv_buffer[0] == 'o') {
                std::cerr << c->recv_buffer[0] << c->recv_buffer[1] << std::endl;
                if (c->recv_buffer[1] == '1') {
                    state.order = 1;
                } else if (c->recv_buffer[1] == '2') {
                    state.order = 2;
                }
            } else if (c->recv_buffer[0] == 's') {
                float x, y;
                memcpy(&(x), c->recv_buffer.data() + 1, sizeof(float));
                memcpy(&(y), c->recv_buffer.data() + 1 + sizeof(float), sizeof(float));
                state.opponent.player_transformation->position.x = x;
                state.opponent.player_transformation->position.y = y;
                memcpy(&(x), c->recv_buffer.data() + 1 + sizeof(float) + sizeof(float), sizeof(float));
                memcpy(&(y), c->recv_buffer.data() + 1 + sizeof(float) + sizeof(float) + sizeof(float), sizeof(float));
                state.opponent.hook_transformation->position.x = x;
                state.opponent.hook_transformation->position.y = y;
                std::cerr << c->recv_buffer[0] << x << "," << y << std::endl;
            }
            c->recv_buffer.clear();
        }
    });

    state.player.update(elapsed);

    for (Target *target: state.target_list) {
        target->update(elapsed);
        if (target->hit_detect(state.player.hook_position)) {
            target->on_hit(state.player);
        }
    }

    state.player.hook_transformation->position.x = state.player.hook_position.x;
    state.player.hook_transformation->position.y = state.player.hook_position.y;

    state.player.player_transformation->position.x = state.player.position.x;
    state.player.player_transformation->position.y = state.player.position.y;
}

void FishMode::draw(glm::uvec2 const &drawable_size) {
    camera->aspect = drawable_size.x / float(drawable_size.y);


    glClearColor(0.25f, 0.0f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set up basic OpenGL state:
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //set up light positions:
    glUseProgram(vertex_color_program->program);

    glUniform3fv(vertex_color_program->sun_color_vec3, 1, glm::value_ptr(glm::vec3(0.81f, 0.81f, 0.76f)));
    glUniform3fv(vertex_color_program->sun_direction_vec3, 1,
                 glm::value_ptr(glm::normalize(glm::vec3(-0.2f, 0.2f, 1.0f))));
    glUniform3fv(vertex_color_program->sky_color_vec3, 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.3f)));
    glUniform3fv(vertex_color_program->sky_direction_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));

    scene->draw(camera);

    GL_ERRORS();
}



