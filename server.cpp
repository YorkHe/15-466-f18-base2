#include "Connection.hpp"
#include "Game.hpp"

#include "glm/glm.hpp"

#include <iostream>
#include <set>
#include <chrono>

void syncPosition(Connection *c, float x, float y) {
    c->send_raw("s", 1);
    c->send_raw(&(x), sizeof(float));
    c->send_raw(&(y), sizeof(float));
}

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Usage:\n\t./server <port>" << std::endl;
		return 1;
	}
	
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
                std::cout << "Buffer" << c->recv_buffer.size() << "::" << (char) (c->recv_buffer[0])
                          << (int) (c->recv_buffer[0]) << std::endl;
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
                    std::cout << c << ": Got sync." << std::endl;
                    if (c->recv_buffer.size() < 1 + sizeof(int) + sizeof(float) + sizeof(float)) {
						return; //wait for more data
					} else {
                        std::cout << c << ": Start parsing." << c->recv_buffer.size() << std::endl;
                        int order = 0;
                        int offset = 1;
                        memcpy(&order, c->recv_buffer.data() + offset, sizeof(int));
                        std::cout << "Order" << order << std::endl;
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
                            if (player_two_conn != nullptr) {
                                player_two_conn->send_raw("s", 1);
                                player_two_conn->send_raw(&(player_one.position.x), sizeof(float));
                                player_two_conn->send_raw(&(player_one.position.y), sizeof(float));
                                player_two_conn->send_raw(&(player_one.hook_position.x), sizeof(float));
                                player_two_conn->send_raw(&(player_one.hook_position.y), sizeof(float));
                            }
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
                            if (player_one_conn != nullptr) {
                                player_one_conn->send_raw("s", 1);
                                player_one_conn->send_raw(&(player_two.position.x), sizeof(float));
                                player_one_conn->send_raw(&(player_two.position.y), sizeof(float));
                                player_one_conn->send_raw(&(player_two.hook_position.x), sizeof(float));
                                player_one_conn->send_raw(&(player_two.hook_position.y), sizeof(float));
                            }
                        }
                        std::cout << c << ": After parsing." << c->recv_buffer.size() << std::endl;
					}
				}
			}
		}, 0.01);
		//every second or so, dump the current paddle position:
		static auto then = std::chrono::steady_clock::now();
		auto now = std::chrono::steady_clock::now();
		if (now > then + std::chrono::seconds(1)) {
			then = now;
            std::cerr << "Current player 1 position" << player_one.position.x << std::endl;
            std::cerr << "Current player 2 position" << player_two.position.x << std::endl;
		}
	}
}
