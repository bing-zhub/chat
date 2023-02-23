#include "chat_service.hpp"


// 注册消息与对应的handler
ChatService::ChatService() {
    using namespace std::placeholders;
    _handlerMap.insert({LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3)});
    _handlerMap.insert({REG_MSG, bind(&ChatService::reg, this, _1, _2, _3)});
}

ChatService* ChatService::getInstance() {
    static ChatService service;
    return &service;
}

// 通过msgid获取msg_handler
MsgHandler ChatService::getHandler(int msg_id) {
    auto it = _handlerMap.find(msg_id);
    if(it == _handlerMap.end()) {
       // 返回一个默认处理器, 空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time) {
            LOG_ERROR << "msg_id " << msg_id << " cannot find handler!"; 
        };
    } 

    return _handlerMap[msg_id];
}

// 登录
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    
    json resp;
    resp["msg_id"] = LOGIN_MSG_ACK;
    try{
        int id = js["id"].get<int>();
        string password = js["password"];

        if(id < 0 || password.empty()) {
            resp["errno"] = -3;
            resp["errmsg"] = "ID AND PASSWORD ARE REQUIRED"; 
            conn->send(resp.dump());
            return ;
        }

        User user = _userModel.query(id);

        if(user.getId() == -1 || user.getPassword() != password) {
            resp["errno"] = -2;
            resp["errmsg"] = "WRONG ID OR PASSWORD"; 
            conn->send(resp.dump());
            return ;
        }

        if(user.getState() != "offline") {
            resp["errno"] = -4;
            resp["errmsg"] = "USER ALREADY ONLINE"; 
            conn->send(resp.dump());
            return ;
        }

        user.setState("online");
        _userModel.update(user);


        resp["errmsg"] = "OK"; 
        resp["errno"] = 0;
        resp["id"] = user.getId();
        resp["name"] = user.getName();
        resp["state"] = user.getState();

        conn->send(resp.dump());
    } catch (json::exception& e) {
        resp["errno"] = -500;
        resp["errmsg"] = e.what();
        conn->send(resp.dump());
    }
}

// 注册
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    json resp;
    resp["msg_id"] = REG_MSG_ACK;

    try {
        string name = js["name"];
        string password = js["password"];
        User user;
        user.setName(name);
        user.setPassword(password);
        bool res = _userModel.insert(user);

        if(res) {
            // 注册成功
            resp["errno"] = 0;
            resp["errmsg"] = "OK";
            resp["id"] = user.getId();
        } else {
            // 注册失败
            resp["errno"] = -1;
            resp["errmsg"] = "USER EXIST";
        }

        conn->send(resp.dump());

    } catch (json::exception& e) {
        resp["errno"] = -500;
        resp["errmsg"] = e.what();
        conn->send(resp.dump());
    }
    
}