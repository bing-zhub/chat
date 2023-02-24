基于TCP的IM聊天软件
## 技术栈
muduo, JSON for Modern C++, MySQL
## 测试案例
``` json
// 登录
{"msg_id": 1,"id": 21,"name":"bin", "password": "123"}
// 发送信息
{"msg_id": 5,"to_id": 22,"msg": "HelloWorld!"}
// 添加好友
{"msg_id":7,"user_id":21,"friend_id":22}
// 查看好友列表
{"msg_id":9,"user_id":21}


{"msg_id":1,"id": 22,"name":"bing","password": "123"}
{"msg_id":5,"to_id":21,"msg":"Hello Fucking World!"}
```