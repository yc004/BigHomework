#ifndef TINYCSV_H
#define TINYCSV_H
#include "commonutil.h"

/// ==================================== TinyCSV.MD ====================================
char* tinycsv_getuuid(const char* token);

/// ==================================== TinyCSV.MD ====================================

#define CSV_HEADER "uuid,type,itemName,file,string,acc,pwd,createTime,updateTime"
//直接修改表头即可自动调整顺序，像这样："uuid,type,createTime,updateTime,file,string,acc,pwd"

typedef enum TinyCsvType {
    TINY_CSV_TYPE_FILE = (1 << 0),
    TINY_CSV_TYPE_STRING = (1 << 1),
    TINY_CSV_TYPE_ACCPWD = (1 << 2),
    TINY_CSV_TYPE_UNKNOWN = -1,
} TinyCsvType;

// 匹配TinyCsvType的宏（用于复合判断，但是注意，数据只能有唯一类型）
#define TINY_CSV_TYPE_CONSISTENT_FILE(type) ((type&TINY_CSV_TYPE_FILE) == 1)
#define TINY_CSV_TYPE_CONSISTENT_STRING(type) ((type&TINY_CSV_TYPE_STRING) == 1)
#define TINY_CSV_TYPE_CONSISTENT_ACCPWD(type) ((type&TINY_CSV_TYPE_ACCPWD) == 1)

typedef struct TinyCsvWebUIData {
    char* uuid; //uuid自行分配吧，就不直接指定尺寸了
    TinyCsvType type;
    char* itemName;
    char createTime[32];
    char updateTime[32];

    union {
        char* file;
        char* string;

        struct {
            char* acc;
            char* pwd;
        } accpwd;
    } data;

    struct TinyCsvWebUIData* next;
} TinyCsvWebUIData;


// TinyCsvWebUIData的GET宏
#define TINY_CSV_DATA_GET_FILE(data) ((data)->data.file)
#define TINY_CSV_DATA_GET_STRING(data) ((data)->data.string)
#define TINY_CSV_DATA_GET_ACC(data) ((data)->data.accpwd.acc)
#define TINY_CSV_DATA_GET_PWD(data) ((data)->data.accpwd.pwd)
#define TINY_CSV_DATA_GET_ITEMNAME(data) ((data)->itemName)
#define TINY_CSV_DATA_GET_TYPE(data) ((data)->type)
#define TINY_CSV_DATA_GET_UUID(data) ((data)->uuid)
#define TINY_CSV_DATA_GET_CREATETIME(data) ((data)->createTime)
#define TINY_CSV_DATA_GET_UPDATETIME(data) ((data)->updateTime)

static void freeTinyCsvWebUIData(TinyCsvWebUIData* data) {
    if (data == NULL)return;
    if (data->uuid != NULL) free(data->uuid);
    if (data->itemName != NULL) free(data->itemName);
    switch (data->type) {
        case TINY_CSV_TYPE_FILE:
            if (data->data.file != NULL) free(data->data.file);
            break;
        case TINY_CSV_TYPE_STRING:
            if (data->data.string != NULL) free(data->data.string);
            break;
        case TINY_CSV_TYPE_ACCPWD:
            if (data->data.accpwd.acc != NULL) free(data->data.accpwd.acc);
            if (data->data.accpwd.pwd != NULL) free(data->data.accpwd.pwd);
            break;
        default:
            break;
    }
    free(data);
}

static void freeTinyCsvWebUIDataList(TinyCsvWebUIData* data) {
    if (data == NULL)return;
    TinyCsvWebUIData* next = data->next;
    freeTinyCsvWebUIData(data);
    freeTinyCsvWebUIDataList(next);
}

static TinyCsvWebUIData* new_TinyCsvWebUIData() {
    TinyCsvWebUIData* data = (TinyCsvWebUIData *)malloc(sizeof(TinyCsvWebUIData));
    memset(data, 0, sizeof(TinyCsvWebUIData));
    return data;
}

static TinyCsvWebUIData* new_TinyCsvWebUIData_FILE(char* uuid, char* itemName, char* file, char* createTime,
                                                   char* updateTime) {
    TinyCsvWebUIData* data = new_TinyCsvWebUIData();
    data->uuid = malloc(strlen(uuid) + 1);
    strcpy(data->uuid, uuid);
    data->itemName = malloc(strlen(itemName) + 1);
    strcpy(data->itemName, itemName);
    data->type = TINY_CSV_TYPE_FILE;
    data->data.file = file;
    strcpy(data->createTime, createTime);
    strcpy(data->updateTime, updateTime);
    return data;
}

static TinyCsvWebUIData* new_TinyCsvWebUIData_STRING(const char* uuid, const char* itemName, char* string, const char* createTime,
                                                     const char* updateTime) {
    TinyCsvWebUIData* data = new_TinyCsvWebUIData();
    data->uuid = malloc(strlen(uuid) + 1);
    strcpy(data->uuid, uuid);
    data->itemName = malloc(strlen(itemName) + 1);
    strcpy(data->itemName, itemName);
    data->type = TINY_CSV_TYPE_STRING;
    data->data.string = string;
    strcpy(data->createTime, createTime);
    strcpy(data->updateTime, updateTime);
    return data;
}

static TinyCsvWebUIData* new_TinyCsvWebUIData_ACCPWD(const char* uuid, const char* itemName, char* acc, char* pwd,
                                                     const char* createTime, const char* updateTime) {
    TinyCsvWebUIData* data = new_TinyCsvWebUIData();
    data->uuid = malloc(strlen(uuid) + 1);
    strcpy(data->uuid, uuid);
    data->itemName = malloc(strlen(itemName) + 1);
    strcpy(data->itemName, itemName);
    data->type = TINY_CSV_TYPE_ACCPWD;
    data->data.accpwd.acc = acc;
    data->data.accpwd.pwd = pwd;
    strcpy(data->createTime, createTime);
    strcpy(data->updateTime, updateTime);
    return data;
}

#define Concat_TinyCsvWebUIData(data, nxt) (data)->next = (nxt)

static char* TinyCsv_dump(TinyCsvWebUIData* data) {
    //第一遍遍历，计算长度
    int memsize = sizeof(CSV_HEADER) + 1 + 2; //HEADER + \r\n
    TinyCsvWebUIData* p = data;
    while (p != NULL) {
        memsize += strlen(p->uuid) + 1; //uuid
        memsize += 64; //createTime
        memsize += 64; //updateTime
        memsize += 16; //type
        switch (p->type) {
            case TINY_CSV_TYPE_FILE:
                memsize += strlen(p->data.file) * 2 + 1;
                break;
            case TINY_CSV_TYPE_STRING:
                memsize += strlen(p->data.string) * 2 + 1;
                break;
            case TINY_CSV_TYPE_ACCPWD:
                memsize += strlen(p->data.accpwd.acc) * 2 + 1;
                memsize += strlen(p->data.accpwd.pwd) * 2 + 1;
                break;
            default:
                break;
        }
        memsize += 64; //,\r\n等等
        p = p->next;
    }
    char* mem = (char *)malloc(memsize);
    memset(mem, 0, memsize);
    p = data;
    char* pMem = mem;
    //表头
    strcpy(pMem, CSV_HEADER);
    pMem += sizeof(CSV_HEADER) - 1;
    *pMem++ = '\r';
    *pMem++ = '\n';
    // 读取表头，找到对应的列，所以先整好idxOf
    int idxOf_uuid = 100;
    int idxOf_type = idxOf_uuid + 1;
    int idxOf_itemName = idxOf_type + 1;
    int idxOf_file = idxOf_itemName + 1;
    int idxOf_string = idxOf_file + 1;
    int idxOf_acc = idxOf_string + 1;
    int idxOf_pwd = idxOf_acc + 1;
    int idxOf_createTime = idxOf_pwd + 1;
    int idxOf_updateTime = idxOf_createTime + 1;
    //解析表头
    char* pHeader = CSV_HEADER;
    char* pHeaderEnd = pHeader + sizeof(CSV_HEADER) - 1;
    int idx = 0;
    while (pHeader < pHeaderEnd) {
        if (*pHeader == ',') {
            idx++;
            pHeader++;
        }
        if (strncmp(pHeader, "uuid", 4) == 0) {
            idxOf_uuid = idx;
            pHeader += 4;
        }
        else if (strncmp(pHeader, "type", 4) == 0) {
            idxOf_type = idx;
            pHeader += 4;
        }
        else if (strncmp(pHeader, "itemName", 8) == 0) {
            idxOf_itemName = idx;
            pHeader += 8;
        }
        else if (strncmp(pHeader, "file", 4) == 0) {
            idxOf_file = idx;
            pHeader += 4;
        }
        else if (strncmp(pHeader, "string", 6) == 0) {
            idxOf_string = idx;
            pHeader += 6;
        }
        else if (strncmp(pHeader, "acc", 3) == 0) {
            idxOf_acc = idx;
            pHeader += 3;
        }
        else if (strncmp(pHeader, "pwd", 3) == 0) {
            idxOf_pwd = idx;
            pHeader += 3;
        }
        else if (strncmp(pHeader, "createTime", 10) == 0) {
            idxOf_createTime = idx;
            pHeader += 10;
        }
        else if (strncmp(pHeader, "updateTime", 10) == 0) {
            idxOf_updateTime = idx;
            pHeader += 10;
        }
        else {
            pHeader++;
        }
    }
    int idxmax = idx;
    //数据
    while (p != NULL) {
        idx = 0;
        while (idx <= idxmax) {
            char* TXT = NULL;
            if (idx == idxOf_uuid) {
                //uuid
                strcpy(pMem, p->uuid);
                pMem += strlen(p->uuid);
                *pMem++ = ',';
                idx++;
            }
            else if (idx == idxOf_type) {
                //type
                switch (p->type) {
                    case TINY_CSV_TYPE_FILE:
                        strcpy(pMem, "file");
                        pMem += 4;
                        break;
                    case TINY_CSV_TYPE_STRING:
                        strcpy(pMem, "string");
                        pMem += 6;
                        break;
                    case TINY_CSV_TYPE_ACCPWD:
                        strcpy(pMem, "accpwd");
                        pMem += 6;
                        break;
                    default:
                        strcpy(pMem, "unknown");
                        pMem += 6;
                        break;
                }
                *pMem++ = ',';
                idx++;
            }
            else if (idx == idxOf_itemName) {
                //itemName
                TXT = encHex(p->itemName, 0);
                strcpy(pMem, TXT);
                pMem += strlen(TXT);
                free(TXT);
                *pMem++ = ',';
                idx++;
            }
            else if (idx == idxOf_file) {
                //file
                if (p->type == TINY_CSV_TYPE_FILE) {
                    TXT = encHex(p->data.file, 0);
                    strcpy(pMem, TXT);
                    pMem += strlen(TXT);
                    free(TXT);
                }
                *pMem++ = ',';
                idx++;
            }
            else if (idx == idxOf_string) {
                //string
                if (p->type == TINY_CSV_TYPE_STRING) {
                    TXT = encHex(p->data.string, 0);
                    strcpy(pMem, TXT);
                    pMem += strlen(TXT);
                    free(TXT);
                }
                *pMem++ = ',';
                idx++;
            }
            else if (idx == idxOf_acc) {
                //acc
                if (p->type == TINY_CSV_TYPE_ACCPWD) {
                    TXT = encHex(p->data.accpwd.acc, 0);
                    strcpy(pMem, TXT);
                    pMem += strlen(TXT);
                    free(TXT);
                }
                *pMem++ = ',';
                idx++;
            }
            else if (idx == idxOf_pwd) {
                //pwd
                if (p->type == TINY_CSV_TYPE_ACCPWD) {
                    TXT = encHex(p->data.accpwd.pwd, 0);
                    strcpy(pMem, TXT);
                    pMem += strlen(TXT);
                    free(TXT);
                }
                *pMem++ = ',';
                idx++;
            }
            else if (idx == idxOf_createTime) {
                //createTime
                strcpy(pMem, p->createTime);
                pMem += strlen(p->createTime);
                *pMem++ = ',';
                idx++;
            }
            else if (idx == idxOf_updateTime) {
                //updateTime
                strcpy(pMem, p->updateTime);
                pMem += strlen(p->updateTime);
                *pMem++ = ',';
                idx++;
            }
        }
        //回滚上个逗号
        pMem--;
        *pMem++ = '\r';
        *pMem++ = '\n';
        p = p->next;
    }
    *pMem = '\0';
    return realloc(mem, pMem - mem + 1); //收缩内存占用
}

static TinyCsvWebUIData* TinyCsv_load(char* csv) {
    // 读取表头，找到对应的列，所以先整好idxOf
    int idxOf_uuid = 0;
    int idxOf_type = idxOf_uuid + 1;
    int idxOf_itemName = idxOf_type + 1;
    int idxOf_file = idxOf_itemName + 1;
    int idxOf_string = idxOf_file + 1;
    int idxOf_acc = idxOf_string + 1;
    int idxOf_pwd = idxOf_acc + 1;
    int idxOf_createTime = idxOf_pwd + 1;
    int idxOf_updateTime = idxOf_createTime + 1;
    char* p = csv;
    // 读取表头绑定
    char* pHeader = p;
    char lastChar = '\0';
    // 将指针移向表头的结尾 即表中第一个数据的开头
    while (*p != '\r' && *p != '\n' && *p != '\0') {
        p++;
    }
    lastChar = *p; //记录临时修改的字符
    *p = '\0';
    // 读取表头
    char* pHeaderEnd = p;
    p = pHeader;
    int idx = 0;
    while (p < pHeaderEnd) {
        if (*p == ',') {
            idx++;
            p++;
        }
        else if (strncmp(p, "uuid", 4) == 0) {
            idxOf_uuid = idx;
            p += 4;
        }
        else if (strncmp(p, "type", 4) == 0) {
            idxOf_type = idx;
            p += 4;
        }
        else if (strncmp(p, "itemName", 8) == 0) {
            idxOf_itemName = idx;
            p += 8;
        }
        else if (strncmp(p, "file", 4) == 0) {
            idxOf_file = idx;
            p += 4;
        }
        else if (strncmp(p, "string", 6) == 0) {
            idxOf_string = idx;
            p += 6;
        }
        else if (strncmp(p, "acc", 3) == 0) {
            idxOf_acc = idx;
            p += 3;
        }
        else if (strncmp(p, "pwd", 3) == 0) {
            idxOf_pwd = idx;
            p += 3;
        }
        else if (strncmp(p, "createTime", 10) == 0) {
            idxOf_createTime = idx;
            p += 10;
        }
        else if (strncmp(p, "updateTime", 10) == 0) {
            idxOf_updateTime = idx;
            p += 10;
        }
        else {
            p++;
        }
    }
    *pHeaderEnd = lastChar; //恢复临时修改的字符
    while (*p == '\r' || *p == '\n' || *p == '\0') {
        p++;
    }
    // 读取数据区域，一行一组数据，内容以上方扫描的idxOf为准，以,分隔item,以\r\n分隔行（可能只有\n）
    TinyCsvWebUIData* data = NULL;
    TinyCsvWebUIData* pLast = NULL;
    int line = 0;
    while (*p != '\0') {
        line++;
        TinyCsvWebUIData* pNew = new_TinyCsvWebUIData();
        if (data == NULL) {
            data = pNew;
        }
        else {
            Concat_TinyCsvWebUIData(pLast, pNew);
        }
        pLast = pNew;
        // 读取一行
        char* pLine = p;
        while (*p != '\r' && *p != '\n' && *p != '\0') {
            p++;
        }
        lastChar = *p; //记录临时修改的字符
        *p = '\0';
        // 读取一行
        char* pLineEnd = p;
        p = pLine;
        idx = 0;
        char lastChar2 = '\0';
        while (p < pLineEnd) {
            if (*p == ',') {
                idx++;
                p++;
            }
            else if (idx == idxOf_uuid) {
                //拷贝到end或者到,为止
                char* pEnd = p;
                while (*pEnd != ',' && *pEnd != '\0' && *pEnd != '\r' && *pEnd != '\n') {
                    pEnd++;
                }
                lastChar2 = *pEnd; //记录临时修改的字符
                *pEnd = '\0';
                pNew->uuid = (char *)malloc(strlen(p) + 1);
                strcpy(pNew->uuid, p);
                *pEnd = lastChar2; //恢复临时修改的字符
                p = pEnd;
            }
            else if (idx == idxOf_type) {
                if (strncmp(p, "file", 4) == 0) {
                    pNew->type = TINY_CSV_TYPE_FILE;
                }
                else if (strncmp(p, "string", 6) == 0) {
                    pNew->type = TINY_CSV_TYPE_STRING;
                }
                else if (strncmp(p, "accpwd", 6) == 0) {
                    pNew->type = TINY_CSV_TYPE_ACCPWD;
                }
                else {
                    pNew->type = TINY_CSV_TYPE_UNKNOWN;
                }
                while (*p != ',' && *p != '\0' && *p != '\r' && *p != '\n') {
                    p++;
                }
            }
            else if (idx == idxOf_itemName || idx == idxOf_file || idx == idxOf_string || idx == idxOf_acc || idx ==
                     idxOf_pwd) {
                //拷贝到end或者到,为止
                char* pEnd = p;
                while (*pEnd != ',' && *pEnd != '\0' && *pEnd != '\r' && *pEnd != '\n') {
                    pEnd++;
                }
                lastChar2 = *pEnd; //记录临时修改的字符
                *pEnd = '\0';
                if (idx == idxOf_itemName) {
                    // char* dataTemp = p;
                    // pNew->itemName = dataTemp;
                    pNew->itemName = decHex(p, NULL);
                }
                else if (idx == idxOf_file) {
                    pNew->data.file = decHex(p, NULL);
                }
                else if (idx == idxOf_string) {
                    pNew->data.string = decHex(p, NULL);
                }
                else if (idx == idxOf_acc) {
                    // pNew->data.accpwd.acc = p;
                    // char* dataTemp = p;
                    // pNew->data.accpwd.acc = dataTemp;
                    pNew->data.accpwd.acc = decHex(p, NULL);
                }
                else if (idx == idxOf_pwd) {
                    // pNew->data.accpwd.pwd=p;
                    // char* dataTemp = p;
                    // pNew->data.accpwd.pwd = dataTemp;
                    pNew->data.accpwd.pwd = decHex(p, NULL);
                }
                *pEnd = lastChar2; //恢复临时修改的字符
                p = pEnd;
            }
            else if (idx == idxOf_createTime || idx == idxOf_updateTime) {
                //拷贝到end或者到,为止
                char* pEnd = p;
                while (*pEnd != ',' && *pEnd != '\0' && *pEnd != '\r' && *pEnd != '\n') {
                    pEnd++;
                }
                lastChar2 = *pEnd; //记录临时修改的字符
                *pEnd = '\0';
                if (idx == idxOf_createTime) {
                    strcpy(pNew->createTime, p);
                }
                else if (idx == idxOf_updateTime) {
                    strcpy(pNew->updateTime, p);
                }
                *pEnd = lastChar2; //恢复临时修改的字符
                p = pEnd;
            }
            else {
                p++;
            }
        }
        *pLineEnd = lastChar; //恢复临时修改的字符
        p = pLineEnd;
        while (*p == '\r' || *p == '\n') {
            p++;
        }
    }
    return data;
}


#endif // TINYCSV_H
