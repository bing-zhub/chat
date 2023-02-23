#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User {

public:
    User(int id = -1, string name = "", string password = "", string state = "offline")
        : id(id), name(name), password(password), state(state) {

    }
    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setPassword(string password) { this->password = password; }
    void setState(string state) { this->state = state; }
    
    int getId() { return this->id; }
    string getName() { return this->name; }
    string getPassword() { return this->password; }
    string getState() { return this->state; }
private:
    int id;
    string name;
    string password;
    string state;
};

#endif