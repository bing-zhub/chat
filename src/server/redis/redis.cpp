#include "redis.hpp"
#include <iostream>
using namespace std;

// 连接redis服务器
bool Redis::connect(){
    _publishContext = redisConnect("127.0.0.1", 6379);
    if(!_publishContext) return false;
    _subscribeContext = redisConnect("127.0.0.1", 6379);
    if(!_subscribeContext) return false;
    
    // 在单独的线程中监听通道上的事件
    thread t([&]() {
        observerChannelMsg();
    });
    t.detach();
    return true;
}

// 向redis指定channel发送消息
bool Redis::publish(int channel, string msg){
    redisReply *reply = (redisReply *)redisCommand(_publishContext, "PUBLISH %d %s", channel, msg.c_str());
    if (nullptr == reply)
    {
        cerr << "publish command failed!" << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// redisCommand = redisAppendCommand + redisBufferWrite + redisGetReply

// 向redis指定channel订阅消息
bool Redis::subscribe(int channel){
    // 只订阅通道， 不接收通道消息， 接收消息通过observerChannelMsg在独立线程进行
    if(REDIS_ERR == redisAppendCommand(this->_subscribeContext, "SUBSCRIBE %d", channel)) {
        cerr << "subscribe command failed!" << endl;
        return false;
    }

    int done = 0;
    while(!done) {
        if(REDIS_ERR == redisBufferWrite(this->_subscribeContext, &done)) {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }
    // 为了不被阻塞 拆分开写 去除redisGetReply
    return true;
}

// 从redis中取消订阅指定的channel
bool Redis::unsubscribe(int channel){
    if (REDIS_ERR == redisAppendCommand(this->_subscribeContext, "UNSUBSCRIBE %d", channel)){
        cerr << "unsubscribe command failed!" << endl;
        return false;
    }
    
    // redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕（done被置为1）
    int done = 0;
    while (!done) {
        if (REDIS_ERR == redisBufferWrite(this->_subscribeContext, &done)) {
            cerr << "unsubscribe command failed!" << endl;
            return false;
        }
    }
    return true;
}

// 在独立线程中接收订阅通道的信息
void Redis::observerChannelMsg(){
    redisReply *reply = nullptr;
    while (REDIS_OK == redisGetReply(this->_subscribeContext, (void **)&reply)) {
        // 订阅收到的消息是一个带三元素的数组
        if (reply && reply->element[2] && reply->element[2]->str) {
            // 给业务层上报通道上发生的消息
            _notifyMsgHandler(atoi(reply->element[1]->str) , reply->element[2]->str);
        }

        freeReplyObject(reply);
    }
}

// 初始化向业务层上报消息的回调对象
void Redis::initNotifyHandler(function<void(int, string)> fn){
    this->_notifyMsgHandler = fn;
}
