#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <functional>
#include <string>

#include "json.hpp"  

using namespace muduo;
using namespace muduo::net;

class ChatServer {
public:
    // 初始化聊天服务器对象
    ChatServer(EventLoop *loop, const InetAddress& listenAddr, const string& nameArg);
    // 启动服务
    void start();

private:
    // 用户连接相关回调
    void onConnection(const TcpConnectionPtr& conn);
    // 读写事件相关回调
    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time);
    
    TcpServer _server;
    EventLoop *_loop;
};

#endif