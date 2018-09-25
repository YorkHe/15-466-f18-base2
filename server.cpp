#include "Connection.hpp"
#include "Game.hpp"
#include "Target.h"

#include "glm/glm.hpp"

#include <iostream>
#include <set>
#include <chrono>
#include <random>
#include <vector>

int id_counter = 0;

std::vector<Target *> target_list;

void syncPosition(Connection *c, float x, float y) {
    c->send_raw("s", 1);
    c->send_raw(&(x), sizeof(float));
    c->send_raw(&(y), sizeof(float));
}

void getRandomTarget() {

    int n = (std::rand() % 100);

    bool should = n < 2;

    if (!should) return;

    int type = ((std::rand() % 100) < 20) ? 1 : 0;
    int direction = (std::rand() % 2);
    float y = ((std::rand() % 800) / 100.0f) - 5.0f;
    int speed = (std::rand() % 3) + 4;

    std::cerr << "G:" << type << "," << direction << "," << y << "," << speed << std::endl;

    Scene::Transform trans = Scene::Transform();
    trans.position.x = -15;
    trans.position.y = y;
    Target *t = new Target(trans, speed);
    t->id = id_counter++;
    t->type = type;
    if (direction == 0) {
        t->velocity = speed;
    } else {
        if (type == 1) {
            t->velocity = speed;
        } else {
            t->velocity = -speed;
            t->transform.position.x = 15;
        }
    }

    target_list.emplace_back(t);
}

void syncTarget(Connection *c) {
    if (c == nullptr) return;

    c->send_raw("g", 1);
    size_t size = target_list.size();
    c->send_raw(&size, sizeof(size_t));
    for (int i = 0; i < size; i++) {
        Target *t = target_list[i];
        c->send_raw(&(t->id), sizeof(int));
        c->send_raw(&(t->type), sizeof(int));
        c->send_raw(&(t->velocity), sizeof(int));
        c->send_raw(&(t->position.x), sizeof(float));
        c->send_raw(&(t->position.y), sizeof(float));
    }
}

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Usage:\n\t./server <port>" << std::endl;
		return 1;
	}

    std::srand(0x15666);
	Server server(argv[1]);

//	Game state;

    Connection *player_one_conn = nullptr;
    Connection *player_two_conn = nullptr;

    Player player_one;
    Player player_two;


	while (1) {
		server.poll([&](Connection *c, Connection::Event evt){
			if (evt == Connection::OnOpen) {
                std::cout << "Open" << c->recv_buffer.size() << std::endl;
			} else if (evt == Connection::OnClose) {
                std::cout << "Close" << c->recv_buffer.size() << std::endl;
			} else { assert(evt == Connection::OnRecv);
//                std::cout << "Buffer" << c->recv_buffer.size() << "::" << (char) (c->recv_buffer[0])
//                          << (int) (c->recv_buffer[0]) << std::endl;
				if (c->recv_buffer[0] == 'h') {
					c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() + 1);
                    std::cout << c << ": Got hello." << c->recv_buffer.size() << std::endl;
                    if (player_one_conn == nullptr) {
                        player_one_conn = c;
                        c->send_raw("o1", 2);
                    } else if (player_two_conn == nullptr) {
                        player_two_conn = c;
                        c->send_raw("o2", 2);
                    } else {
                        std::string msg = "There can only be two active messages.";
                        c->send_raw(&msg, msg.size());
                    }
				} else if (c->recv_buffer[0] == 's') {
//                    std::cout << c << ": Got sync." << std::endl;
                    if (c->recv_buffer.size() < 1 + sizeof(int) + sizeof(float) + sizeof(float)) {
						return; //wait for more data
					} else {
//                        std::cout << c << ": Start parsing." << c->recv_buffer.size() << std::endl;
                        int order = 0;
                        int offset = 1;
                        memcpy(&order, c->recv_buffer.data() + offset, sizeof(int));
//                        std::cout << "Order" << order << std::endl;
                        offset += sizeof(int);
                        if (order == 1) {
                            memcpy(&(player_one.position.x), c->recv_buffer.data() + offset, sizeof(float));
                            offset += sizeof(float);
                            memcpy(&(player_one.position.y), c->recv_buffer.data() + offset, sizeof(float));
                            offset += sizeof(float);
                            memcpy(&(player_one.hook_position.x), c->recv_buffer.data() + offset, sizeof(float));
                            offset += sizeof(float);
                            memcpy(&(player_one.hook_position.y), c->recv_buffer.data() + offset, sizeof(float));
                            offset += sizeof(float);
                            c->recv_buffer.clear();

                        } else if (order == 2) {
                            memcpy(&(player_two.position.x), c->recv_buffer.data() + offset, sizeof(float));
                            offset += sizeof(float);
                            memcpy(&(player_two.position.y), c->recv_buffer.data() + offset, sizeof(float));
                            offset += sizeof(float);
                            memcpy(&(player_two.hook_position.x), c->recv_buffer.data() + offset, sizeof(float));
                            offset += sizeof(float);
                            memcpy(&(player_two.hook_position.y), c->recv_buffer.data() + offset, sizeof(float));
                            offset += sizeof(float);

                            c->recv_buffer.clear();
                        }
//                        std::cout << c << ": After parsing." << c->recv_buffer.size() << std::endl;
					}
				}
			}
		}, 0.01);
		//every second or so, dump the current paddle position:
		static auto then = std::chrono::steady_clock::now();
		auto now = std::chrono::steady_clock::now();
        if (now > then + std::chrono::milliseconds(1000 / 60)) {

            getRandomTarget();
            for (auto it = target_list.begin(); it != target_list.end(); ++it) {
                Target *target = *it;
                target->update(1 / 60.0f);
                if ((target->position.x > 16) || (target->position.x < -20)) {
                    target_list.erase(it);
                }
            }

            then = now;
            if (player_two_conn != nullptr) {
                player_two_conn->send_raw("s", 1);
                player_two_conn->send_raw(&(player_one.position.x), sizeof(float));
                player_two_conn->send_raw(&(player_one.position.y), sizeof(float));
                player_two_conn->send_raw(&(player_one.hook_position.x), sizeof(float));
                player_two_conn->send_raw(&(player_one.hook_position.y), sizeof(float));
            }

            if (player_one_conn != nullptr) {
                player_one_conn->send_raw("s", 1);
                player_one_conn->send_raw(&(player_two.position.x), sizeof(float));
                player_one_conn->send_raw(&(player_two.position.y), sizeof(float));
                player_one_conn->send_raw(&(player_two.hook_position.x), sizeof(float));
                player_one_conn->send_raw(&(player_two.hook_position.y), sizeof(float));
            }

            syncTarget(player_one_conn);
            syncTarget(player_two_conn);
		}
	}
}
