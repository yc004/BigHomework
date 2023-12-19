#include "webuiapi.h"

/**
    请完善此函数，实现登录功能
    参数：acc为账号，pwd为密码
    返回token，如果登录失败，返回NULL
    token尽量保证随机性（作为身份认证），是长度不超过128字节的HEX字符串
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    返回的指针一定要可以被释放（malloc或者realloc出来的指针），提交完成会自动走free流程
*/
char* webuiapi_login(char* acc, char* pwd) {
    // 这里mock一个token，实际应用中请绑定账号和密码与token关系
    char* token = malloc(129);

    int size;
    char* file = readFile("../test.csv", &size);
    // printf("******************%d", size);
    // printf("******************%s", file);
    const TinyCsvWebUIData* list = TinyCsv_load(file);
    const TinyCsvWebUIData* current = list;

    // printf("*****************%s %s", acc, pwd);
    // 匹配用户名密码 正确返回token
    while (current != NULL) {
        if (strcmp(current->data.accpwd.acc, acc) == 1 && strcmp(current->data.accpwd.pwd, pwd) == 1) {
            strcpy(token, "1234567890ABCDEFFEDCBA0987654321");
            printf("*****************%s %s", acc, pwd);
            free(file);
            return token;
        }
        current = current->next;
    }
    // return token;
    // 如果用户名不存在或密码错误则返回NULL
    free(file);
    return NULL;
}

/**
    请完善此函数，实现注册功能
    参数：acc为账号，pwd为密码
    返回code：0为成功，1为失败，2为账号已存在。
*/
int webuiapi_register(char* acc, char* pwd) {
    // 这里mock一个code为0的返回值
    // int code = 0;
    int code = 1;
    // int code = 2;


    return code;
}


/**
    请完善此函数，实现token验证功能
    参数：token
    返回code：0为成功，1为失败。
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
*/
int webuiapi_checkToken(char* token) {
    // 这里mock一个code为0的返回值
    // int code = 0;
    int code = 1;
    return code;
}


/**
    请完善此函数，实现token退出功能
    参数：token
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
*/
void webuiapi_quitToken(char* token) {
}

/**
    请完善此函数，实现uuid生成功能
    参数：token
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
    返回的指针一定要可以被释放（malloc或者realloc出来的指针），提交完成会自动走free流程
*/
char* tinycsv_getuuid(char* token) {
    // 这里mock一个uuid，实际应用中请绑定账号和密码与token关系
    char* uuid = malloc(129);
    strcpy(uuid, "1234567890ABCDEFFEDCBA0987654321");
    return uuid;
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
char* webuiapi_getDataList(char* token, int sortType, char* orderType, int queryType, char* search) {
    // 这里mock一个列表，实际应用中请绑定账号和密码与token关系
    char* csv = malloc(1024);
    strcpy(csv, "uuid,type,itemName,file,string,acc,pwd,createTime,updateTime""\r\n"
           "1,file,6974656d41,312e747874,,,,2023-12-12 12:12:12,2023-12-12 12:12:12""\r\n"
           "2,string,6974656d42,,68656c6c6f20776f726c64,,,2023-12-12 12:12:12,2023-12-12 12:12:12""\r\n"
           "3,accpwd,6974656d43,,,61646d696e,3132333435363738,2023-12-12 12:12:12,2023-12-12 12:12:12""\r\n");
    //     strcpy(csv, "uuid,type,itemName,file,string,acc,pwd,createTime,updateTime""\r\n"
    //    "1,file,57696e546f70,443a5c50726f6772616d2046696c65732028783836295c57696e546f705c57696e546f702e657865,,,,2023-1-2 3:4:5,2023-1-2 3:4:5""\r\n"
    //     // "2,string,7374724974656d,,B0A2B0CDB0A2B0CD,,,2023-2-3 4:5:6,2023-2-3 4:5:6""\r\n"
    //     // "3,accpwd,CAD7CAA6D5CBBAC5,,,313233343536,363534333231,2023-3-4 5:6:7,2023-3-4 5:6:7""\r\n"
    //     );
    return csv;
}

/**
    请完善此函数，实现申请获取数据列表功能
    参数：token、uuid
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
    返回的指针一定要可以被释放（malloc或者realloc出来的指针），提交完成会自动走free流程
*/
char* webuiapi_getDataByUUID(char* token, char* uuid) {
    // 这里mock一个列表，实际应用中请绑定账号和密码与token关系
    char* csv = malloc(1024);
    strcpy(csv, "uuid,type,itemName,file,string,acc,pwd,createTime,updateTime""\r\n"
           "1,file,6974656d41,312e747874,,,,2023-12-12 12:12:12,2023-12-12 12:12:12""\r\n"
           // "2,string,6974656d42,,68656c6c6f20776f726c64,,,2023-12-12 12:12:12,2023-12-12 12:12:12""\r\n"
           // "3,accpwd,6974656d43,,,61646d696e,3132333435363738,2023-12-12 12:12:12,2023-12-12 12:12:12""\r\n"
    );
    //     strcpy(csv, "uuid,type,itemName,file,string,acc,pwd,createTime,updateTime""\r\n"
    // // "1,file,57696e546f70,443a5c50726f6772616d2046696c65732028783836295c57696e546f705c57696e546f702e657865,,,,2023-1-2 3:4:5,2023-1-2 3:4:5""\r\n"
    // // "2,string,7374724974656d,,B0A2B0CDB0A2B0CD,,,2023-2-3 4:5:6,2023-2-3 4:5:6""\r\n"
    // "3,accpwd,CAD7CAA6D5CBBAC5,,,313233343536,363534333231,2023-3-4 5:6:7,2023-3-4 5:6:7""\r\n"
    // );
    return csv;
}

/**
    请完善此函数，实现申请修改指定数据功能
    参数：token、uuid、itemName、string、acc、pwd
    其中string、acc、pwd为可选参数，请根据数据列表中的type字段判断是否使用
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
*/
int webuiapi_editItem(char* token, char* uuid, char* itemName, char* string, char* acc, char* pwd) {
    // 这里mock一个code为0的返回值
    int code = 0;
    // int code = 1;
    return code;
}

/**
    请完善此函数，实现删除指定uuid的数据功能
    参数：token、uuid
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
*/
int webuiapi_deleteItem(char* token, char* uuid) {
    // 这里mock一个code为0的返回值
    int code = 0;
    // int code = 1;
    return code;
}

/**
    请完善此函数，实现还原文件功能
    参数：token、uuid
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
*/
int webuiapi_decryptFile(char* token, char* uuid) {
    // 这里mock一个code为0的返回值
    int code = 0;
    // int code = 1;
    return code;
}

/**
    请完善此函数，实现申请添加数据功能
    参数：token、uuid、itemName、string、acc、pwd、file
    其中string、acc、pwd、file为可选参数，请根据itype判断是否使用
    需要管理token与账号的关系，账号也会退出登录时，token也要失效，没退出但是直接重登陆，token也要失效
    需要管理uuid和数据列表的关系，它不能重复，uuid对应着每个item的唯一标识
*/
int webuiapi_addItem(char* token, int itype, char* name, char* acc, char* pwd, char* string, char* file) {
    // 这里mock一个code为0的返回值
    int code = 0;
    // int code = 1;
    return code;
}

/**
    请完善此函数，实现获取你的学号功能
    参数：无
    返回值：你的学号数字
*/
unsigned long long webuiapi_getStudentId() {
    return 1231003009;
}
