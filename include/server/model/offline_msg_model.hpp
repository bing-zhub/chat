#ifndef OFFLINE_MSG_MODEL_H
#define OFFLINE_MSG_MODEL_H

#include <string>
#include <vector>
using namespace std;

// 提供离线消息表的操作接口方法
class OfflineMsgModel {
public:
    // 存储用户的离线消息
    bool insert(int userId, string msg);

    // 移除用户的离线消息
    bool remove(int userId);

    // 查询用户的离线消息
    vector<string> query(int userId);
};

#endif