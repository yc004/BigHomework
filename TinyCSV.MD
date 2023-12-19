# TinyCsv开发文档

___注意！为了可以较直观的查看本文档，请使用支持MarkDown的阅读器查看，如果你正在用VSCode看到本内容，请按Ctrl+Shift+V即可查看预览。
___

## 一、应用场景说明
---

- 这并不是一个健全的CSV处理库，它只适用于这个WebUI传递列表信息数据时的序列化和反序列化，并进行了一些特殊调整。

## 二、预定义
---

- 类型定义

```c
typedef enum TinyCsvType {
    TINY_CSV_TYPE_FILE = (1 << 0),
    TINY_CSV_TYPE_STRING = (1 << 1),
    TINY_CSV_TYPE_ACCPWD = (1 << 2),
    TINY_CSV_TYPE_UNKNOWN = -1,
} TinyCsvType;
```

- 数据结构定义（链表）

```c
typedef struct TinyCsvWebUIData {
    char* uuid;
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
```

- 预定义宏

```c
#define TINY_CSV_TYPE_CONSISTENT_FILE(type) ((type&TINY_CSV_TYPE_FILE) == 1)
#define TINY_CSV_TYPE_CONSISTENT_STRING(type) ((type&TINY_CSV_TYPE_STRING) == 1)
#define TINY_CSV_TYPE_CONSISTENT_ACCPWD(type) ((type&TINY_CSV_TYPE_ACCPWD) == 1)
#define TINY_CSV_DATA_GET_FILE(data) ((data)->data.file)
#define TINY_CSV_DATA_GET_STRING(data) ((data)->data.string)
#define TINY_CSV_DATA_GET_ACC(data) ((data)->data.accpwd.acc)
#define TINY_CSV_DATA_GET_PWD(data) ((data)->data.accpwd.pwd)
#define TINY_CSV_DATA_GET_ITEMNAME(data) ((data)->itemName)
#define TINY_CSV_DATA_GET_TYPE(data) ((data)->type)
#define TINY_CSV_DATA_GET_UUID(data) ((data)->uuid)
#define TINY_CSV_DATA_GET_CREATETIME(data) ((data)->createTime)
#define TINY_CSV_DATA_GET_UPDATETIME(data) ((data)->updateTime)

#define CSV_HEADER "uuid,type,itemName,file,string,acc,pwd,createTime,updateTime"
```

> TINY_CSV_TYPE_CONSISTENT_系宏用于复合判断（比如筛选某几类时确定匹配某类）
>
> TINY_CSV_DATA_GET_系宏用于获取数据（但是请先进行类型判断）
>
> CSV_HEADER支持调整键顺序转储，直接调整这个宏即可，但是并不建议调整。

## 三、具体API
---

### 1. 释放单个数据

```c
static inline void freeTinyCsvWebUIData(TinyCsvWebUIData* data);
```

### 2. 释放从某位置开始的链表

```c
static inline void freeTinyCsvWebUIDataList(TinyCsvWebUIData* data);
```

### 3. 新建一个初始的数据

```c
static inline TinyCsvWebUIData* new_TinyCsvWebUIData();
```

### 4. 新建一个文件类型数据

```c
static inline TinyCsvWebUIData* new_TinyCsvWebUIData_FILE(char* uuid, char* itemName, char* file, char* createTime, char* updateTime);
```

### 5. 新建一个文本类型数据

```c
static inline TinyCsvWebUIData* new_TinyCsvWebUIData_STRING(char* uuid, char* itemName, char* string, char* createTime, char* updateTime);
```

### 6. 新建一个账号密码类型数据

```c
static inline TinyCsvWebUIData* new_TinyCsvWebUIData_ACCPWD(char* uuid, char* itemName, char* acc, char* pwd, char* createTime, char* updateTime);
```

### 7. 转储

```c
static inline char* TinyCsv_dump(TinyCsvWebUIData* data);
```

### 8. 加载转储

```c
static inline TinyCsvWebUIData* TinyCsv_load(char* csv);
```

> 注意！这个转储函数会对输入的数据进行修改而从而实现直接解析，不使用临时内存，所以输入的char指针指向的必须是可修改的数据。

