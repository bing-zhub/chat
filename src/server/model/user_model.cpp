#include <iostream>

#include "user_model.hpp"
#include "db.hpp"

using namespace std;

bool UserModel::insert(User& user) {
    // 组装sql语句
    char sql[1024] = { 0 };
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s');",
        user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());

    MySQL mysql;
    if(!mysql.connect()) return false;

    if(mysql.update(sql)) {
        user.setId(mysql_insert_id(mysql.getConnection()));
        return true;
    }
    
    return false;
}

User UserModel::query(int id) {
    // 组装sql语句
    char sql[1024] = { 0 };
    sprintf(sql, "select * from user where id = %d;", id);

    MySQL mysql;
    if(!mysql.connect()) return false;

    MYSQL_RES *res = mysql.query(sql);
    
    if(!res) return User();

    MYSQL_ROW row = mysql_fetch_row(res);
    
    if(!row) return User();

    User user;
    user.setId(atoi(row[0]));
    user.setName(row[1]);
    user.setPassword(row[2]);
    user.setState(row[3]);

    mysql_free_result(res);
    return user;
}

bool UserModel::update(User& user) {
    // 组装sql语句
    char sql[1024] = { 0 };
    sprintf(sql, "update user set name='%s',password='%s',state='%s' where id = %d;", user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str(), user.getId());

    MySQL mysql;
    if(!mysql.connect()) return false;

    return mysql.update(sql);
}

bool UserModel::resetState() {
    // 组装sql语句
    char sql[1024] = "update user set state='offline' where state='online'";

    MySQL mysql;
    if(!mysql.connect()) return false;

    return mysql.update(sql);
}

bool UserModel::addFriend(int userId, int friendId) {
    // 组装sql语句
    char sql[1024] = { 0 };
    sprintf(sql, "insert into friend(userid, friendid) select distinct %d,%d from user where id in (select id from user where id=%d or id = %d);", userId, friendId, userId, friendId);

    MySQL mysql;
    if(!mysql.connect()) return false;

    return mysql.update(sql);
}

vector<User> UserModel::viewFriendList(int userId) {
     // 组装sql语句
    char sql[1024] = { 0 };
    sprintf(sql, "select id, name, state from `user` where id in (select friendid from friend where userid = %d);", userId);

    MySQL mysql;
    if(!mysql.connect()) return {};

    MYSQL_RES *res = mysql.query(sql);
    
    if(!res) return {};

    vector<User> vec;

    MYSQL_ROW row;
    
    while((row = mysql_fetch_row(res)) != NULL) {
        vec.push_back(User(atoi(row[0]), row[1], "", row[2]));
    }

    mysql_free_result(res);
    return vec;
}