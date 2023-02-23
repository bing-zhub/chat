#include "chat_server.hpp"
#include "chat_service.hpp"
#include <iostream>

#define PORT 999

using namespace std;

int main() {
    
    ChatService::getInstance()->resetState();

    EventLoop loop;
    InetAddress inetAddr("127.0.0.1", PORT);
    ChatServer chatServer(&loop, inetAddr, "chat");

    chatServer.start();
    loop.loop();

    return 0;
}