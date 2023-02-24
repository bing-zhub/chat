#ifndef GROUP_USER_H
#define GROUP_USER_H

#include "user.hpp"

class GroupUser : public User {
public:
    void setRole(string role) { this->role = role; }
    string getRole() { return this->role; }
private:
    string role;
};


#endif