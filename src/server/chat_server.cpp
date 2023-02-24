#include "chat_server.hpp"
#include "chat_service.hpp"

using namespace std;
using json = nlohmann::json;

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
    if(conn->connected()) {
        LOG_INFO << "[Network] Connected: " << conn->peerAddress().toIpPort() << " ---> " << conn->localAddress().toIpPort();
    } else {
        // 客户端断开连接
        LOG_INFO << "[Network] Disconnected: " << conn->peerAddress().toIpPort() << " -x-> " << conn->localAddress().toIpPort();
        ChatService::getInstance()->clientCloseException(conn);
        conn->shutdown();
    }
}

// 读写事件相关回调
void  ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time) {
    string data = buffer->retrieveAllAsString();
    json js = json::parse(data);

    // 解耦网络模块代码与业务模块代码
    try {
        auto handler = ChatService::getInstance()->getHandler(js["msg_id"].get<int>());
        handler(conn, js, time);
    } catch (json::exception& e) {
        LOG_ERROR << e.what();
    }
}