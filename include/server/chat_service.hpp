#ifndef CHAT_SERVICE_H
#define CHAT_SERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <muduo/base/Logging.h>
#include <string>
#include <mutex>

#include "json.hpp"
#include "public.hpp"
#include "user_model.hpp"
#include "offline_msg_model.hpp"

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
    // 一对一聊天
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 添加好友
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 查看好友列表
    void viewFriendList(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 获取消息对应的处理器
    MsgHandler getHandler(int msg_id);
    // 服务端异常退出
    void resetState();
    // 客户端异常退出
    void clientCloseException(const TcpConnectionPtr& conn);
private:
    ChatService();

    // 存储消息id和handler对应关系
    unordered_map<int, MsgHandler> _handlerMap;
    
    // 数据操作类
    UserModel _userModel;

    // 定义互斥锁, 保证_userConnMap的安全
    mutex _connMutex;

    // 存储在线用户的通讯连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    // 离线消息操作类
    OfflineMsgModel _offlineMsgModel; 
};

#endif
