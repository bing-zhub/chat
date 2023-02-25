基于TCP的IM聊天软件
## 技术栈
Muduo, JSON for Modern C++, MySQL, Redis, Nginx
## 测试案例
用户A
- 登录
`{"msg_id":1,"id":21,"name":"bin","password":"123"}`
- 发送信息
`{"msg_id":5,"to_id": 22,"msg":"HelloWorld!"}`
- 添加好友
`{"msg_id":7,"user_id":21,"friend_id":22}`
- 查看好友列表
`{"msg_id":9,"user_id":21}`
- 创建群聊
`{"msg_id":11,"id":21,"group_name":"ChicChat","desc":"xyz"}`
- 发送群聊信息
`{"msg_id":15,"id":21,"group_id":2,"msg":"HelloWorld"}`


用户B
- 登录
`{"msg_id":1,"id":22,"name":"bing","password": "123"}`
- 发送消息
`{"msg_id":5,"to_id":21,"msg":"Hello Fucking World!"}`
- 加入群组
`{"msg_id":13,"id":22,"group_id":2}`


用户C
- 登录
`{"msg_id":1,"id":19,"name":"bi","password": "123"}`
- 加入群组
`{"msg_id":13,"id":19,"group_id":2}`

## 负载均衡器
1. 把Client的请求按照负载算法分发到具体的业务服务器ChatServer上
2. 能够和ChatServer保持心跳机制，检测ChatServer故障
3. 能够发现新添加的ChatServer设备，方便拓展服务器数量

选择nginx作为TCP负载均衡模块
1. 对nginx进行原码编译，包含tcp负载均衡模块
2. 通过nginx.conf配置负载均衡
3. nginx平滑加载配置文件启动(不重启服务器)