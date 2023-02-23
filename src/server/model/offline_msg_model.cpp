#include "offline_msg_model.hpp"
#include "db.hpp"

// 存储用户的离线消息
bool OfflineMsgModel::insert(int userId, string msg) {
    char sql[1024] = { 0 };
    sprintf(sql, "insert into offline_msg(user_id, msg) values(%d, '%s');",
        userId, msg.c_str());

    MySQL mysql;
    if(!mysql.connect()) return false;
    
    return mysql.update(sql);
}

// 移除用户的离线消息
bool OfflineMsgModel::remove(int userId) {
    char sql[1024] = { 0 };
    sprintf(sql, "DELETE FROM offline_msg WHERE user_id=%d;", userId);

    MySQL mysql;
    if(!mysql.connect()) return false;
    
    return mysql.update(sql);
}

// 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userId) {
     // 组装sql语句
    char sql[1024] = { 0 };
    sprintf(sql, "SELECT msg FROM offline_msg WHERE user_id = %d;", userId);

    MySQL mysql;
    if(!mysql.connect()) return {};

    MYSQL_RES *res = mysql.query(sql);
    
    if(!res) return {};

    vector<string> vec;

    MYSQL_ROW row;
    
    while((row = mysql_fetch_row(res)) != NULL) {
        vec.push_back(row[0]);
    }

    mysql_free_result(res);
    return vec;
}