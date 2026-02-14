# Chat Protocol (v1)

本文件定义客户端和服务端的最小可用协议，采用 `JSON + 换行`（每一行一条 JSON 消息）。

## 1. 基本约定

1. 传输层：TCP
2. 编码：UTF-8
3. 分帧：每条消息以 `\n` 结尾
4. 字段命名：小写下划线（snake_case）
5. 每条消息都包含：
   - `type`：消息类型（字符串）
   - `request_id`：请求 ID（字符串，可选，客户端请求时建议带上）

## 2. 通用返回格式

服务端对请求类消息返回统一应答：

```json
{"type":"ack","request_id":"req-1001","ok":true}
```

失败示例：

```json
{"type":"ack","request_id":"req-1001","ok":false,"error":"user_not_found"}
```

## 3. 登录（login）

### 3.1 客户端 -> 服务端

```json
{"type":"login","request_id":"req-1","user_id":"u10001","nickname":"an"}
```

字段：
- `user_id`：用户唯一 ID
- `nickname`：昵称（可选）

### 3.2 服务端 -> 客户端

```json
{"type":"ack","request_id":"req-1","ok":true}
```

## 4. 私聊（direct_message）

### 4.1 客户端 -> 服务端

```json
{"type":"direct_message","request_id":"req-2","from":"u10001","to":"u10002","text":"你好","client_msg_id":"cmsg-1","ts":1739505600}
```

字段：
- `from`：发送方用户 ID
- `to`：接收方用户 ID
- `text`：消息文本
- `client_msg_id`：客户端消息 ID（用于去重）
- `ts`：客户端时间戳（秒，可选）

### 4.2 服务端 -> 接收方（推送）

```json
{"type":"direct_message","from":"u10001","to":"u10002","text":"你好","server_msg_id":"smsg-9001","client_msg_id":"cmsg-1","ts":1739505601}
```

### 4.3 服务端 -> 发送方（回执）

```json
{"type":"ack","request_id":"req-2","ok":true,"server_msg_id":"smsg-9001"}
```

## 5. 群聊（group_message）

### 5.1 客户端 -> 服务端

```json
{"type":"group_message","request_id":"req-3","from":"u10001","group_id":"g20001","text":"大家好","client_msg_id":"cmsg-2","ts":1739505700}
```

### 5.2 服务端 -> 群成员（推送）

```json
{"type":"group_message","from":"u10001","group_id":"g20001","text":"大家好","server_msg_id":"smsg-9002","client_msg_id":"cmsg-2","ts":1739505701}
```

### 5.3 服务端 -> 发送方（回执）

```json
{"type":"ack","request_id":"req-3","ok":true,"server_msg_id":"smsg-9002"}
```

## 6. 获取会话列表（session_list）

### 6.1 客户端 -> 服务端

```json
{"type":"session_list","request_id":"req-4","user_id":"u10001"}
```

### 6.2 服务端 -> 客户端

```json
{
  "type":"session_list_result",
  "request_id":"req-4",
  "ok":true,
  "sessions":[
    {"session_type":"direct","session_id":"u10002","title":"张三","last_text":"在吗","last_ts":1739505000,"unread":2},
    {"session_type":"group","session_id":"g20001","title":"产品群","last_text":"明天开会","last_ts":1739505300,"unread":0}
  ]
}
```

字段：
- `session_type`：`direct` 或 `group`
- `session_id`：会话唯一标识（私聊用对方 user_id，群聊用 group_id）
- `title`：会话标题
- `last_text`：最后一条消息预览
- `last_ts`：最后消息时间戳
- `unread`：未读数

## 7. 添加好友（friend_add）

### 7.1 客户端 -> 服务端

```json
{"type":"friend_add","request_id":"req-5","user_id":"u10001","friend_id":"u10002"}
```

### 7.2 服务端 -> 客户端

```json
{"type":"ack","request_id":"req-5","ok":true}
```

## 8. 创建群（group_create）

### 8.1 客户端 -> 服务端

```json
{"type":"group_create","request_id":"req-6","owner":"u10001","name":"周末开黑","members":["u10002","u10003"]}
```

### 8.2 服务端 -> 客户端

```json
{"type":"group_create_result","request_id":"req-6","ok":true,"group_id":"g20008"}
```

## 9. 错误码建议

- `invalid_json`：JSON 格式错误
- `invalid_type`：未知消息类型
- `missing_field`：缺少必要字段
- `user_not_found`：用户不存在
- `group_not_found`：群不存在
- `not_group_member`：不在群内
- `duplicate_client_msg_id`：重复消息（去重命中）

## 10. 版本策略

建议客户端首包附带：

```json
{"type":"login","request_id":"req-1","user_id":"u10001","protocol_version":"1"}
```

服务端若不支持，可返回：

```json
{"type":"ack","request_id":"req-1","ok":false,"error":"unsupported_protocol_version"}
```

