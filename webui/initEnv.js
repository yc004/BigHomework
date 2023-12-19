/**
 * @file: initEnv.js
 * @description:
 * 向http://localhost:18080/server?地址进行POST请求交互数据
 * 接口有固定参数api和可选参数token以及自定义参数
 * api和token是原文传输，自定义参数是16进制字符串
 * localhost:127.0.0.1
 */
const serverUrl = "http://localhost:18080/server?";
let isGBKBakend = true; //是否是GBK编码的后端服务器（Win不设置那就是GBK）
var userToken = ""; //用户唯一标识（类似Cookie）
//先检查是否有userToken，如果没有则生成一个
if (localStorage.getItem("userToken") == null) {
    localStorage.setItem("userToken", userToken);
} else {
    userToken = localStorage.getItem("userToken");
}
//检查是否是GBK编码的后端服务器
var Cxhr = new XMLHttpRequest();
Cxhr.open("POST", serverUrl, true);
Cxhr.onreadystatechange = function () {
    if (Cxhr.readyState == 4 && Cxhr.status == 200) {
        if (Cxhr.responseText != "GBK") {
            isGBKBakend = false;
        }
    }
}
Cxhr.send("api=checkCharset");

/**
 * 把包括二进制情况的字符串转换为16进制字符串
 * @param {String} str
 * @return {String} 16进制字符串
 */
function encHex(str) {
    if (str == null) return str;
    var val = "";
    var encoder = null;
    if (isGBKBakend) {
        encoder = new TextEncoder("GBK");
    } else {
        encoder = new TextEncoder("UTF-8");
    }
    var bytes = encoder.encode(str);
    for (var i = 0; i < bytes.length; i++) {
        val += bytes[i].toString(16);
    }
    return val;
}

/**
 * 把16进制字符串转换为原字符串
 * @param {String} hex
 * @return {String} 原字符串
 */
function decHex(hex) {
    if (hex == null) return hex;
    var decoder = null;
    if (isGBKBakend) {
        decoder = new TextDecoder("GBK");
    } else {
        decoder = new TextDecoder("UTF-8");
    }
    var bytes = new Uint8Array(hex.length / 2);
    for (var i = 0; i < hex.length; i += 2) {
        bytes[i / 2] = parseInt(hex.substr(i, 2), 16);
    }
    val = decoder.decode(bytes);
    return val;
}

/**
 * 向服务器发送登录请求
 * @param {String} username
 * @param {String} password
 * @param {Function} callback
 *
 * @Post {String} api: login
 * @Post {String:Hex} acc: username
 * @Post {String:Hex} pwd: password
 *
 * @Note: callback的参数: (ok, msg)
 * @Note: token即认证令牌，如果发生任意一种登录失败的情况，token将为空。
 */
function login(username, password, callback) {
    var api = "login";
    if (typeof callback != "function") {
        alert("login函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    if (username == null || username == "") {
        callback(false, "用户名不能为空");
        return;
    }
    if (username.length > 20) {
        callback(false, "用户名不能超过20个字符");
        return;
    }
    if (password == null || password == "") {
        callback(false, "密码不能为空");
        return;
    }
    if (password.length > 20) {
        callback(false, "密码不能超过20个字符");
        return;
    }
    var data = "api=" + api + "&acc=" + encHex(username) + "&pwd=" + encHex(password);
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                if (xhr.responseText == "" || xhr.responseText == null) {
                    callback(false, "账号或密码错误");
                    return;
                }
                userToken = xhr.responseText;
                localStorage.setItem("userToken", userToken);
                callback(true, userToken);
            } else {
                callback(false, "网络错误");
            }
        }
    }
    xhr.send(data);
}

/**
 * 向服务器发送注册请求
 * @param {String} username
 * @param {String} password
 * @param {Function} callback
 *
 * @Post {String} api: register
 * @Post {String:Hex} acc: username
 * @Post {String:Hex} pwd: password
 *
 * @Note: callback的参数: (ok, msg)
 * @Note: code即注册结果，0为成功，1为失败，2为账号已存在。
 */
function register(username, password, callback) {
    var api = "register";
    if (typeof callback != "function") {
        alert("register函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    if (username == null || username == "") {
        callback(false, "用户名不能为空");
        return;
    }
    if (username.length > 20) {
        callback(false, "用户名不能超过20个字符");
        return;
    }
    if (password == null || password == "") {
        callback(false, "密码不能为空");
        return;
    }
    if (password.length > 20) {
        callback(false, "密码不能超过20个字符");
        return;
    }
    var data = "api=" + api + "&acc=" + encHex(username) + "&pwd=" + encHex(password);
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                if (xhr.responseText == "" || xhr.responseText == null) {
                    callback(false, "注册失败");
                    return;
                }
                var code = parseInt(xhr.responseText);
                if (code == 0) {
                    callback(true, "注册成功");
                } else if (code == 1) {
                    callback(false, "注册失败");
                } else if (code == 2) {
                    callback(false, "账号已存在");
                } else {
                    callback(false, "未知错误");
                }
            } else {
                callback(false, "网络错误");
            }
        }
    }
    xhr.send(data);
}

/**
 * 向服务器发送检查令牌请求
 * @param {Function} callback
 *
 * @Post {String} api: checkToken
 * @Post {String} token: token
 *
 * @Note: callback的参数: (ok, msg)
 * @Note: code即验证结果，0为成功，1为失败。
 */
function checkToken(callback) {
    var api = "checkToken";
    if (typeof callback != "function") {
        alert("checkToken函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    var data = "api=" + api + "&token=" + userToken;
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                if (xhr.responseText == "" || xhr.responseText == null) {
                    callback(false, "验证失败");
                    return;
                }
                var code = parseInt(xhr.responseText);
                if (code == 0) {
                    callback(true, "验证成功");
                } else if (code == 1) {
                    callback(false, "验证失败");
                } else {
                    callback(false, "未知错误");
                }
            } else {
                callback(false, "网络错误");
            }
        }
    }
    xhr.send(data);
}

/**
 * 向服务器发送退出登录请求
 * @param {Function} callback
 *
 * @Post {String} api: quitToken
 * @Post {String} token: token
 */
function quitToken(callback) {
    var api = "quitToken";
    if (typeof callback != "function") {
        alert("quitToken函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    var data = "api=" + api + "&token=" + userToken;
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            userToken = "";
            localStorage.setItem("userToken", userToken);
            callback();
        }
    }
    xhr.send(data);
}

// ============================== TinyCsv对应的处理库 - START ==============================
function parseTinyCsv(csvStr) {
    var mtable = [];
    var idxOf_uuid = 0;
    var idxOf_type = idxOf_uuid + 1;
    var idxOf_itemName = idxOf_type + 1;
    var idxOf_file = idxOf_itemName + 1;
    var idxOf_string = idxOf_file + 1;
    var idxOf_acc = idxOf_string + 1;
    var idxOf_pwd = idxOf_acc + 1;
    var idxOf_createTime = idxOf_pwd + 1;
    var idxOf_updateTime = idxOf_createTime + 1;
    // 扫描表头，重新定位idxOf_，换行\r或者\r\n或者\n
    var lines = csvStr.split(/[\r\n]+/);
    var headers = lines[0].split(",");
    for (var i = 0; i < headers.length; i++) {
        if (headers[i] == "uuid") {
            idxOf_uuid = i;
        } else if (headers[i] == "type") {
            idxOf_type = i;
        } else if (headers[i] == "itemName") {
            idxOf_itemName = i;
        } else if (headers[i] == "file") {
            idxOf_file = i;
        } else if (headers[i] == "string") {
            idxOf_string = i;
        } else if (headers[i] == "acc") {
            idxOf_acc = i;
        } else if (headers[i] == "pwd") {
            idxOf_pwd = i;
        } else if (headers[i] == "createTime") {
            idxOf_createTime = i;
        } else if (headers[i] == "updateTime") {
            idxOf_updateTime = i;
        }
    }
    // 扫描表格换行\r或者\r\n或者\n
    for (var i = 1; i < lines.length; i++) {
        //跳过空行
        if (lines[i] == "") {
            continue;
        }
        var line = lines[i];
        var cells = line.split(",");
        var uuid = cells[idxOf_uuid];
        var type = cells[idxOf_type];
        var itemName = cells[idxOf_itemName];
        var file = cells[idxOf_file];
        var string = cells[idxOf_string];
        var acc = cells[idxOf_acc];
        var pwd = cells[idxOf_pwd];
        var createTime = cells[idxOf_createTime];
        var updateTime = cells[idxOf_updateTime];
        var mrow = {
            uuid: uuid,
            type: type,
            itemName: decHex(itemName),
            file: decHex(file),
            string: decHex(string),
            acc: decHex(acc),
            pwd: decHex(pwd),
            createTime: createTime,
            updateTime: updateTime
        };
        mtable.push(mrow);
    }
    // console.log(mtable);
    return mtable;
}

function Csv2str(csvTable) {
    var headers = "uuid,type,itemName,file,string,acc,pwd,createTime,updateTime";
    var csvStr = headers + "\r\n";
    for (var i = 0; i < csvTable.length; i++) {
        var row = csvTable[i];
        var uuid = row.uuid;
        var type = row.type;
        var itemName = encHex(row.itemName);
        var file = encHex(row.file);
        var string = encHex(row.string);
        var acc = encHex(row.acc);
        var pwd = encHex(row.pwd);
        var createTime = row.createTime;
        var updateTime = row.updateTime;
        var line = uuid + "," + type + "," + itemName + "," +
            encHex(file) + "," + encHex(string) + "," + encHex(acc) + "," + encHex(pwd) + "," +
            createTime + "," + updateTime;
        csvStr += line + "\r\n";
    }
    return csvStr;
}


// ============================== TinyCsv对应的处理库 - END ==============================

/**
 * 向服务器发送获取数据列表请求
 * @param {String} sortType
 * @param {String} orderType
 * @param {String} queryType
 * @param {String} search
 * @param {Function} callback
 *
 * @Post {String} api: getDataList
 * @Post {String} token: token
 * @Post {String} sortType: sortType
 * @Post {String} queryType: queryType
 * @Post {String} search: searchContent
 *
 * @Note: callback的参数: (ok, csv)
 */
function getDataList(sortType, orderType, queryType, search, callback) {
    var api = "getDataList";
    if (typeof callback != "function") {
        alert("getDataList函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    if (typeof sortType == "boolean") { //如果sort是布尔值，则转换为字符串，true转换为升序，false转换为降序
        sortType = sortType ? "asc" : "desc";
    }
    if (sortType == "升序") {
        sortType = "asc";
    } else if (sortType == "降序") {
        sortType = "desc";
    }
    if (typeof orderType == "undefined") {
        orderType = "uuid";
    }
    if (orderType == "UUID排序") {
        orderType = "uuid";
    } else if (orderType == "创建时间排序") {
        orderType = "createTime";
    } else if (orderType == "更新时间排序") {
        orderType = "updateTime";
    } else if (orderType == "标题名排序") {
        orderType = "itemName";
    }
    if (typeof queryType == "undefined") {
        queryType = "*";
    }
    if (queryType.indexOf("文件") != -1) {
        queryType = queryType.replace("文件", "file");
    }
    if (queryType.indexOf("字符串") != -1) {
        queryType = queryType.replace("字符串", "string");
    }
    if (queryType.indexOf("文字") != -1) {
        queryType = queryType.replace("文字", "string");
    }
    if (queryType.indexOf("账号密码") != -1) {
        queryType = queryType.replace("账号密码", "accpwd");
    }
    if (typeof search == "undefined") {
        search = "";
    }
    var data = "api=" + api + "&token=" + userToken + "&sort=" + sortType +
        "&order=" + orderType + "&query=" + queryType + "&search=" + encHex(search);
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                var csv = xhr.responseText;
                callback(true, parseTinyCsv(csv));
            } else {
                callback(false, "网络错误");
            }
        }
    }
    xhr.send(data);
}

/**
 * 向服务器发送申请获取指定uuid的数据请求
 * @param {String} uuid
 * @param {Function} callback
 *
 * @Post {String} api: getDataList
 * @Post {String} token: token
 * @Post {String} uuid: uuid
 *
 * @Note: callback的参数: (ok, csv)
 */
function getDataByUUID(uuid, callback) {
    var api = "getDataByUUID";
    if (typeof callback != "function") {
        alert("getDataByUUID函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    var data = "api=" + api + "&token=" + userToken + "&uuid=" + uuid;
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                var csv = xhr.responseText;
                csv = parseTinyCsv(csv);
                callback(true, csv ? csv[0] : []);
            } else {
                callback(false, "网络错误");
            }
        }
    }
    xhr.send(data);
}

/**
 * 向服务器发送申请修改数据请求
 * @param {String} uuid
 * @param {Json} csv
 * @param {Function} callback
 *
 * @Post {String} api: editItem
 * @Post {String} token: token
 * @Post {String} uuid: uuid
 * @Post {String} itemName: itemName
 * @Post {String} string: string
 * @Post {String} acc: acc
 * @Post {String} pwd: pwd
 *
 * @Note: callback的参数: (ok, msg)
 * @Note: 0为成功，1为失败，2为uuid不存在。
 */
function editItem(uuid, csv, callback) {
    var api = "editItem";
    if (typeof callback != "function") {
        alert("editItem函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    if (typeof uuid == "undefined" || uuid == null) {
        uuid = DATA.uuid;
        if (typeof uuid == "undefined") {
            callback(false, "uuid参数不能为空");
            return;
        }
    }
    if (typeof csv == "undefined") {
        callback(false, "csv参数不能为空");
        return;
    }
    if (typeof csv.itemName == "undefined" || csv.itemName == "") {
        callback(false, "项目名不能为空");
        return;
    }
    if (DATA.type == "string" && (typeof csv.string == "undefined" || csv.string == "")) {
        callback(false, "字符串不能为空");
        return;
    }
    if (DATA.type == "accpwd" && (typeof csv.acc == "undefined" || csv.acc == "")) {
        callback(false, "账号不能为空");
        return;
    }
    if (DATA.type == "accpwd" && (typeof csv.pwd == "undefined" || csv.pwd == "")) {
        callback(false, "密码不能为空");
        return;
    }
    var data = "api=" + api + "&token=" + userToken + "&uuid=" + uuid;
    for (var key in csv) {
        data += "&" + key + "=" + encHex(csv[key]);
    }
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                var msg = parseInt(xhr.responseText);
                if (msg == 0) {
                    callback(true, "修改成功");
                } else if (msg == 1) {
                    callback(false, "修改失败");
                } else if (msg == 2) {
                    callback(false, "uuid不存在");
                } else {
                    callback(false, "未知错误");
                }
            } else {
                callback(false, "网络错误");
            }
        }
    }
    xhr.send(data);
}

/**
 * 向服务器发送删除指定uuid的数据请求
 * @param {String} uuid
 * @param {Function} callback
 *
 * @Post {String} api: deleteItem
 * @Post {String} token: token
 * @Post {String} uuid: uuid
 *
 * @Note: callback的参数: (ok, msg)
 * @Note: 0为成功，1为失败，2为uuid不存在。
 */
function deleteItem(uuid, callback) {
    var api = "deleteItem";
    if (typeof callback != "function") {
        alert("deleteItem函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    var data = "api=" + api + "&token=" + userToken + "&uuid=" + uuid;
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                var msg = parseInt(xhr.responseText);
                if (msg == 0) {
                    callback(true, "删除成功");
                } else if (msg == 1) {
                    callback(false, "删除失败");
                } else if (msg == 2) {
                    callback(false, "uuid不存在");
                } else {
                    callback(false, "未知错误");
                }
            } else {
                callback(false, "网络错误");
            }
        }
    }
    xhr.send(data);
}

/**
 * 向服务器发送还原文件请求
 * @param {String} uuid
 * @param {Function} callback
 *
 * @Post {String} api: decryptFile
 * @Post {String} token: token
 * @Post {String} uuid: uuid
 *
 * @Note: callback的参数: (ok, msg)
 * @Note: 0为成功，1为失败，2为uuid不存在。
 */
function decryptFile(uuid, callback) {
    var api = "decryptFile";
    if (typeof callback != "function") {
        alert("decryptFile函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    var data = "api=" + api + "&token=" + userToken + "&uuid=" + uuid;
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                var msg = parseInt(xhr.responseText);
                if (msg == 0) {
                    callback(true, "还原成功");
                } else if (msg == 1) {
                    callback(false, "还原失败");
                } else if (msg == 2) {
                    callback(false, "uuid不存在");
                } else {
                    callback(false, "未知错误");
                }
            } else {
                callback(false, "网络错误");
            }
        }
    }
    xhr.send(data);
}


// ### 10. 申请添加数据
// > 提交（token鉴权）
// - `api` : `addItem`
// - `token` : `token`
// - `type` : `type`(file文件，string字符串，accpwd账号密码)
// - `itemName` : `itemName`(HEX编码)
// - `string` : `string`(HEX编码，根据类型可选)
// - `acc` : `acc`(HEX编码，根据类型可选)
// - `pwd` : `pwd`(HEX编码，根据类型可选)
// - `file` : `file`(HEX编码，根据类型可选)
// > 返回
// - code

//         code即添加结果，0为成功，1为失败。


/**
 * 向服务器发送申请添加数据请求
 * @param {Json} csv
 * @param {Function} callback
 *
 * @Post {String} api: addItem
 * @Post {String} token: token
 * @Post {String} type: type
 * @Post {String} itemName: itemName
 * @Post {String} string: string
 * @Post {String} acc: acc
 * @Post {String} pwd: pwd
 * @Post {String} file: file
 *
 * @Note: callback的参数: (ok, msg)
 * @Note: 0为成功，1为失败。
 */
function addItem(csv, callback) {
    var api = "addItem";
    if (typeof callback != "function") {
        alert("addItem函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    if (typeof csv.type == "undefined" || csv.type == "") {
        alert("addItem函数的csv参数必须包含type属性，请检查JS代码。");
        return;
    }
    if (typeof csv.itemName == "undefined" || csv.itemName == "") {
        callback(false, "名称不能为空");
        return;
    }
    if (csv.type == "string" && (typeof csv.string == "undefined" || csv.string == "")) {
        callback(false, "字符串不能为空");
        return;
    }
    if (csv.type == "accpwd" && (typeof csv.acc == "undefined" || csv.acc == "")) {
        callback(false, "账号不能为空");
        return;
    }
    if (csv.type == "accpwd" && (typeof csv.pwd == "undefined" || csv.pwd == "")) {
        callback(false, "密码不能为空");
        return;
    }
    var data = "api=" + api + "&token=" + userToken + "&type=" + csv.type;
    for (var key in csv) {
        if (key != "type" && key != "uuid" && key != "createTime" && key != "updateTime") {
            data += "&" + key + "=" + encHex(csv[key]);
        }
    }
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                var msg = parseInt(xhr.responseText);
                if (msg == 0) {
                    callback(true, "添加成功");
                } else if (msg == 1) {
                    callback(false, "添加失败");
                } else {
                    callback(false, "未知错误");
                }
            } else {
                callback(false, "网络错误");
            }
        }
    }
    xhr.send(data);
}

/**
 * 向服务器发送获取学号ID请求（用于网页显示）
 * @param {Function} callback
 *
 * @Post {String} api: getStudentId
 */
function getStudentId(callback) {
    var api = "getStudentId";
    if (typeof callback != "function") {
        alert("getStudentId函数的callback参数必须是函数，请检查JS代码。");
        return;
    }
    var data = "api=" + api;
    var xhr = new XMLHttpRequest();
    xhr.open("POST", serverUrl, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                var msg = xhr.responseText;
                callback(true, msg);
            } else {
                callback(false, "网络错误");
            }
        }
    }
    xhr.send(data);
}