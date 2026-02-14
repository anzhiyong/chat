# Qt Chat Server

一个最小可用的 Qt TCP 聊天服务器，和当前客户端协议一致（每行一个 JSON）。

## 功能
- 多客户端并发连接
- 处理 `join` / `chat` 两种消息
- 聊天消息广播给所有在线客户端
- 系统消息广播（加入/离开/错误提示）

## 协议（NDJSON）
客户端发：
- `{"type":"join","name":"alice"}`
- `{"type":"chat","name":"alice","text":"hello"}`

服务器发：
- `{"type":"chat","name":"alice","text":"hello","time":"..."}`
- `{"type":"system","text":"alice 加入聊天室"}`

## 构建
```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:\Qt\6.5.3\mingw_64 -DCMAKE_CXX_COMPILER=C:\Qt\Tools\mingw1120_64\bin\g++.exe -DCMAKE_MAKE_PROGRAM=C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe
cmake --build build -j 8
```

## 运行
```powershell
.\build\QtChatServer.exe --host 0.0.0.0 --port 9000
```
