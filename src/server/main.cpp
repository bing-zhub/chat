#include "chat_server.hpp"
#include "chat_service.hpp"
#include <iostream>
#include <signal.h>

using namespace std;

void resetCallback(int) {
    ChatService::getInstance()->resetState();
    exit(0);
} 

int main(int argc, char* argv[]) {
    
    if(argc < 2) { return 0; }
    int port = atoi(argv[1]);

    signal(SIGINT, resetCallback);

    EventLoop loop;
    InetAddress inetAddr("127.0.0.1", port);
    ChatServer chatServer(&loop, inetAddr, "chat");

    chatServer.start();
    loop.loop();

    return 0;
}