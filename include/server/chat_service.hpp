#ifndef CHAT_SERVICE_H
#define CHAT_SERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <muduo/base/Logging.h>
#include <string>

#include "json.hpp"
#include "public.hpp"

using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

// 处理消息的时间回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>;

// 聊天服务器业务类 单例模式
class ChatService {
public:
    // 获取单例对象的接口函数
    static ChatService* getInstance();
    // 登录
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 注册
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 获取消息对应的处理器
    MsgHandler getHandler(int msg_id);
private:
    ChatService();

    // 存储消息id和handler对应关系
    unordered_map<int, MsgHandler> _handlerMap;
};

#endif
