/**
 * @file socketLib.h
 * @warning If you are not a developer, please do not modify this file unless you know what you are doing.
 */
#ifndef SOCKETLIB_H
#define SOCKETLIB_H

//10MB(recv/send buffer)
#define RECV_BUFF_SIZE 1024*1024*10
//same as localhost
#define HOST "127.0.0.1"
//same as initEnv.js settings
#define PORT 18080

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#endif


#ifdef _WIN32

static inline int socket_init() {
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(2, 2), &wsa_data);
}

static inline void socket_cleanup() {
    WSACleanup();
}
#else
static inline int socket_init() {
    return 0;
}

static inline void socket_cleanup() {
    return;
}
#endif

static inline int socket_create() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket_create: failed to create socket\n");
        return -1;
    }
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) == -1) {
        perror("socket_create: failed to set SO_REUSEADDR option\n");
        return -1;
    }
    return sockfd;
}

static inline int socket_bind(int sockfd, const char* host, int port) {
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (host != NULL) {
        if (strcmp(host, "localhost") == 0) {
            addr.sin_addr.s_addr = inet_addr(HOST);
        }
        else {
            addr.sin_addr.s_addr = inet_addr(host);
        }
    }
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("socket_bind: failed to bind socket\n");
        printf("Error: try to check your port %d is not used by other application.\n"
               "cmd command: netstat -ano | findstr %d\n"
               "find that PID and kill it.\n", port, port);
        return -1;
    }
    return 0;
}

static inline int socket_listen(int sockfd) {
    if (listen(sockfd, 5) == -1) {
        perror("socket_listen: failed to listen on socket\n");
        return -1;
    }
    return 0;
}

static inline int socket_accept(int sockfd) {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int connfd = accept(sockfd, (struct sockaddr *)&addr, &addrlen);
    if (connfd == -1) {
        perror("socket_accept: failed to accept connection\n");
        return -1;
    }
    return connfd;
}

static inline int socket_connect(int sockfd, const char* host, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (host != NULL) {
        if (strcmp(host, "localhost") == 0) {
            addr.sin_addr.s_addr = inet_addr(HOST);
        }
        else {
            addr.sin_addr.s_addr = inet_addr(host);
        }
    }
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("socket_connect: failed to connect to remote host\n");
        return -1;
    }
    return 0;
}

static inline int socket_send(int sockfd, const char* data, int len) {
    int sent = 0;
    while (sent < len) {
        int n = send(sockfd, data + sent, len - sent, 0);
        if (n == -1) {
            perror("socket_send: failed to send data\n");
            return -1;
        }
        sent += n;
        if (n < RECV_BUFF_SIZE) {
            break;
        }
    }
    return sent;
}

static inline int socket_recv(int sockfd, char* buffer, int len) {
    int received = 0;
    while (received < len) {
        int n = recv(sockfd, buffer + received, len - received, 0);
        printf("Info: socket_recv: received %d bytes\n", n);
        if (n == -1) {
            printf("Info: socket_recv: failed to receive data, maybe connection timeout.\n");
            return -1;
        }
        received += n;
        if (n == 0 || n < RECV_BUFF_SIZE) {
            // Connection closed
            break;
        }
    }
    return received;
}

static inline void socket_close(int sockfd) {
#ifdef _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif
}


static int set_socket_timeout(int fd, int read_sec, int write_sec) {
    struct timeval send_timeval;
    struct timeval recv_timeval;
    if (fd <= 0) return -1;
    send_timeval.tv_sec = write_sec < 0 ? 0 : write_sec;
    send_timeval.tv_usec = 0;
    recv_timeval.tv_sec = read_sec < 0 ? 0 : read_sec;;
    recv_timeval.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&send_timeval, sizeof(struct timeval)) == -1) {
        return -1;
    }
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&recv_timeval, sizeof(struct timeval)) == -1) {
        return -1;
    }
    return 0;
}

static inline int recv_msg(int sockfd, char* buffer, int len) {
    int n = socket_recv(sockfd, buffer, len);
    if (n == -1) {
        printf("Info: failed to receive message from socket %d\n", sockfd);
        return -1;
    }
    if (n == 0) {
        printf("Info: connection closed by socket %d\n", sockfd);
        return 0;
    }
    buffer[n] = '\0';
    printf(
        "\n===============>>>>>[Received message from socket %d]>>>>>===============\n"
        "%s"
        "\n===============<<<<<[Received message from socket %d]<<<<<===============\n", sockfd, buffer, sockfd);
    return n;
}

static inline void send_msg(int sockfd, const char* msg, int len) {
    if (len == 0)len = strlen(msg);
    if (socket_send(sockfd, msg, len) == -1) {
        char err[100];
        sprintf(err, "Error: failed to send message to socket %d\n", sockfd);
        perror(err);
        return;
    }
    printf(
        "\n===============>>>>>[Sent message to socket %d]>>>>>================\n"
        "%s"
        "\n===============<<<<<[Sent message to socket %d]<<<<<===============\n", sockfd, msg, sockfd);
}


typedef struct SocketPair {
    char* key;
    char* value;
} SocketPair;

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

typedef struct SocketData {
    char* buff;
    int buffSize; //实际数据
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

static inline void freeSocketData(SocketData* data) {
    if (data == NULL)return;
    if (data->buff) free(data->buff); //很多数据都是指向buff的，所以只需要释放buff就可以了
    if (data->headers) free(data->headers);
    if (data->body) free(data->body);
    if (data->params) free(data->params);
    free(data);
}

static inline SocketData* parseSocketData(char* buff, int buffSize) {
    SocketData* data = (SocketData *)malloc(sizeof(SocketData));
    data->buff = buff;
    data->buffSize = buffSize;
    data->method = SM_UNKNOWN;
    data->url = NULL;
    data->version = NULL;
    data->headers = NULL;
    data->headerSize = 0;
    data->body = NULL;
    data->bodySize = 0;
    data->params = NULL;
    data->paramSize = 0;
    //解析请求头
    char* p = buff;
    char* end = buff + buffSize;
    char* lineEnd = NULL;
    char* lineStart = p;
    while (p < end) {
        if (*p == '\r' && *(p + 1) == '\n') {
            lineEnd = p;
            *p = '\0';
            p += 2;
            if (lineStart == lineEnd) {
                //请求头结束
                break;
            }
            if (data->method == SM_UNKNOWN) {
                //解析请求方法
                if (strncmp(lineStart, "GET", 3) == 0) {
                    data->method = SM_GET;
                }
                else if (strncmp(lineStart, "POST", 4) == 0) {
                    data->method = SM_POST;
                }
                else if (strncmp(lineStart, "PUT", 3) == 0) {
                    data->method = SM_PUT;
                }
                else if (strncmp(lineStart, "DELETE", 6) == 0) {
                    data->method = SM_DELETE;
                }
                else if (strncmp(lineStart, "HEAD", 4) == 0) {
                    data->method = SM_HEAD;
                }
                else if (strncmp(lineStart, "OPTIONS", 7) == 0) {
                    data->method = SM_OPTIONS;
                }
                else if (strncmp(lineStart, "TRACE", 5) == 0) {
                    data->method = SM_TRACE;
                }
                else if (strncmp(lineStart, "CONNECT", 7) == 0) {
                    data->method = SM_CONNECT;
                }
                else if (strncmp(lineStart, "PATCH", 5) == 0) {
                    data->method = SM_PATCH;
                }
                //解析请求URL
                char* urlStart = lineStart;
                while (*urlStart != ' ') {
                    urlStart++;
                }
                *urlStart = '\0';
                urlStart++;
                data->url = urlStart;
                //解析请求版本
                char* versionStart = urlStart;
                while (*versionStart != ' ') {
                    versionStart++;
                }
                *versionStart = '\0';
                versionStart++;
                data->version = versionStart;
            }
            else {
                //解析请求头
                char* keyStart = lineStart;
                char* keyEnd = keyStart;
                while (*keyEnd != ':') {
                    keyEnd++;
                }
                *keyEnd = '\0';
                keyEnd++;
                char* valueStart = keyEnd;
                while (*valueStart == ' ') {
                    valueStart++;
                }
                char* valueEnd = valueStart;
                while (*valueEnd != '\0') {
                    valueEnd++;
                }
                valueEnd--;
                while (*valueEnd == ' ') {
                    valueEnd--;
                }
                valueEnd++;
                *valueEnd = '\0';
                data->headerSize++;
                data->headers = (SocketPair *)realloc(data->headers, sizeof(SocketPair) * data->headerSize);
                data->headers[data->headerSize - 1].key = keyStart;
                data->headers[data->headerSize - 1].value = valueStart;
            }
            lineStart = p;
        }
        else {
            p++;
        }
    }
    //解析请求体
    switch (data->method) {
        case SM_POST: {
            //这种后头是数据
            for (int i = 0; i < data->headerSize; i++) {
                if (strcmp(data->headers[i].key, "Content-Length") == 0) {
                    data->bodySize = atoi(data->headers[i].value);
                    data->body = malloc(data->bodySize + 1);
                    memcpy(data->body, p, data->bodySize);
                    data->body[data->bodySize] = '\0';
                    break;
                }
            }
            break;
        }
        case SM_GET: {
            //这种在url后面
            char* paramStart = data->url;
            while (*paramStart != '?' && *paramStart != '\0') {
                paramStart++;
            }
            if (*paramStart == '?') {
                char* paramEnd = paramStart;
                while (*paramEnd != '\0') {
                    paramEnd++;
                }
                paramEnd--;
                while (*paramEnd == ' ') {
                    paramEnd--;
                }
                paramEnd++;
                *paramEnd = '\0';
                data->bodySize = paramEnd - paramStart;
                data->body = malloc(data->bodySize + 1);
                memcpy(data->body, paramStart + 1, data->bodySize);
                data->body[data->bodySize] = '\0';
            }
            break;
        }
    }
    if (data->body != NULL) {
        //解析请求体参数
        char* p = data->body;
        char* end = data->body + data->bodySize;
        char* lineStart = p;
        while (p < end) {
            if (*p == '&' || p == end - 1) {
                if (p == end - 1) {
                    p++;
                }
                *p = '\0';
                p++;
                char* keyStart = lineStart;
                char* keyEnd = keyStart;
                while (*keyEnd != '=') {
                    keyEnd++;
                }
                *keyEnd = '\0';
                keyEnd++;
                char* valueStart = keyEnd;
                char* valueEnd = valueStart;
                while (*valueEnd != '\0') {
                    valueEnd++;
                }
                valueEnd--;
                while (*valueEnd == ' ') {
                    valueEnd--;
                }
                valueEnd++;
                *valueEnd = '\0';
                data->paramSize++;
                data->params = (SocketPair *)realloc(data->params, sizeof(SocketPair) * data->paramSize);
                data->params[data->paramSize - 1].key = keyStart;
                data->params[data->paramSize - 1].value = valueStart;
                lineStart = p;
            }
            else {
                p++;
            }
        }
    }
    return data;
}

//paramsGET
static inline SocketPair* paramsGET(SocketData* data, char* key) {
    for (int i = 0; i < data->paramSize; i++) {
        if (strcmp(data->params[i].key, key) == 0) {
            return &data->params[i];
        }
    }
    return NULL;
}

//400
static inline void do400BadRequest(SocketData* data, SOCKET connfd) {
    freeSocketData(data);
    send_msg(connfd, "HTTP/1.1 400 Bad Request\r\nAccess-Control-Allow-Origin: *\r\n\r\n", 0);
    socket_close(connfd);
}
#endif
