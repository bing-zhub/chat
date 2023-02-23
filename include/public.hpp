#ifndef PUBLIC_H
#define PUBLIC_H

/*
    server client的公共文件
*/

enum EnMsgType {
    LOGIN_MSG = 1, // 登录
    REG_MSG, // 注册
    LOGIN_MSG_ACK, // 登录响应信息
    REG_MSG_ACK, // 注册响应消息
    ONE_CHAT_MSG, // 聊天信息
    ONE_CHAT_MSG_ACK // 聊天信息回应
};

#endif