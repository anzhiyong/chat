# 客户端学习笔记（小白版）

## 1. 这个项目的文件分工
- `MainWindow.ui`：界面结构（拖拽控件保存的文件）
- `MainWindow.h`：类声明（有哪些函数、变量）
- `MainWindow.cpp`：具体逻辑（点击按钮后做什么）
- `main.cpp`：程序入口

## 2. 一句话理解工作流程
- 你在界面输入地址 -> 点击连接 -> `QTcpSocket` 连服务器 -> 收发 JSON 文本消息 -> 显示到聊天框。

## 3. 为什么要 `m_receiveBuffer`
- TCP 不是“按消息”传输，而是“按字节流”传输。
- 可能一次收到半条消息，也可能一次收到多条消息。
- 所以必须先缓存，再按 `\n` 切成完整消息。

## 4. 客户端发什么
- 连接成功后自动发：`{"type":"join","name":"你的昵称"}`
- 每次发送聊天发：`{"type":"chat","name":"你的昵称","text":"消息"}`

## 5. 客户端收什么
- `type=chat`：显示为聊天消息
- `type=system`：显示为系统消息

## 6. 你该怎么继续学
- 先只看 `main.cpp` + `MainWindow.h`
- 再看 `MainWindow.cpp` 里这 5 个函数：
  - `connectOrDisconnect`
  - `sendMessage`
  - `onConnected`
  - `onReadyRead`
  - `processLine`
