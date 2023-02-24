#include "group_model.hpp"
#include "db.hpp"

// 创建群组
bool GroupModel::createGroup(Group &group) {
    char sql[1024] = { 0 };
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s','%s')", group.getName().c_str(), group.getDesc().c_str());

    MySQL mysql;
    if(!mysql.connect()) return false;

    if(!mysql.update(sql)) return false;

    group.setId(mysql_insert_id(mysql.getConnection()));

    return true;
}

// 加入群组
bool GroupModel::joinGroup(int userId, int groupId, string role) {
    char sql[1024] = { 0 };
    sprintf(sql, "insert into groupuser values('%d','%d','%s')", groupId, userId, role.c_str());

    MySQL mysql;
    if(!mysql.connect()) return false;

    return mysql.update(sql);
}

// 查询用户所在群组
vector<Group> GroupModel::queryGroups(int userId) {
    /*
        1. 根据userid在groupuser中查询群组信息
        2. 再根据群组信息，查询属于该群组所有用户的userid，和user联查，拿到所有信息
    */
    // 获取用户所有组 
    char sql[1024] = { 0 };
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a INNER JOIN groupuser b on a.id = b.groupid  WHERE b.userid = %d;", userId);
    
    MySQL mysql;
    if(!mysql.connect()) return {};

    MYSQL_RES *res = mysql.query(sql);
    if(!res) return {};

    vector<Group> groupVec;

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res)) != NULL) {
        Group group;
        group.setId(atoi(row[0]));
        group.setName(row[1]);
        group.setDesc(row[2]);
        groupVec.push_back(group);
    }

    // 获取用户信息
    for(Group &g : groupVec) {
        sprintf(sql, "SELECT u.id,u.name,u.state,g.grouprole from groupuser g INNER JOIN `user` u on g.userid = u.id where groupid = %d;", g.getId());
        res = mysql.query(sql);
        if(!res) break ;
        while((row = mysql_fetch_row(res)) != NULL) {
            GroupUser gUser;
            gUser.setId(atoi(row[0]));
            gUser.setName(row[1]);
            gUser.setState(row[2]);
            gUser.setRole(row[3]);
            g.getUsers().push_back(gUser);
        }
        mysql_free_result(res);
    }
    
    return groupVec;
}

// 根据GroupID查询群组用户列表, 除userId自己, 给其它用户发送信息
vector<int> GroupModel::queryGroupUsers(int userId, int groupId){
    char sql[1024] = { 0 };
    sprintf(sql, "select userid from groupuser where groupid=%d and userid != %d;", groupId, userId);

    vector<int> idVec;
    MySQL mysql;
    if(!mysql.connect()) return {};

    MYSQL_RES *res = mysql.query(sql);
    if(!res) return {};

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res)) != NULL) {
        idVec.push_back(atoi(row[0]));
    }
    mysql_free_result(res);
    return idVec;
}