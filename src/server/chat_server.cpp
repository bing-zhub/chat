#include "chat_server.hpp"
#include <functional>

using namespace std;

ChatServer::ChatServer(EventLoop *loop, 
                       const InetAddress& listenAddr, 
                       const string& nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop){
    
    // 注册连接回调
    // 成员函数有两个参数, muduo库只有一个参数, 所以把this绑定到新函数
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, placeholders::_1));

    // 消息回调
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));

    _server.setThreadNum(4);
}

void ChatServer::start() { _server.start(); }

// 用户连接相关回调
void ChatServer::onConnection(const TcpConnectionPtr& conn) {
    
}
// 读写事件相关回调
void  ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time) {

}