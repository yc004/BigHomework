# WebUIApi开发文档

___注意！为了可以较直观的查看本文档，请使用支持MarkDown的阅读器查看，如果你正在用VSCode看到本内容，请按Ctrl+Shift+V即可查看预览。
___

## 一、支持状况
---

- Windows: 支持
- Linux: 尚不支持（无设备测试，无法开发）
- Mac: 尚不支持（无设备测试，无法开发）

## 二、基本API格式
---

- 提交地址：`http://localhost:18080/server?`或者`http://127.0.0.1:18080/server?`。
- 提交方法：POST
- 固定参数`api`，指定具体行为接口
- 可选固定参数`token`，由C后端生成的一串Hex内容，浏览器将缓存这个内容作为身份认证，在需要鉴权的接口时一并提交，以保证权限机制和用户机制正常运行，后端请留存处理使用。
- 其他参数几乎都需要Hex编码

## 三、具体API
---
> 注意！这里的API是WebUI的API，不是C后端的API，C后端的API参数几乎要求必须全齐，WebUI会自动补齐后发送给后端。

### 1. 登录

> 提交

- `api` : `login`
- `acc` : `账号`(HEX编码)
- `pwd` : `密码`(HEX编码)

> 返回

- token

        token即认证令牌，如果发生任意一种登录失败的情况，token将为空。

### 2. 注册

> 提交

- `api` : `register`
- `acc` : `账号`(HEX编码)
- `pwd` : `密码`(HEX编码)

> 返回

- code

        code即注册结果，0为成功，1为失败，2为账号已存在。

### 3. 验证token

> 提交

- `api` : `checkToken`
- `token` : `token`

> 返回

- code

        code即验证结果，0为成功，1为失败。

### 4. 退出token

> 提交

- `api` : `quitToken`
- `token` : `token`

### 5. 申请获取数据列表

> 提交（token鉴权）

- `api` : `getDataList`
- `token` : `token`
- `sort` : `sortType`(asc升序，desc降序，不填则默认升序)
- `order` : `orderType`(排序依据：uuid/itemName/createTime/updateTime，不填则默认按照uuid排序)
- `query` : `queryType`(
  file文件，string字符串，accpwd账号密码，允许使用|分隔多个类型（如：file|accpwd），不填则默认全部，特别的，*代表全部)
- `search` : `searchContent`(HEX编码，搜索内容，不填则默认空)

> 返回

- csv

        csv即数据列表，格式为csv，每行为一条数据，每行的每个字段用逗号分隔，存在表头：uuid,type,itemName,file,string,acc,pwd,createTime,updateTime
        其中，itemName/file/string/acc/pwd字段内容为HEX编码，需要解码后使用（因为CSV需要考虑转义等问题，这里简化要求，直接使用HEX编码内容解决需要转义的问题）；createTime/updateTime字段为时间，具体到年月日时分秒，格式为：yyyy-MM-dd HH:mm:ss；file是路径

> csv样例：

        uuid,type,itemName,file,string,acc,pwd,createTime,updateTime
        1,file,6974656d41,,312e747874,,,,2023-12-12 12:12:12,2023-12-12 12:12:12
        2,string,6974656d42,,68656c6c6f20776f726c64,,,2023-12-12 12:12:12,2023-12-12 12:12:12
        3,accpwd,6974656d43,,,61646d696e,3132333435363738,2023-12-12 12:12:12,2023-12-12 12:12:12

> 对应表格（解码后）：

| uuid | type   | itemName | file  | string      | acc   | pwd      | createTime          | updateTime          |
|------|--------|----------|-------|-------------|-------|----------|---------------------|---------------------|
| 1    | file   | itemA    | 1.txt |             |       |          | 2023-12-12 12:12:12 | 2023-12-12 12:12:12 |
| 2    | string | itemB    |       | hello world |       |          | 2023-12-12 12:12:12 | 2023-12-12 12:12:12 |
| 3    | accpwd | itemC    |       |             | admin | 12345678 | 2023-12-12 12:12:12 | 2023-12-12 12:12:12 |

> 可以使用提供的`tinycsv`进行处理，它并非正规的处理库，这个库是专门设计处理`api`是`getDataList`时的通信的CSV数据的，具体使用方法请阅读对应文档。

### 6. 申请获取指定uuid的数据

> 提交（token鉴权）

- `api` : `getDataByUUID`
- `token` : `token`
- `uuid` : `uuid`

> 返回

- csv

        和getDataList一样，只是返回的是单条数据

### 7. 申请修改指定数据

> 提交（token鉴权）

- `api` : `editItem`
- `token` : `token`
- `uuid` : `uuid`
- `itemName` : `itemName`(HEX编码)
- `string` : `string`(HEX编码，根据类型可选)
- `acc` : `acc`(HEX编码，根据类型可选)
- `pwd` : `pwd`(HEX编码，根据类型可选)

> 返回

- code

        code即修改结果，0为成功，1为失败，2为uuid不存在。 

### 8. 删除指定uuid的数据

> 提交（token鉴权）

- `api` : `deleteItem`
- `token` : `token`
- `uuid` : `uuid`

> 返回

- code

        code即删除结果，0为成功，1为失败，2为uuid不存在。

### 9. 还原文件

> 提交（token鉴权）

- `api` : `decryptFile`
- `token` : `token`
- `uuid` : `uuid`

> 返回

- code

        code即删除结果，0为成功，1为失败，2为uuid不存在。

### 10. 申请添加数据

> 提交（token鉴权）

- `api` : `addItem`
- `token` : `token`
- `type` : `type`(file文件，string字符串，accpwd账号密码)
- `itemName` : `itemName`(HEX编码)
- `string` : `string`(HEX编码，根据类型可选)
- `acc` : `acc`(HEX编码，根据类型可选)
- `pwd` : `pwd`(HEX编码，根据类型可选)
- `file` : `file`(HEX编码，根据类型可选)

> 返回

- code

        code即添加结果，0为成功，1为失败。

### 11. 获取后端编码

> 提交

- `api` : `checkCharset`

> 返回

- charset

        charset即后端编码，如：UTF-8、GBK等

### 12. 获取学号

> 提交

- `api` : `getStudentId`

> 返回

- idnum

        idnum是学号，如：1234567890