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
    ONE_CHAT_MSG_ACK, // 聊天信息回应
    ADD_FRIEND_MSG, // 添加好友信息
    ADD_FRIEND_MSG_ACK, // 添加好友信息回复
    VIEW_FRIEND_LIST, // 查看好友列表
    VIEW_FRIEND_LIST_ACK,  // 查看好友列表响应
    CREATE_GROUP_MSG, // 创建群组
    CREATE_GROUP_MSG_ACK,
    JOIN_GROUP_MSG, // 加入群组
    JOIN_GROUP_MSG_ACK,
    GROUP_CHAT_MSG, // 群聊天 
    GROUP_CHAT_MSG_ACK
};

#endif