# CommonUtil开发文档

___注意！为了可以较直观的查看本文档，请使用支持MarkDown的阅读器查看，如果你正在用VSCode看到本内容，请按Ctrl+Shift+V即可查看预览。
___

## 一、基本说明
---

- 这是一个函数库，提供其他文件使用的一些基本函数。
- 如果是因为这个库导致编译失败可以删除报错的而没用到的函数。

## 二、具体API
---

### 1. HEX编码（16进制编码）

```c
static inline char* encHex(char* data, int size);
```

> `size`为`0`时，会使用`strlen`函数计算`data`的长度，返回的字符串需要手动释放。

### 2. HEX解码（16进制解码）

```c
static inline char* decHex(char* hex, int* size);
```

> `size`为返回内容的长度，可以为`NULL`，返回的字符串需要手动释放。

### 3. 格式化时间日期

```c
static inline char* fmtYmdHMS(char* fmt, long long timestamp);
```

> `fmt`为`NULL`时使用默认值`%Y-%m-%d %H:%M:%S`，`timestamp`使用Unix时间戳（省略毫秒）表示，为`0`时使用当前时间，返回的字符串需要手动释放。

### 4. 获取当前Unix时间戳（省略毫秒）

```c
static inline long long getTimestamp();
```

### 5. 根据输入的时间字符串获取时间戳（省略毫秒）

```c
static inline long long getTimestampByStr(char* str);
```

> 匹配时间字符串：`%Y-%m-%d %H:%M:%S`。

### 6. Base64编码

```c
static inline char* encBase64(char* data, int size);
```

> `size`为`0`时，会使用`strlen`函数计算`data`的长度，返回的字符串需要手动释放。

### 7. Base64解码

```c
static inline char* decBase64(char* base64, int* size);
```

> `size`为返回内容的长度，可以为`NULL`，返回的字符串需要手动释放。

### 8. CRC32校验

```c
static inline unsigned int crc32(char* data, int size);
```

> `size`为`0`时，会使用`strlen`函数计算`data`的长度。注意，这是哈希函数，不是加密函数。

### 9. RC4加密

```c
static inline char* Rc4Core(char* data, int size, char* key, int keySize);
#define encRc4(...) Rc4Core(__VA_ARGS__)
#define decRc4(...) Rc4Core(__VA_ARGS__)
```

> `size`为`0`时，会使用`strlen`函数计算`data`的长度，`keySize`为`0`时，会使用`strlen`函数计算`key`
> 的长度，返回的字符串需要手动释放。RC4是一种对称加密算法，加密和解密使用同一函数，所以提供`Rc4Core`函数，`encRc4`和`decRc4`
> 是`Rc4Core`的宏定义，使用方法一致。

### 10. 文件读取

```c
static inline char* readFile(char* path, int* size);
```

> `size`为返回内容的长度，可以为`NULL`，返回的字符串需要手动释放。

### 11. 文件写入

```c
static inline int writeFile(char* path, char* data, int size);
```

> `size`为`0`时，会使用`strlen`函数计算`data`的长度。

### 12. 文件追加

```c
static inline int appendFile(char* path, char* data, int size);
```

> `size`为`0`时，会使用`strlen`函数计算`data`的长度。

### 13. 获取文件大小

```c
static inline int getFileSize(char* path);
```

### 14. 文件是否存在

```c
static inline int fileExists(char* path);
```

### 15. 目录是否存在

```c
static inline int dirExists(char* path);
```

> 理论支持Windows/Mac/Linux，但是只在Windows上测试过对应API，其他平台只是根据资料写的，没经过测试。

### 16. 删除文件或目录

```c
static inline int deleteFile(char* path);
```

> 理论支持Windows/Mac/Linux，但是只在Windows上测试过对应API，其他平台只是根据资料写的，没经过测试。

### 17. 创建目录

```c
static inline int createDir(char* path);
```

> 理论支持Windows/Mac/Linux，但是只在Windows上测试过对应API，其他平台只是根据资料写的，没经过测试。

### 18. 获取目录下所有文件（和目录）

```c
static inline char** getFiles(char* path, int* count);
```

> `count`为返回的文件数量，可以为`NULL`（数组以`NULL`
> 结尾），返回的字符串和字符串数组需要手动释放。理论支持Windows/Mac/Linux，但是只在Windows上测试过对应API，其他平台只是根据资料写的，没经过测试。
