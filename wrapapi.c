#include "webuiapi.h"

static struct account {
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


char* generateItemName() {
    char* itemName = malloc(sizeof(char) * 11);
    srand(time(NULL));

    const char charset[] = "0123456789";

    for (int i = 0; i < 10; ++i) {
        const int index = rand() % (int)(sizeof charset - 1);
        itemName[i] = charset[index];
    }

    itemName[10] = '\0';

    return itemName;
}

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

    TinyCsvWebUIData* new = new_TinyCsvWebUIData_ACCPWD(tinycsv_getuuid("inner"), generateItemName(), acc, pwd,
                                                        currentTime, currentTime);
    // 将新的节点链接至原链表的尾部
    if (current->itemName != NULL) {
        current->next = new;
    }
    else {
        list = new;
    }
    char* csv = TinyCsv_dump(list);
    if (writeFile("../test.csv", csv, 0)) {
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
int webuiapi_checkToken(const char* token) {
    if (account.token == NULL) {
        return 1;
    }
    if (strcmp(account.token, token) == 0) {
        return 0;
    }
    return 1;
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

void swap(TinyCsvWebUIData* a_p, TinyCsvWebUIData* b_p) {
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
*/
char* webuiapi_getDataList(const char* token, const int sortType, const char* orderType, const int queryType,
                           char* search) {
    // token鉴权
    if (strcmp(account.token, token) != 0) {
        return NULL;
    }

    char* csv = readFile("../test.csv", NULL);
    TinyCsvWebUIData* head = TinyCsv_load(csv);
    TinyCsvWebUIData* cur = head;
    const TinyCsvWebUIData* prev = NULL;


    // 先按照筛选条件挑出满足条件的元素
    while (cur != NULL) {
        // 指定的文件类型筛选
        // 当元素的类型和函数指定的类型做 按位与 运算值为 0
        // 说明当前元素不是前端需要的元素 将该节点删除即可
        if ((cur->type & queryType) == 0) {
            if (cur != head) {
                prev = cur->next;
            }
            else {
                head = cur->next;
            }
        }
        prev = cur;
        cur = cur->next;
    }

    // 使用选择排序对链表进行排序
    // 当前节点重新指向链表头部
    cur = head;
    // 升序
    if (sortType) {
        while (cur != NULL) {
            TinyCsvWebUIData* p = cur->next;
            while (p != NULL) {
                if (strcmp(orderType, "uuid") == 0 && cur->uuid > p->uuid) {
                    swap(cur, p);
                }
                else if (strcmp(orderType, "itemName") == 0 && strcmp(cur->itemName, p->itemName) > 0) {
                    swap(cur, p);
                }
                else if (strcmp(orderType, "createTime") == 0 && getTimestampByStr(cur->createTime) > getTimestampByStr(
                             p->createTime)) {
                    swap(cur, p);
                }
                else if (strcmp(orderType, "updateTime") == 0 && getTimestampByStr(cur->updateTime) >
                         getTimestampByStr(p->updateTime)) {
                    swap(cur, p);
                }
                p = p->next;
            }
            cur = cur->next;
        }
    }
    // 降序
    else {
        while (cur != NULL) {
            TinyCsvWebUIData* p = cur->next;
            while (p != NULL) {
                if (strcmp(orderType, "uuid") == 0 && cur->uuid < p->uuid) {
                    swap(cur, p);
                }
                else if (strcmp(orderType, "itemName") == 0 && strcmp(cur->itemName, p->itemName) < 0) {
                    swap(cur, p);
                }
                else if (strcmp(orderType, "createTime") == 0 && getTimestampByStr(cur->createTime) < getTimestampByStr(
                             p->createTime)) {
                    swap(cur, p);
                }
                else if (strcmp(orderType, "updateTime") == 0 && getTimestampByStr(cur->updateTime) <
                         getTimestampByStr(p->updateTime)) {
                    swap(cur, p);
                }
                p = p->next;
            }
            cur = cur->next;
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
    if (strcmp(token, account.token) != 0) {
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
int webuiapi_editItem(const char* token, const char* uuid, char* itemName, char* string, const char* acc,
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
int webuiapi_deleteItem(const char* token, const char* uuid) {
    // 这里mock一个code为0的返回值
    // int code = 0;
    // int code = 1;

    // token 鉴权
    if (strcmp(token, account.token) != 0) {
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
            }
            else if (prev != NULL) {
                prev->next = cur->next;
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
int webuiapi_decryptFile(const char* token, const char* uuid) {
    // token 鉴权
    if (strcmp(account.token, token) != 0) {
        return 1;
    }

    char* csv = readFile("../test.csv", NULL);
    const TinyCsvWebUIData* list = TinyCsv_load(csv);
    const TinyCsvWebUIData* cur = list;

    // 查找对应元素
    while (cur != NULL) {
        if (strcmp(uuid, cur->uuid) == 0) {
            if (fileExists(cur->data.file)) {
                if (writeFile(cur->data.file, decBase64(readFile(cur->data.file, NULL), 0), 0)) {
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
int webuiapi_addItem(const char* token, const int itype, char* name, char* acc, char* pwd, char* string, char* file) {
    // 这里mock一个code为0的返回值
    // int code = 0;
    // int code = 1;
    // token 鉴权
    if (strcmp(token, account.token) != 0) {
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
        // 将文件路径记录至数据库
        new = new_TinyCsvWebUIData_FILE(tinycsv_getuuid(token), name, file, currentTime, currentTime);
        // 对原文件进行加密
        if (fileExists(file)) {
            if (!writeFile(file, encBase64(readFile(file, NULL), 0), 0)) {
                // 如果写入失败 返回错误代码 1
                return 1;
            }
        }
        // 指定的文件不存在
        else {
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
