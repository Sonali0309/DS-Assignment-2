#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <fstream>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 3000
#define MAX_CLIENTS 10

std::vector<SOCKET> clients;
std::map<SOCKET, std::string> client_names;
std::ofstream log_file("chat_history.txt", std::ios::app);

void log_message(const std::string& message) {
    log_file << message << std::endl;
}

void handle_client(SOCKET client_socket) {
    char buffer[1024];
    std::string username;

    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        closesocket(client_socket);
        return;
    }

    username = std::string(buffer, bytes_received);
    client_names[client_socket] = username;

    std::cout << username << " has joined the chat!" << std::endl;

    std::string join_msg = username + " has joined the chat!";
    log_message(join_msg);
    for (SOCKET client : clients) {
        if (client != client_socket) {
            send(client, join_msg.c_str(), join_msg.size(), 0);
        }
    }

    clients.push_back(client_socket);

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break;
        }

        std::string message(buffer, bytes_received);

        if (message[0] == '@') {
            size_t space_pos = message.find(' ', 1);
            if (space_pos != std::string::npos) {
                std::string target_user = message.substr(1, space_pos - 1);
                std::string dm_message = username + " (DM): " + message.substr(space_pos + 1);

                bool found = false;
                for (const auto& client : client_names) {
                    if (client.second == target_user) {
                        send(client.first, dm_message.c_str(), dm_message.size(), 0);
                        log_message(username + " -> " + target_user + " (DM): " + message.substr(space_pos + 1));
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    std::string error_msg = "User not found: " + target_user;
                    send(client_socket, error_msg.c_str(), error_msg.size(), 0);
                }
            }
        } else {
            std::string error_msg = "Invalid message. Please send only direct messages starting with @username.";
            send(client_socket, error_msg.c_str(), error_msg.size(), 0);
        }
    }

    clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
    client_names.erase(client_socket);
    std::string leave_msg = username + " has left the chat.";
    log_message(leave_msg);

    for (SOCKET client : clients) {
        send(client, leave_msg.c_str(), leave_msg.size(), 0);
    }

    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        return -1;
    }

    SOCKET server_socket;
    sockaddr_in server_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        WSACleanup();
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed!" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) {
        std::cerr << "Listen failed!" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    std::cout << "Server is running on port " << PORT << std::endl;

    sockaddr_in client_addr;
    int addr_len = sizeof(client_addr);

    while (true) {
        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed!" << std::endl;
            continue;
        }

        std::thread t(handle_client, client_socket);
        t.detach();
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
