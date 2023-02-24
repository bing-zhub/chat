#ifndef GROUP_MODEL_H
#define GROUP_MODEL_H

#include "group.hpp"
#include <string>
#include <vector>
using namespace std;

class GroupModel {
public:
    // 创建群组
    bool createGroup(Group &group);
    // 加入群组
    bool joinGroup(int userId, int groupId, string role);
    // 查询用户所在群组
    vector<Group> queryGroups(int userId);
    // 根据GroupID查询群组用户列表, 除userId自己, 给其它用户发送信息
    vector<int> queryGroupUsers(int userId, int groupId);
};

#endif