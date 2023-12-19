// electron main.js
const {app, BrowserWindow} = require('electron')
const path = require('path')
const url = require('url')
// index.html
const index = url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
})
// create window
let win = null

function createWindow() {
    //最大化（非全屏）
    win = new BrowserWindow({width: 1400, height: 800})
    win.maximize()
    win.loadURL(index)
    win.on('closed', () => {
        win = null
    })

    // 取消菜单栏
    win.setMenu(null)
}

// app ready
app.on('ready', createWindow)
// app quit
app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit()
    }
})
app.on('activate', () => {
    if (win === null) {
        createWindow()
    }
})

//调整窗口位置到中间
function centerWindow() {
    const {
        screen: {
            width,
            height
        }
    } = require('electron')
    win.setPosition(Math.floor((width - win.getSize()[0]) / 2), Math.floor((height - win.getSize()[1]) / 2))
}