#ifndef WEBUIAPI_H
#define WEBUIAPI_H
/**
   拦截本机发往的LocalHost请求
   地址：http://localhost:8080/server?api=api&args=args&token=token
   适配Windows
*/
#include "commonutil.h"
#include "tinycsv.h"
#include <unistd.h>
#include <sys/types.h>
//windows环境别忘记编译时加上-lws2_32

/// ==================================== WebUIAPI ====================================
char* webuiapi_login(char* acc, char* pwd);

int webuiapi_register(char* acc, char* pwd);

int webuiapi_checkToken(const char* token);

void webuiapi_quitToken(char* token);

char* webuiapi_getDataList(const char* token, int sortType, char* orderType, int queryType, char* search);

char* webuiapi_getDataByUUID(char* token, const char* uuid);

int webuiapi_editItem(char* token, char* uuid, char* itemName, char* string, char* acc, char* pwd);

int webuiapi_deleteItem(char* token, char* uuid);

int webuiapi_decryptFile(char* token, char* uuid);

int webuiapi_addItem(char* token, int itype, char* name, char* acc, char* pwd, char* string, char* file);

unsigned long long webuiapi_getStudentId();

/// ==================================== WebUIAPI ====================================


static void doLocalProxy() {
resetdoLocalProxy:
    if (socket_init() == -1) {
        perror("Error: failed to initialize socket library\n");
        return;
    }
    int server_sockfd = socket_create();
    if (server_sockfd == -1) {
        perror("Error: failed to create server socket\n");
        return;
    }
    if (set_socket_timeout(server_sockfd, 60, 60) == -1) {
        perror("Error: failed to set socket timeout\n");
        return;
    }
    if (socket_bind(server_sockfd, HOST, PORT) == -1) {
        char err[100];
        sprintf(err, "Error: failed to bind server socket to  %s:%d\n", HOST, PORT);
        perror(err);
        return;
    }
    if (socket_listen(server_sockfd) == -1) {
        perror("Error: failed to listen on server socket\n");
        return;
    }
    printf("Info: Server is listening on %s:%d\n", HOST, PORT);
    while (1) {
        int client_sockfd = socket_accept(server_sockfd);
        if (client_sockfd == -1) {
            fprintf(stderr, "Error: failed to accept connection on server socket\n");
            return;
        }
        int bufsize = RECV_BUFF_SIZE;
        if (setsockopt(client_sockfd, SOL_SOCKET, SO_RCVBUF, (const char *)&bufsize, sizeof(bufsize)) == -1) {
            perror("setsockopt: failed to set SO_RCVBUF option\n");
            return;
        }
        if (setsockopt(client_sockfd, SOL_SOCKET, SO_SNDBUF, (const char *)&bufsize, sizeof(bufsize)) == -1) {
            perror("setsockopt: failed to set SO_SNDBUF option\n");
            return;
        }
        char* buf = malloc(RECV_BUFF_SIZE + 1);
        int r = recv_msg(client_sockfd, buf, RECV_BUFF_SIZE);
        if (r == 0 || r == -1) {
            /*what's up*/
            free(buf);
            printf("Info: HTTP connection closed\n");
            socket_close(client_sockfd);
            socket_close(server_sockfd);
            printf("Info: Server closed client socket %d & server socket %d\n", client_sockfd, server_sockfd);
            socket_cleanup();
            printf("Info: restart...\n");
            goto resetdoLocalProxy;
        }
        SocketData* data = parseSocketData(buf, r);
        if (strstr(data->url, "server") == NULL) {
            do400BadRequest(data, client_sockfd);
            continue;
        }
        SocketPair* api = paramsGET(data, "api");
        char* TXT = NULL;
        if (api == NULL) {
            do400BadRequest(data, client_sockfd);
            continue;
        }
        else if (strcmp(api->value, "checkCharset") == 0) {
            TXT = malloc(100);
            //暂时只考虑GBK和UTF-8情况
#ifdef _WIN32
            //获取系统默认编码
            UINT codepage = GetACP();
            if (codepage == 65001) {
                sprintf(TXT, "UTF-8");
            }
            else {
                sprintf(TXT, "GBK");
            }
#else
// #elif defined(__linux__) || defined(__APPLE__)
            //获取系统默认编码
            char* locale = setlocale(LC_ALL, NULL);
            if (strcmp(locale, "zh_CN.UTF-8") == 0) {
                sprintf(TXT, "UTF-8");
            } else {
                sprintf(TXT, "GBK");
            }
#endif
        }
        else if (strcmp(api->value, "login") == 0) {
            SocketPair* username = paramsGET(data, "acc");
            SocketPair* password = paramsGET(data, "pwd");
            if (username == NULL || password == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* acc = decHex(username->value, NULL);
            char* pwd = decHex(password->value, NULL);
            if (acc == NULL || pwd == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            TXT = webuiapi_login(acc, pwd);
            free(acc);
            free(pwd);
        }
        else if (strcmp(api->value, "register") == 0) {
            SocketPair* username = paramsGET(data, "acc");
            SocketPair* password = paramsGET(data, "pwd");
            if (username == NULL || password == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* acc = decHex(username->value, NULL);
            char* pwd = decHex(password->value, NULL);
            if (acc == NULL || pwd == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            int code = webuiapi_register(acc, pwd);
            TXT = malloc(10);
            sprintf(TXT, "%d", code);
            free(acc);
            free(pwd);
        }
        else if (strcmp(api->value, "checkToken") == 0) {
            SocketPair* token = paramsGET(data, "token");
            if (token == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* tk = token->value;
            if (tk == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            int code = webuiapi_checkToken(tk);
            TXT = malloc(10);
            sprintf(TXT, "%d", code);
        }
        else if (strcmp(api->value, "quitToken") == 0) {
            SocketPair* token = paramsGET(data, "token");
            if (token == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* tk = token->value;
            if (tk == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            webuiapi_quitToken(tk);
        }
        else if (strcmp(api->value, "getDataList") == 0) {
            SocketPair* token = paramsGET(data, "token");
            SocketPair* sort = paramsGET(data, "sort");
            SocketPair* order = paramsGET(data, "order");
            SocketPair* query = paramsGET(data, "query");
            SocketPair* search = paramsGET(data, "search");
            if (token == NULL || sort == NULL || query == NULL || search == NULL || order == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* tk = token->value;
            char* so = sort->value;
            char* qu = query->value;
            char* od = order->value;
            if (tk == NULL || so == NULL || qu == NULL || od == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* searchStr = decHex(search->value, NULL);
            if (searchStr == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            int sortType = 1;
            if (strstr(so, "desc") != NULL) {
                sortType = 0;
            }
            int queryType = 0;
            if (strstr(qu, "file") != NULL) {
                queryType |= TINY_CSV_TYPE_FILE;
            }
            if (strstr(qu, "string") != NULL) {
                queryType |= TINY_CSV_TYPE_STRING;
            }
            if (strstr(qu, "accpwd") != NULL) {
                queryType |= TINY_CSV_TYPE_ACCPWD;
            }
            if (strstr(qu, "*") != NULL) {
                queryType |= TINY_CSV_TYPE_FILE | TINY_CSV_TYPE_STRING | TINY_CSV_TYPE_ACCPWD;
            }
            TXT = webuiapi_getDataList(tk, sortType, od, queryType, searchStr);
            free(searchStr);
        }
        else if (strcmp(api->value, "getDataByUUID") == 0) {
            SocketPair* token = paramsGET(data, "token");
            SocketPair* uuid = paramsGET(data, "uuid");
            if (token == NULL || uuid == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* tk = token->value;
            char* id = uuid->value;
            if (tk == NULL || id == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            TXT = webuiapi_getDataByUUID(tk, id);
        }
        else if (strcmp(api->value, "editItem") == 0) {
            SocketPair* token = paramsGET(data, "token");
            SocketPair* uuid = paramsGET(data, "uuid");
            SocketPair* itemName = paramsGET(data, "itemName");
            if (token == NULL || uuid == NULL || itemName == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* tk = token->value;
            char* id = uuid->value;
            char* name = decHex(itemName->value, NULL);
            if (tk == NULL || id == NULL || name == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char *T_acc, *T_pwd, *T_string;
            T_acc = T_pwd = T_string = NULL;
            SocketPair* acc = paramsGET(data, "acc");
            SocketPair* pwd = paramsGET(data, "pwd");
            SocketPair* string = paramsGET(data, "string");
            if (acc != NULL) {
                T_acc = decHex(acc->value, NULL);
            }
            if (pwd != NULL) {
                T_pwd = decHex(pwd->value, NULL);
            }
            if (string != NULL) {
                T_string = decHex(string->value, NULL);
            }
            int code = webuiapi_editItem(tk, id, name, T_acc, T_pwd, T_string);
            TXT = malloc(10);
            sprintf(TXT, "%d", code);
            free(name);
            if (T_acc != NULL) free(T_acc);
            if (T_pwd != NULL) free(T_pwd);
            if (T_string != NULL) free(T_string);
        }
        else if (strcmp(api->value, "deleteItem") == 0) {
            SocketPair* token = paramsGET(data, "token");
            SocketPair* uuid = paramsGET(data, "uuid");
            if (token == NULL || uuid == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* tk = token->value;
            char* id = uuid->value;
            if (tk == NULL || id == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            int code = webuiapi_deleteItem(tk, id);
            TXT = malloc(10);
            sprintf(TXT, "%d", code);
        }
        else if (strcmp(api->value, "decryptFile") == 0) {
            SocketPair* token = paramsGET(data, "token");
            SocketPair* uuid = paramsGET(data, "uuid");
            if (token == NULL || uuid == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* tk = token->value;
            char* id = uuid->value;
            if (tk == NULL || id == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            int code = webuiapi_decryptFile(tk, id);
            TXT = malloc(10);
            sprintf(TXT, "%d", code);
        }
        else if (strcmp(api->value, "addItem") == 0) {
            SocketPair* token = paramsGET(data, "token");
            SocketPair* type = paramsGET(data, "type");
            SocketPair* itemName = paramsGET(data, "itemName");
            if (token == NULL || type == NULL || itemName == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* tk = token->value;
            char* tp = type->value;
            if (tk == NULL || tp == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char* name = decHex(itemName->value, NULL);
            if (name == NULL) {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            char *T_acc, *T_pwd, *T_string, *T_file;
            T_acc = T_pwd = T_string = T_file = NULL;
            int itype = TINY_CSV_TYPE_UNKNOWN;
            if (strcmp(tp, "accpwd") == 0) {
                itype = TINY_CSV_TYPE_ACCPWD;
                SocketPair* acc = paramsGET(data, "acc");
                SocketPair* pwd = paramsGET(data, "pwd");
                if (acc == NULL || pwd == NULL) {
                    do400BadRequest(data, client_sockfd);
                    continue;
                }
                T_acc = decHex(acc->value, NULL);
                T_pwd = decHex(pwd->value, NULL);
                if (T_acc == NULL || T_pwd == NULL) {
                    do400BadRequest(data, client_sockfd);
                    continue;
                }
            }
            else if (strcmp(tp, "string") == 0) {
                itype = TINY_CSV_TYPE_STRING;
                SocketPair* string = paramsGET(data, "string");
                if (string == NULL) {
                    do400BadRequest(data, client_sockfd);
                    continue;
                }
                T_string = decHex(string->value, NULL);
                if (T_string == NULL) {
                    do400BadRequest(data, client_sockfd);
                    continue;
                }
            }
            else if (strcmp(tp, "file") == 0) {
                itype = TINY_CSV_TYPE_FILE;
                SocketPair* file = paramsGET(data, "file");
                if (file == NULL) {
                    do400BadRequest(data, client_sockfd);
                    continue;
                }
                T_file = decHex(file->value, NULL);
                if (T_file == NULL) {
                    do400BadRequest(data, client_sockfd);
                    continue;
                }
            }
            else {
                do400BadRequest(data, client_sockfd);
                continue;
            }
            int code = webuiapi_addItem(tk, itype, name, T_acc, T_pwd, T_string, T_file);
            TXT = malloc(10);
            sprintf(TXT, "%d", code);
            free(name);
            if (T_acc != NULL) free(T_acc);
            if (T_pwd != NULL) free(T_pwd);
            if (T_string != NULL) free(T_string);
            if (T_file != NULL) free(T_file);
        }
        else if (strcmp(api->value, "getStudentId") == 0) {
            unsigned long long id = webuiapi_getStudentId();
            TXT = malloc(50);
            sprintf(TXT, "%llu", id);
        }
        //复用buf，反正不会再用了
        if (TXT != NULL) {
            sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nAccess-Control-Allow-Origin: *\r\n\r\n%s",
                    strlen(TXT), TXT);
            free(TXT);
        }
        else {
            strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
        }
        send_msg(client_sockfd, buf, 0);
        freeSocketData(data);
        socket_close(client_sockfd);
        printf("Info: Server closed client socket %d\n", client_sockfd);
    }
    socket_close(server_sockfd);
    printf("Info: Server closed server socket %d\n", server_sockfd);
    socket_cleanup();
    printf("Info: Server cleaned up socket library\n");
}

#endif //WEBUIAPI_H
