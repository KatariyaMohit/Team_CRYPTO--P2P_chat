#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <sstream>
#include <set>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

mutex peer_mutex;
set<string> peers;
string team_name;

string getLocalIP() {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    struct addrinfo hints = {}, *res, *ptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        return "127.0.0.1";
    }

    for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
        struct sockaddr_in* addr = (struct sockaddr_in*)ptr->ai_addr;
        string ip = inet_ntoa(addr->sin_addr);
        if (ip != "127.0.0.1") {
            freeaddrinfo(res);
            return ip;
        }
    }
    freeaddrinfo(res);
    return "127.0.0.1";
}

void receiveMessages(SOCKET server_socket) {
    while (true) {
        struct sockaddr_in client_addr;
        int addr_len = sizeof(client_addr);
        char buffer[1024];

        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            cerr << "Error accepting connection" << endl;
            continue;
        }

        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            string received_msg(buffer);

            istringstream iss(received_msg);
            string sender_info, sender_team;
            getline(iss, sender_info, ' ');
            getline(iss, sender_team, ' ');
            string sender_message;
            getline(iss, sender_message);
            sender_message.erase(0, sender_message.find_first_not_of(" "));
            {
                lock_guard<mutex> lock(peer_mutex);
            if (sender_message == "exit") {
                peers.erase(sender_info);
                cout << "\n" << sender_info << " [" << sender_team << "] has disconnected." << endl;
            } else {
                peers.insert(sender_info);
                if (sender_message == "Connected!") {
                    cout << "\nConnected to " << sender_info << " [" << sender_team << "]" << endl;
                } else {
                    cout << "\nMessage from " << sender_info << " [" << sender_team << "]: " << sender_message << endl;
                }
            }
        }

        }
        closesocket(client_socket);
    }
}

void sendMessage(string my_ip, int my_port) {
    while (true) {
        cout << "\n1. Send Message\n2. Query Active Peers\n3. Connect to Peers\n0. Quit\nEnter choice: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            string target_ip, message;
            int target_port;
            cout << "Enter recipient's IP address: ";
            cin >> target_ip;
            cout << "Enter recipient's port number: ";
            cin >> target_port;
            cout << "Enter your message: ";
            cin.ignore();
            getline(cin, message);

            SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in serv_addr;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(target_port);
            inet_pton(AF_INET, target_ip.c_str(), &serv_addr.sin_addr);

            if (connect(client, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
                cerr << "Connection failed." << endl;
                continue;
            }

            string formatted_message = my_ip + ":" + to_string(my_port) + " " + team_name + " " + message;
            send(client, formatted_message.c_str(), formatted_message.size(), 0);
            cout << "Message sent!" << endl;
            closesocket(client);

            peer_mutex.lock();
            peers.insert(target_ip + ":" + to_string(target_port));
            peer_mutex.unlock();
        }
        else if (choice == 2) {
            cout << "\nQuerying active peers..." << endl;
            string query_message = my_ip + ":" + to_string(my_port) + " " + team_name + " query";

            this_thread::sleep_for(chrono::seconds(2));

            cout << "\nActive Peers:" << endl;
            peer_mutex.lock();
            for (const auto& peer : peers) {
                cout << peer << "\n";
            }
            if (peers.empty()) {
                cout << "No connected peers." << endl;
            }
            peer_mutex.unlock();
        }
        else if (choice == 3) {
            cout << "\n1. Connect to known peers\n2. Connect to a new peer\nEnter choice: ";
            int connect_choice;
            cin >> connect_choice;

            if (connect_choice == 1) {
                cout << "\nConnecting to known peers..." << endl;
                peer_mutex.lock();
                for (const auto& peer : peers) {
                    size_t pos = peer.find(":");
                    if (pos == string::npos) continue;
                    string ip = peer.substr(0, pos);
                    int port;
                    try {
                        port = stoi(peer.substr(pos + 1));
                    } catch (...) {
                        continue;
                    }

                    SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
                    struct sockaddr_in serv_addr;
                    serv_addr.sin_family = AF_INET;
                    serv_addr.sin_port = htons(port);
                    inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);

                    if (connect(client, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != SOCKET_ERROR) {
                        string connect_message = my_ip + ":" + to_string(my_port) + " " + team_name + " Connected!";
                        send(client, connect_message.c_str(), connect_message.size(), 0);
                        cout << "Connected to " << ip << ":" << port << endl;
                    }
                    closesocket(client);
                }
                peer_mutex.unlock();
            }
            else if (connect_choice == 2) {
                string new_ip;
                int new_port;
                cout << "Enter new peer's IP: ";
                cin >> new_ip;
                cout << "Enter new peer's port: ";
                cin >> new_port;

                SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(new_port);
                inet_pton(AF_INET, new_ip.c_str(), &serv_addr.sin_addr);

                if (connect(client, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != SOCKET_ERROR) {
                    string connect_message = my_ip + ":" + to_string(my_port) + " " + team_name + " Connected!";
                    send(client, connect_message.c_str(), connect_message.size(), 0);
                    cout << "Connected to " << new_ip << ":" << new_port << endl;
                    peer_mutex.lock();
                    peers.insert(new_ip + ":" + to_string(new_port));
                    peer_mutex.unlock();
                }
                else {
                    cout << "Failed to connect to the new peer." << endl;
                }
                closesocket(client);
            }
        }
        else if (choice == 0) {
            peer_mutex.lock();
            for (const auto& peer : peers) {
                size_t pos = peer.find(":");
                if (pos == string::npos) continue;
                string ip = peer.substr(0, pos);
                int port = stoi(peer.substr(pos + 1));

                SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port);
                inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);

                if (connect(client, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != SOCKET_ERROR) {
                    string exit_message = my_ip + ":" + to_string(my_port) + " " + team_name + " exit";
                    send(client, exit_message.c_str(), exit_message.size(), 0);
                }
                closesocket(client);
            }
            peer_mutex.unlock();
            break;
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    string my_ip = getLocalIP();
    int my_port;
    cout << "Enter your team name: ";
    cin >> team_name;
    cout << "Enter your port number: ";
    cin >> my_port;

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(my_port);
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);
    cout << "Server listening on " << my_ip << ":" << my_port << endl;

    thread recv_thread(receiveMessages, server_socket);
    sendMessage(my_ip, my_port);

    closesocket(server_socket);
    WSACleanup();
    return 0;
}