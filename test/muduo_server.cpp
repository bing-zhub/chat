/*
  muduo网络库提供了两个主要的类
  TcpServer: 用于编写服务器程序
  TcpClient: 用于编写客户端程序

  epoll + 线程池
  好处: 能够把网络I/O代码和业务代码区分开
        1. 用户的链接与断开 2. 用户的可读写事件
*/

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <placeholder>
using namespace std;
using namespace muduo;
using namespace muduo::net;


/*
  基于muduo开发服务器程序
  1. 组合TcpServer对象
  2. 创建EventLoop时间循环对象的指针
  3. 明确TcpServer构造函数需要什么参数, 输出ChatServer构造函数
*/

class ChatServer {
  public:
   // 事件循环; IP+端口; 服务器名称
   ChatServer(EventLoop* loop, const InetAddress& listenAddr,
              const string& nameArg)
       : _server(loop, listenAddr, nameArg), _loop(loop) {
      // 给服务器注册用户连接的创建和断开回调
      std::bind(&ChatServer::onConnection, this, std::placeholders::_1);
      
      // 读写回调
    }

  private:
  // 处理用户的链接创建和断开
  void onConnection(const TcpConnectionPtr& ptr) {

  }
   TcpServer _server;
   EventLoop *_loop;
}