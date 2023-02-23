#include <iostream>

#include "user_model.hpp"
#include "db.hpp"

using namespace std;

bool UserModel::insert(User& user) {
    // 1.组装sql语句
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