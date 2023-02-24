#ifndef USER_MODEL_H
#define USER_MODEL_H

#include <vector>

#include "user.hpp"

// User数据操作类
class UserModel {
public:
    // 添加用户
    bool insert(User& user);
    // 查询用户
    User query(int id);
    // 更新用户
    bool update(User& user);
    // 重置用户状态
    bool resetState();
    // 添加好友
    bool addFriend(int userId, int friendId);
    // 查看好友列表
    vector<User> viewFriendList(int userId);
};

#endif