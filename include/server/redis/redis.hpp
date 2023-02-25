#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
#include <string>
using namespace std;

class Redis {
public:
    Redis() : _publishContext(nullptr), _subscribeContext(nullptr){}
    ~Redis() {
        if(_publishContext) redisFree(_publishContext);
        if(_subscribeContext) redisFree(_subscribeContext);
    }

    // 连接redis服务器
    bool connect();

    // 向redis指定channel发送消息
    bool publish(int channel, string msg);

    // 向redis指定channel订阅消息
    bool subscribe(int channel);

    // 从redis中取消订阅指定的channel
    bool unsubscribe(int channel);

    // 在独立线程中接收订阅通道的信息
    void observerChannelMsg();

    // 初始化向业务层上报消息的回调对象
void initNotifyHandler(function<void(int, string)> fn);

private:
    // 相当于一个client， 一个client阻塞订阅， 一个client发布消息
    // hiredis 同步上下文对象 负责publish消息
    redisContext *_publishContext;
    
    // hiredis 同步上下文对象 负责subscribe消息
    redisContext *_subscribeContext;

    // 回调操作 就饿收到订阅消息 给service层上报
    function<void(int, string)> _notifyMsgHandler;
};

#endif