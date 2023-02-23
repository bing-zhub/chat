#ifndef USER_MODEL_H
#define USER_MODEL_H

#include "user.hpp"

// User数据操作类
class UserModel {
public:
    // 添加用户
    bool insert(User& user);

};

#endif