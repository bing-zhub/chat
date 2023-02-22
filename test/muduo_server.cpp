/*
  muduo网络库提供了两个主要的类
  TcpServer: 用于编写服务器程序
  TcpClient: 用于编写客户端程序

  epoll + 线程池
  好处: 能够把网络I/O代码和业务代码区分开
        1. 用户的链接与断开 2. 用户的可读写事件
*/

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <functional>
#include <iostream>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;

/*
  基于muduo开发服务器程序
  1. 组合TcpServer对象
  2. 创建EventLoop时间循环对象的指针
  3. 明确TcpServer构造函数需要什么参数, 输出ChatServer构造函数
  4. 在当前服务器类的构造函数中, 注册处理连接的回调函数和处理读写事件的回调函数
  5. 设置合适的服务端数量, 内部自动分配I/O线程和worker线程
*/

class ChatServer {
 public:
  // 事件循环; IP+端口; 服务器名称
  ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop) {
    // 给服务器注册用户连接的创建和断开回调
    _server.setConnectionCallback(
        bind(&ChatServer::onConnection, this, placeholders::_1));

    // 读写回调
    _server.setMessageCallback(
        bind(&ChatServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));

    // 设置服务器端的线程数量
    _server.setThreadNum(4);  // 1个IO 3个Worker
  }

  // 开启事件循环
  void start() { _server.start(); }

 private:
  // 处理用户的链接创建和断开
  void onConnection(const TcpConnectionPtr& conn) {
    if(conn->connected()) {
      cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " state: online" << endl;
    } else {
      cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " state: offline" << endl;
      conn->shutdown(); // close(fd);
    }
  }

  // 专门处理用户的读写事件
  void onMessage(const TcpConnectionPtr& conn,  // 连接
                 Buffer* buf,                   // 缓冲区
                 Timestamp time) {              // 时间信息
    string str = buf->retrieveAllAsString();
    cout << "RX [" << time.toFormattedString() << "]:" <<  str << endl;
    conn->send(str);
  }
  TcpServer _server;
  EventLoop* _loop;
};

int main() {
  EventLoop loop;
  InetAddress addr("127.0.0.1", 999);
  ChatServer server(&loop, addr, "chat");

  server.start(); // listen fd添加到epoll上
  loop.loop(); // epoll_wait 以阻塞方式等待新用户连接/读写事件等

  return 0;
}