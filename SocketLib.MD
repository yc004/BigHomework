# SocketLib开发文档

___注意！为了可以较直观的查看本文档，请使用支持MarkDown的阅读器查看，如果你正在用VSCode看到本内容，请按Ctrl+Shift+V即可查看预览。
___

## 一、您知道socket吗？

- 知道，那么您可以自由修改。
- 不知道，但是我想调整接口，那么您可以了解一下定义的`SocketPair`结构体和`SocketData`结构体，这是socket的基本数据的包装，这有助于你使用它。
- 不知道，我也不打算调整接口，那么请不要动任何东西，除非调整`RECV_BUFF_SIZE`预设缓冲区大小和`PORT`
  端口（端口请同步修改`initEnv.js`请求端口）

## 二、结构体和枚举定义

- SocketPair

> 它是Header键值对以及参数键值对封装

```c
typedef struct SocketPair {
    char* key;
    char* value;
} SocketPair;
```

- SocketMethod

> 它是请求方法的枚举

```c
typedef enum SocketMethod {
    SM_GET,
    SM_POST,
    SM_PUT,
    SM_DELETE,
    SM_HEAD,
    SM_OPTIONS,
    SM_TRACE,
    SM_CONNECT,
    SM_PATCH,
    SM_UNKNOWN,
} SocketMethod;
```

- SocketData

> 它是请求数据的封装结构

```c
typedef struct SocketData {
    char* buff;
    int buffSize;
    SocketMethod method;
    char* url;
    char* version;
    SocketPair* headers;
    int headerSize;
    char* body;
    int bodySize;
    SocketPair* params;
    int paramSize;
} SocketData;
```

## 三、多平台socket API封装

- socket_init

> 初始化socket。

```c
static inline int socket_init();
```

- socket_cleanup

> 清理socket。

```c
static inline void socket_cleanup();
```

- socket_create

> 创建socket，返回socket句柄。

```c
static inline int socket_create();
```

- socket_bind

> 绑定socket，返回`0`表示成功，`-1`表示失败。`host`为`NULL`时，使用`localhost`。

```c
static inline int socket_bind(int sockfd, const char* host, int port);
```

- socket_listen

> 监听socket，返回`0`表示成功，`-1`表示失败。

```c
static inline int socket_listen(int sockfd);
```

- socket_accept

> 接受socket，返回新的socket句柄（终端句柄）。

```c
static inline int socket_accept(int sockfd);
```

- socket_connect

> 连接socket，返回`0`表示成功，`-1`表示失败。`host`为`NULL`时，使用`localhost`。

```c
static inline int socket_connect(int sockfd, const char* host, int port);
```

- socket_send

> 发送数据，返回发送的字节数。

```c
static inline int socket_send(int sockfd, const char* data, int len);
```

- socket_recv

> 接收数据，返回接收的字节数。

```c
static inline int socket_recv(int sockfd, char* buffer, int len);
```

- socket_close

> 关闭socket。

```c
static inline void socket_close(int sockfd);
```

- set_socktimeout

> 设置socket收发超时时间，返回`0`表示成功，`-1`表示失败。

```c
static int set_socket_timeout(int fd, int read_sec, int write_sec)
```

- recv_msg

> 对`socket_recv`封装。

```c
static inline int recv_msg(int sockfd, char* buffer, int len);
```

- send_msg

> 对`socket_send`封装，`len`为`0`时`strlen`重新获取长度。

```c
static inline int send_msg(int sockfd, const char* data, int len);
```

## 四、上层封装

- freeSocketData

> 释放`SocketData`结构体。

```c
static inline void freeSocketData(SocketData* data);
```

- parseSocketData

> 解析请求数据，返回`SocketData`结构体。

```c
static inline SocketData* parseSocketData(char* buff, int buffSize);
```

- paramsGET

> 获取参数，返回`SocketPair`结构体。

```c
static inline SocketPair* paramsGET(SocketData* data, char* key);
```

- do400BadRequest

> 执行400错误。

```c
static inline void do400BadRequest(SocketData* data, SOCKET connfd);
```

## 五、socket常见问题

- 端口占用

        其他端口占用端口时就会有问题，此时可以修改端口也可以关闭占用端口的程序。

- Windows端没有链接ws2_32

        加上编译参数-lws2_32即可

- 其他未知问题