#include "webuiapi.h"


// 全局储存账户信息 用于和前端token进行验证
static struct {
    char* acc;
    char* pwd;
    char* token;
} account;


char* generateToken() {
    char* token = malloc(sizeof(char) * 17);
    srand(time(NULL));

    const char charset[] = "0123456789ABCDEF";

    for (int i = 0; i < 16; ++i) {
        const int index = rand() % (int)(sizeof charset - 1);
        token[i] = charset[index];
    }
    token[16] = '\0';

    return token;
}


/*char* generateItemName() {
    char* itemName = malloc(sizeof(char) * 11);
    srand(time(NULL));

    const char charset[] = "0123456789";

    for (int i = 0; i < 10; ++i) {
        const int index = rand() % (int)(sizeof charset - 1);
        itemName[i] = charset[index];
    }

    itemName[10] = '\0';

    return itemName;
}*/

/**
    请完善此函数，实现登录功能
    参数：acc为账号，pwd为密码
    返回token，如果登录失败，返回NULL
    token尽量保证随机性（作为身份认证），是长度不超过128字节的HEX字符串
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    返回的指针一定要可以被释放（malloc或者realloc出来的指针），提交完成会自动走free流程
    *已完成*
*/
char* webuiapi_login(const char* acc, const char* pwd) {
    // 这里mock一个token，实际应用中请绑定账号和密码与token关系
    char* token = generateToken();

    char* file = readFile("../test.csv", NULL);
    const TinyCsvWebUIData* list = TinyCsv_load(file);
    const TinyCsvWebUIData* current = list;
    free(file);

    account.acc = malloc(sizeof(char) * 129);
    account.pwd = malloc(sizeof(char) * 129);
    account.token = malloc(sizeof(char) * 17);

    // 匹配用户名密码 正确返回token
    while (current != NULL) {
        if (current->type == TINY_CSV_TYPE_ACCPWD && strcmp(current->data.accpwd.acc, acc) == 0 && strcmp(
                current->data.accpwd.pwd, pwd) == 0) {
            strcpy(account.acc, acc);
            strcpy(account.pwd, pwd);
            strcpy(account.token, token);

            // 在登录时同时检测应用所在位置是否存在存储加密文件的文件夹 没有的话就新建一个
            if (!dirExists("./cryptedFile")) {
                createDir("./cryptedFile");
                printf("dir create successful!");
            }

            return token;
        }
        current = current->next;
    }
    // return token;
    // 如果用户名不存在或密码错误则返回NULL
    // token 置空
    free(account.acc);
    free(account.pwd);
    free(account.token);

    return NULL;
}

/**
    请完善此函数，实现注册功能
    参数：acc为账号，pwd为密码
    返回code：0为成功，1为失败，2为账号已存在。
    *已完成*
*/
/*
 * 1.判断用户名是否存在 已经存在则返回2 表示用户名已存在
 * 2.如果用户名不存在则将用户名密码写入文件，并返回0
 * 3.如果出现其他问题则返回1
 */
int webuiapi_register(char* acc, char* pwd) {
    // 这里mock一个code为0的返回值
    // int code = 0;
    // int code = 1;
    // int code = 2;
    // 读取csv文件
    int size;
    char* file = readFile("../test.csv", &size);
    TinyCsvWebUIData* list = TinyCsv_load(file);
    TinyCsvWebUIData* current = list;
    const char* currentTime = fmtYmdHMS("%Y/%m/%d %H:%M:%S", getTimestamp());

    while (list->itemName != NULL && current != NULL) {
        if (current->type == TINY_CSV_TYPE_ACCPWD) {
            if (strcmp(current->data.accpwd.acc, acc) == 0) {
                return 2;
            }
        }
        current = current->next;
    }

    // 让指针重新指向最后一个节点
    current = list;
    while (list->itemName != NULL && current->next != NULL) {
        current = current->next;
    }

    TinyCsvWebUIData* new = new_TinyCsvWebUIData_ACCPWD(tinycsv_getuuid("inner"), "registeredAcc", acc, pwd,
                                                        currentTime, currentTime);
    // 将新的节点链接至原链表的尾部
    if (current->itemName != NULL) {
        current->next = new;
    }
    else {
        list = new;
    }

    if (writeFile("../test.csv", TinyCsv_dump(list), 0)) {
        // 正常结束
        return 0;
    }
    // 其他异常造成添加失败
    return 1;
}


/**
    请完善此函数，实现token验证功能
    参数：token
    返回code：0为成功，1为失败。
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    *已完成*
*/
int webuiapi_checkToken(char* token) {
    if (account.token == NULL || strcmp(account.token, token) != 0) {
        webuiapi_quitToken(token);
        return 1;
    }
    return 0;
}


/**
    请完善此函数，实现token退出功能
    参数：token
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    *已完成*
*/
void webuiapi_quitToken(char* token) {
    // 让当前后台账户的token恢复为空字符串 token失效
    free(account.acc);
    free(account.pwd);
    free(account.token);
    // free(token);
}

/**
    请完善此函数，实现uuid生成功能
    参数：token
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
    返回的指针一定要可以被释放（malloc或者realloc出来的指针），提交完成会自动走free流程
    *已完成*
*/
char* tinycsv_getuuid(const char* token) {
    char* csv = readFile("../test.csv", NULL);
    const TinyCsvWebUIData* list = TinyCsv_load(csv);
    const TinyCsvWebUIData* cur = list;

    int max = atoi(cur->uuid);
    cur = cur->next;
    // 找到当前链表中的最大uuid
    while (cur != NULL) {
        if (atoi(cur->uuid) > max) {
            max = atoi(cur->uuid);
        }
        cur = cur->next;
    }
    char* uuid = malloc(sizeof(char) * 128);
    // 让新的uuid为最大uuid + 1
    itoa(max + 1, uuid, 10);
    return uuid;
}

char* parsefile(const char* path, char** type) {
    const char* fileName = NULL;
    char* filePath = malloc(sizeof(char) * 128);
    strcpy(filePath, path);
    for (; *filePath != '\0'; ++filePath) {
        if (*filePath == '\\') {
            fileName = ++filePath;
        }
        else if (*filePath == '.') {
            *filePath = '\0';
            if (type != NULL) {
                *type = ++filePath;
            }
        }
    }

    char* storePath = malloc(sizeof(char) * 128);
    strcpy(storePath, "./cryptedFile/");
    strcat(storePath, fileName);

    return storePath;
}

void swap(TinyCsvWebUIData* a_p, TinyCsvWebUIData* b_p) {
    if (a_p == b_p) {
        return;
    }
    const TinyCsvWebUIData temp = *a_p;
    a_p->data = b_p->data;
    a_p->type = b_p->type;
    a_p->uuid = b_p->uuid;
    strcpy(a_p->createTime, b_p->createTime);
    strcpy(a_p->updateTime, b_p->updateTime);
    a_p->itemName = b_p->itemName;

    b_p->uuid = temp.uuid;
    b_p->type = temp.type;
    strcpy(b_p->updateTime, temp.updateTime);
    strcpy(b_p->createTime, temp.createTime);
    b_p->data = temp.data;
    b_p->itemName = temp.itemName;
}

/**
    请完善此函数，实现申请获取数据列表功能
    参数：token、sortType、orderType、queryType、search
    sortType为真，升序排列，为假，降序排列
    orderType为排序字段，可选值为：uuid、itemName、createTime、updateTime
    queryType使用提供的宏即可判断
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
    返回的指针一定要可以被释放（malloc或者realloc出来的指针），提交完成会自动走free流程
    问题很大
*/
char* webuiapi_getDataList(char* token, const int sortType, const char* orderType, const int queryType,
                           char* search) {
    // token鉴权
    if (webuiapi_checkToken(token)) {
        return NULL;
    }

    char* csv = readFile("../test.csv", NULL);
    TinyCsvWebUIData* head = TinyCsv_load(csv);
    TinyCsvWebUIData* cur = head;
    TinyCsvWebUIData* prev = NULL;


    // 先按照筛选条件挑出满足条件的元素
    // 同时计算链表长度 size 用于后续选择排序
    int size = 0;
    while (cur != NULL) {
        // 指定的文件类型筛选
        // 当元素的类型和函数指定的类型做 按位与 运算值为 0
        // 说明当前元素不是前端需要的元素 将该节点删除即可
        // 将search参数和itemName进行匹配筛选 如果search参数为空字符串, 则不匹配
        if ((cur->type & queryType) == 0 || (strcmp(search, "") == 0 ? FALSE : strstr(cur->itemName, search) == NULL)) {
            if (cur != head && prev != NULL) {
                prev->next = cur->next;
                cur = cur->next;
                continue;
            }
            // 头节点情况
            head = cur->next;
            prev = NULL;
            cur = head;
            continue;
        }
        size++;
        prev = cur;
        cur = cur->next;
    }


    // 使用选择排序对链表进行排序
    // 当前节点重新指向链表头部

    TinyCsvWebUIData* target = NULL;

    // 升序
    if (sortType) {
        for (cur = head; cur != NULL; cur = cur->next) {
            target = cur;
            for (TinyCsvWebUIData* temp = cur->next; temp != NULL; temp = temp->next) {
                if (strcmp(orderType, "uuid") == 0 && atoi(cur->uuid) > atoi(temp->uuid)) {
                    target = temp;
                }
                else if (strcmp(orderType, "itemName") == 0 && strcmp(cur->itemName, temp->itemName) > 0) {
                    target = temp;
                }
                else if (strcmp(orderType, "createTime") == 0 && getTimestampByStr(cur->createTime) > getTimestampByStr(
                             temp->createTime)) {
                    target = temp;
                }
                else if (strcmp(orderType, "updateTime") == 0 && getTimestampByStr(cur->updateTime) >
                         getTimestampByStr(temp->updateTime)) {
                    target = temp;
                }
            }
            // 找到最小值节点并交换
            swap(target, cur);
        }
    }
    // 降序
    else {
        for (cur = head; cur != NULL; cur = cur->next) {
            target = cur;
            for (TinyCsvWebUIData* temp = cur->next; temp != NULL; temp = temp->next) {
                if (strcmp(orderType, "uuid") == 0 && atoi(cur->uuid) < atoi(temp->uuid)) {
                    target = temp;
                }
                else if (strcmp(orderType, "itemName") == 0 && strcmp(cur->itemName, temp->itemName) < 0) {
                    target = temp;
                }
                else if (strcmp(orderType, "createTime") == 0 && getTimestampByStr(cur->createTime) < getTimestampByStr(
                             temp->createTime)) {
                    target = temp;
                }
                else if (strcmp(orderType, "updateTime") == 0 && getTimestampByStr(cur->updateTime) <
                         getTimestampByStr(temp->updateTime)) {
                    target = temp;
                }
            }
            // 找到最小值节点并交换
            swap(target, cur);
        }
    }


    // 转为字符串返回前端
    csv = TinyCsv_dump(head);
    return csv;
}

/**
    请完善此函数，实现申请获取数据列表功能
    参数：token、uuid
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
    返回的指针一定要可以被释放（malloc或者realloc出来的指针），提交完成会自动走free流程
    *已完成*
*/
char* webuiapi_getDataByUUID(const char* token, const char* uuid) {
    // token 鉴权
    if (account.token == NULL || strcmp(token, account.token) != 0) {
        return NULL;
    }

    char* csv = readFile("../test.csv", NULL);
    const TinyCsvWebUIData* list = TinyCsv_load(csv);
    const TinyCsvWebUIData* cur = list;

    // 查找对应元素
    while (cur != NULL) {
        if (strcmp(uuid, cur->uuid) == 0) {
            TinyCsvWebUIData data = *cur;
            data.next = NULL;
            return TinyCsv_dump(&data);
        }
        cur = cur->next;
    }
    return NULL;
}

/**
    请完善此函数，实现申请修改指定数据功能
    参数：token、uuid、itemName、string、acc、pwd
    其中string、acc、pwd为可选参数，请根据数据列表中的type字段判断是否使用
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
    *已完成*
*/
int webuiapi_editItem(const char* token, const char* uuid, char* itemName, const char* string, const char* acc,
                      const char* pwd) {
    // 这里mock一个code为0的返回值
    // int code = 1;
    // token 鉴权
    if (strcmp(token, account.token) != 0) {
        return 1;
    }

    // 读取文件
    char* csv = readFile("../test.csv", NULL);
    TinyCsvWebUIData* list = TinyCsv_load(csv);
    TinyCsvWebUIData* cur = list;
    free(csv);

    // 获取当前时间
    char* currentTime = fmtYmdHMS("%Y/%m/%d %H:%M:%S", getTimestamp());


    while (cur != NULL) {
        if (strcmp(cur->uuid, uuid) == 0) {
            cur->itemName = itemName;
            if (cur->type == TINY_CSV_TYPE_ACCPWD) {
                strcpy(cur->data.accpwd.acc, acc);
                strcpy(cur->data.accpwd.pwd, pwd);
                strcpy(cur->updateTime, currentTime);
            }
            else if (cur->type == TINY_CSV_TYPE_STRING) {
                strcpy(cur->data.string, string);
                strcpy(cur->updateTime, currentTime);
            }
            free(currentTime);

            // 数据写入文件
            if (writeFile("../test.csv", TinyCsv_dump(list), 0)) {
                return 0;
            }
            return 1;
        }
        cur = cur->next;
    }

    // 目标不存在
    return 2;
}

/**
    请完善此函数，实现删除指定uuid的数据功能
    参数：token、uuid
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
    *已完成*
*/
int webuiapi_deleteItem(char* token, const char* uuid) {
    // token 鉴权
    if (webuiapi_checkToken(token)) {
        return 1;
    }

    // 读取文件
    char* csv = readFile("../test.csv", NULL);
    TinyCsvWebUIData* list = TinyCsv_load(csv);
    TinyCsvWebUIData* cur = list;
    TinyCsvWebUIData* prev = NULL;
    free(csv);

    // 用于判断目标节点是否已经删除
    int changed = FALSE;
    while (cur != NULL) {
        if (strcmp(cur->uuid, uuid) == 0) {
            changed = TRUE;
            if (cur == list && prev == NULL) {
                list = cur->next;
                // 如果需要删除的是文件 需要将存储在程序目录下的已加密文件同时删除
                if (cur->type == TINY_CSV_TYPE_FILE && fileExists(parsefile(cur->data.file, NULL)) && deleteFile(
                        parsefile(cur->data.file, NULL)) != 0) {
                    return 1;
                }
            }
            else if (prev != NULL) {
                prev->next = cur->next;
                // 如果需要删除的是文件 需要将存储在程序目录下的已加密文件同时删除
                if (cur->type == TINY_CSV_TYPE_FILE && fileExists(parsefile(cur->data.file, NULL)) && deleteFile(
                        parsefile(cur->data.file, NULL)) != 0) {
                    return 1;
                }
            }
        }
        prev = cur;
        cur = cur->next;
    }

    if (changed) {
        if (writeFile("../test.csv", TinyCsv_dump(list), 0)) {
            return 0;
        }
        return 1;
    }
    return 2;
}

/**
    请完善此函数，实现还原文件功能
    参数：token、uuid
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
    *已完成*
*/
int webuiapi_decryptFile(char* token, const char* uuid) {
    // token 鉴权
    if (webuiapi_checkToken(token)) {
        return 1;
    }

    char* csv = readFile("../test.csv", NULL);
    const TinyCsvWebUIData* list = TinyCsv_load(csv);
    const TinyCsvWebUIData* cur = list;

    // 查找对应元素
    while (cur != NULL) {
        if (strcmp(uuid, cur->uuid) == 0) {
            if (fileExists(parsefile(cur->data.file, NULL))) {
                if (writeFile(cur->data.file, decBase64(readFile(parsefile(cur->data.file, NULL), NULL), 0), 0)) {
                    // // 解码后在数据库中删除该元素
                    // if (webuiapi_deleteItem(token, uuid)) {
                    //     return 1;
                    // }
                    // return 0;

                    // 写入成功后删除加密库里面的文件
                    // deleteFile(parsefile(cur->data.file, NULL));
                    return 0;
                }
                // 写入错误
                return 1;
            }
            // 文件不存在错误
            return 1;
        }
        cur = cur->next;
    }

    // 目标未找到
    return 2;
}

/**
    请完善此函数，实现申请添加数据功能
    参数：token、uuid、itemName、string、acc、pwd、file
    其中string、acc、pwd、file为可选参数，请根据itype判断是否使用
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
    *已完成*
*/
int webuiapi_addItem(char* token, const int itype, char* name, char* acc, char* pwd, char* string, const char* file) {
    // 这里mock一个code为0的返回值
    // int code = 0;
    // int code = 1;
    // token 鉴权
    if (webuiapi_checkToken(token)) {
        return 1;
    }

    // 读取文件
    char* csv = readFile("../test.csv", NULL);
    TinyCsvWebUIData* list = TinyCsv_load(csv);
    TinyCsvWebUIData* tail = list;
    free(csv);

    // 将指针至于链表尾部
    while (tail->next != NULL && list != NULL) {
        tail = tail->next;
    }

    TinyCsvWebUIData* new = NULL;
    // 获取当前时间
    char* currentTime = fmtYmdHMS("%Y/%m/%d %H:%M:%S", getTimestamp());

    if (itype == TINY_CSV_TYPE_FILE) {
        // 对原始路径进行复制保留
        char* filePath = malloc(sizeof(char) * 128);
        strcpy(filePath, file);

        // 将文件路径记录至数据库
        new = new_TinyCsvWebUIData_FILE(tinycsv_getuuid(token), name, filePath, currentTime, currentTime);
        char* data = readFile(file, NULL);


        // 解析传入的绝对路径地址 找到文件名 并找到文件类型后缀
        char* fileType = NULL;
        const char* storePath = parsefile(file, &fileType);


        // 对原文件进行加密
        const int result = writeFile(storePath, encBase64(data, 0), 0);
        if (result) {
            // 写入成功则删除原文件
            if (deleteFile(filePath) != 0) {
                // 删除失败则返回错误代码 1
                return 1;
            }
        }
        else {
            // 如果写入失败 返回错误代码 1
            free(filePath);
            free(data);
            return 1;
        }
    }
    else if (itype == TINY_CSV_TYPE_ACCPWD) {
        new = new_TinyCsvWebUIData_ACCPWD(tinycsv_getuuid(token), name, acc, pwd, currentTime,
                                          currentTime);
    }
    else if (itype == TINY_CSV_TYPE_STRING) {
        new = new_TinyCsvWebUIData_STRING(tinycsv_getuuid(token), name, string, currentTime, currentTime);
    }

    // 将新节点插入链表尾部
    tail->next = new;

    // 转换为字符串并写入文件
    if (writeFile("../test.csv", TinyCsv_dump(list), 0)) {
        return 0;
    }
    return 1;
}


/**
    请完善此函数，实现获取你的学号功能
    参数：无
    返回值：你的学号数字
    *已完成*
*/
unsigned long long webuiapi_getStudentId() {
    return 1231003009;
}
