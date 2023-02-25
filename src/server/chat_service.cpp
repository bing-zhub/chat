#include "chat_service.hpp"


// 注册消息与对应的handler
ChatService::ChatService() {
    using namespace std::placeholders;
    _handlerMap.insert({LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3)});
    _handlerMap.insert({LOGIN_OUT_MSG, bind(&ChatService::loginOut, this, _1, _2, _3)});
    _handlerMap.insert({REG_MSG, bind(&ChatService::reg, this, _1, _2, _3)});
    _handlerMap.insert({ONE_CHAT_MSG, bind(&ChatService::oneChat, this, _1, _2, _3)});
    _handlerMap.insert({ADD_FRIEND_MSG, bind(&ChatService::addFriend, this, _1, _2, _3)});
    _handlerMap.insert({VIEW_FRIEND_LIST, bind(&ChatService::viewFriendList, this, _1, _2, _3)});
    _handlerMap.insert({CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, _1, _2, _3)});
    _handlerMap.insert({JOIN_GROUP_MSG, bind(&ChatService::joinGroup, this, _1, _2, _3)});
    _handlerMap.insert({GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, _1, _2, _3)});
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

// 服务端异常退出
void ChatService::resetState() {
    _userModel.resetState();
}

// 客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr& conn) {
    int id = -1;
    {
        lock_guard<mutex> lock(_connMutex);
        for(const auto& [k, v] : _userConnMap) {
            if(v == conn) {
                id = k;
                break;
            }
        }
        _userConnMap.erase(id);
    }
    if(id == -1) return ;
    // 退出登录, 设置为离线
    User user = _userModel.query(id);
    user.setState("offline");
    _userModel.update(user);
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
        // 登录成功记录用户连接信息
        {
            lock_guard<mutex> lock(_connMutex); // 此作用域加锁
            _userConnMap.insert({user.getId(), conn});
        }

        resp["errmsg"] = "OK"; 
        resp["errno"] = 0; 
        resp["id"] = user.getId();
        resp["name"] = user.getName();
        resp["state"] = user.getState();

        // 查询用户是否有离线信息, 如果有的话发送回去
        resp["offline_msgs"] = _offlineMsgModel.query(user.getId());
        _offlineMsgModel.remove(user.getId());
        
        // 查询用户的好友列表， 并返回
        auto friends = _userModel.viewFriendList(user.getId());
        
        resp["friends"] = json::array();

        for(User& u : friends) {
            auto user = json::object({ { u.getName(), json::object({ {"id", u.getId()}, {"state",  u.getState()} })} });
            resp["friends"].push_back(user);
        }

        conn->send(resp.dump());
    } catch (json::exception& e) {
        resp["errno"] = -500;
        resp["errmsg"] = e.what();
        conn->send(resp.dump());
    }
}

// 登出
void ChatService::loginOut(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    User user = _userModel.query(js["id"].get<int>());
    user.setState("offline");
    _userModel.update(user);
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

// 一对一聊天
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    json resp;
    resp["msg_id"] = ONE_CHAT_MSG_ACK;
    TcpConnectionPtr toConn = NULL;
    try {
        int toId = js["to_id"].get<int>();
        {
            lock_guard<mutex> lock(_connMutex);
            if(_userConnMap.count(toId) > 0) {
                toConn = _userConnMap[toId];
            }
        }
        if(!toConn) {
            // 存储离线消息表
            resp["errno"] = 0;
            resp["messgae"] = "USER IS OFFLINE, BUT SAVED";
            conn->send(resp.dump());
            _offlineMsgModel.insert(toId, js.dump());
            return ;
        }
        toConn->send(js.dump());
        resp["errno"] = 0;
        resp["errmsg"] = "OK";
        conn->send(resp.dump());
    } catch (json::exception& e) {
        resp["errno"] = -500;
        resp["errmsg"] = e.what();
        conn->send(resp.dump());
    }
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    json resp;
    resp["msg_id"] = ADD_FRIEND_MSG_ACK;
    try {
        int friendId = js["friend_id"].get<int>();
        int userId = js["user_id"].get<int>();
        bool res = _userModel.addFriend(userId, friendId);
        if(res) {
            resp["errmsg"] = "OK";
        } else {
            resp["errmsg"] = "CANNOT ADD FRIEND";
        }
    } catch (json::exception& e) {
        resp["errno"] = -500;
        resp["errmsg"] = e.what();
    }
    conn->send(resp.dump());
}

void ChatService::viewFriendList(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    json resp;
    resp["msg_id"] = VIEW_FRIEND_LIST_ACK;
    try {
        int userId = js["user_id"].get<int>();
        resp["errmsg"] = "OK";
        // 查询用户的好友列表， 并返回
        auto friends = _userModel.viewFriendList(userId);
        
        resp["friends"] = json::array();

        for(User& u : friends) {
            auto user = json::object({ { u.getName(), json::object({ {"id", u.getId()}, {"state",  u.getState()} })} });
            resp["friends"].push_back(user);
        }
    } catch (json::exception& e) {
        resp["errno"] = -500;
        resp["errmsg"] = e.what();
    }
    conn->send(resp.dump());
}

// 创建群组
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    json resp;
    resp["msg_id"] = CREATE_GROUP_MSG_ACK;
    try {
        int userId = js["id"].get<int>();
        string groupName = js["group_name"];
        string desc = js["desc"];

        Group group(-1, groupName, desc);
        
        bool res = _groupModel.createGroup(group);

        if(res && _groupModel.joinGroup(userId, group.getId(), "creator")) {
            resp["group_id"] = group.getId();
            resp["errmsg"] = "OK";
        } else {
            resp["errmsg"] = "CANNOT CREATE GROUP";
        }
    } catch (json::exception& e) {
        resp["errno"] = -500;
        resp["errmsg"] = e.what();
    }
    conn->send(resp.dump());
}

// 加入群组
void ChatService::joinGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    json resp;
    resp["msg_id"] = JOIN_GROUP_MSG_ACK;
    try {
        int userId = js["id"].get<int>();
        int groupId = js["group_id"].get<int>();
        bool res = _groupModel.joinGroup(userId, groupId, "normal");
        if(res) {
            resp["errno"] = 0;
            resp["errmsg"] = "OK";
        } else {
            resp["error"] = -500;
            resp["errmsg"] = "CANNOT JOIN GROUP";
        }
    }  catch (json::exception& e) {
        resp["errno"] = -500;
        resp["errmsg"] = e.what();
    }
    conn->send(resp.dump());
}

// 群聊
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    json resp;
    resp["msg_id"] = JOIN_GROUP_MSG_ACK;
    try {
        int userId = js["id"].get<int>();
        int groupId = js["group_id"].get<int>();
        vector<int> userIdVec = _groupModel.queryGroupUsers(userId, groupId);
        for(int id : userIdVec) {
            TcpConnectionPtr client_conn = NULL;
            {
                lock_guard<mutex> lock(_connMutex);
                if(_userConnMap.count(id)) {
                    client_conn = _userConnMap[id];
                }
            }
            if(client_conn) {
                client_conn->send(js.dump());
            } else {
                _offlineMsgModel.insert(id, js.dump());
            }
        }
        resp["errno"] = 0;
        resp["errmsg"] = "OK";
    }  catch (json::exception& e) {
        resp["errno"] = -500;
        resp["errmsg"] = e.what();
    }
    conn->send(resp.dump());
}