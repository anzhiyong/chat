# Qt Chat (C++/Qt Widgets)

一个基于 Qt 的即时聊天项目，包含 `客户端 + 服务端`，支持账号登录、好友体系、私聊、群聊与历史持久化。

## 功能状态
- 账号注册/登录（服务端校验）
- 好友申请、同意/拒绝
- 好友列表实时刷新
- 1 对 1 私聊
- 创建群聊、加入群聊、群消息收发
- 会话列表展示（私聊/群聊）
- 私聊历史消息拉取
- 持久化数据（服务端重启后保留）
  - 账号
  - 好友关系
  - 群与群成员
  - 私聊消息历史

## 项目结构
- `client/`：Qt Widgets 客户端
- `server/`：Qt TCP 服务端
- `CMakeLists.txt`：根 CMake 入口（包含 `client/` 子项目）

## 环境要求
- Qt 6（建议 `6.5.x`）
- CMake `>= 3.16`
- MinGW（Windows）或 GCC/Clang（Linux）

## 快速开始（Windows + Qt MinGW）
### 1. 构建客户端
```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:\Qt\6.5.3\mingw_64 -DCMAKE_CXX_COMPILER=C:\Qt\Tools\mingw1120_64\bin\g++.exe -DCMAKE_MAKE_PROGRAM=C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe
cmake --build build -j 8
```

### 2. 构建服务端
```powershell
cmake -S server -B server/build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:\Qt\6.5.3\mingw_64 -DCMAKE_CXX_COMPILER=C:\Qt\Tools\mingw1120_64\bin\g++.exe -DCMAKE_MAKE_PROGRAM=C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe
cmake --build server/build -j 8
```

### 3. 启动服务端
```powershell
.\server\build\QtChatServer.exe --host 0.0.0.0 --port 9000
```

### 4. 启动客户端
```powershell
.\build\QtChatClient.exe
```


## Linux 构建服务端（Ubuntu/Debian）
> 注意：Linux 不要使用 `MinGW Makefiles` 或 `C:\Qt\...` 这类 Windows 参数。

### 1. 安装依赖
```bash
sudo apt update
sudo apt install -y cmake build-essential qt6-base-dev qt6-base-dev-tools
```

### 2. 配置并构建服务端
```bash
cmake -S server -B server/build
cmake --build server/build -j$(nproc)
```

### 3. 启动服务端
```bash
./server/build/QtChatServer --host 0.0.0.0 --port 9000
```

### 4. 如果提示找不到 Qt6Config.cmake
```bash
cmake -S server -B server/build -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6
```
## 持久化文件位置
服务端运行后会在可执行目录下创建 `data/` 目录（例如 `server/build/data`）：
- `accounts.json`
- `friends.json`
- `groups.json`
- `private_messages.ndjson`

## 协议文档
- `server/PROTOCOL.md`

图片示例：`r`n- <img width="2560" height="1440" alt="image" src="https://github.com/user-attachments/assets/d8819a2a-b67b-4681-bb6a-cb975a63f985" />

