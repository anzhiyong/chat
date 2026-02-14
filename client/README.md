# Qt Chat Client

一个基础的 Qt Widgets 聊天客户端，只负责客户端逻辑，使用 `QTcpSocket` 连接服务器。

## 功能
- 输入服务器主机、端口、昵称
- 连接/断开服务器
- 文本消息发送
- 实时接收并显示聊天消息
- 协议使用一行一条 JSON（newline-delimited JSON）

## 协议示例
客户端发送:
- `{"type":"join","name":"alice"}`
- `{"type":"chat","name":"alice","text":"hello","time":"2026-02-14T11:00:00"}`

客户端接收:
- `{"type":"chat","name":"bob","text":"hi"}`
- `{"type":"system","text":"欢迎进入聊天室"}`
- 或普通文本行（非 JSON）

## 构建（Qt 6 + CMake）
```bash
cmake -S . -B build
cmake --build build
```

可执行文件为 `QtChatClient`。
