#include "chat_server.hpp"
#include "chat_service.hpp"
#include <iostream>
#include <signal.h>

#define PORT 999

using namespace std;

void resetCallback(int) {
    ChatService::getInstance()->resetState();
    exit(0);
} 

int main() {
    
    signal(SIGINT, resetCallback);

    EventLoop loop;
    InetAddress inetAddr("127.0.0.1", PORT);
    ChatServer chatServer(&loop, inetAddr, "chat");

    chatServer.start();
    loop.loop();

    return 0;
}