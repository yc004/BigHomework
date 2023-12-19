var color_array = [
    "#FF0000", "#FF7F00", "#FFFF00", "#00FF00", "#0000FF", "#4B0082", "#8B00FF", "#FF1493",
    "#FA8072", "#FFA500", "#FFD700", "#90EE90", "#00FFFF", "#87CEFA", "#9370DB", "#FFC0CB"
];
var canvas = document.getElementById("canvas");
var ctx = canvas.getContext("2d");
var w = canvas.width = window.innerWidth;
var h = canvas.height = window.innerHeight;
var circles = [];
var maxTGenCircles = 100;
var maxCountCircles = 300;
var minRadius = 1;
var maxRadius = 2;
var minSpeed = 0.1;
var maxSpeed = 0.9;
var minAlpha = 0.5;
var makeOneTime = 30;
var maxSize = 30;
var maxGrow = 0.05;
//窗口大小改变时，重新设置canvas大小
window.onresize = function () {
    w = canvas.width = window.innerWidth;
    h = canvas.height = window.innerHeight;
}

function Circle() {
    this.x = Math.random() * w;
    this.y = Math.random() * h;
    this.r = Math.random() * (maxRadius - minRadius) + minRadius;
    this.color = color_array[Math.floor(Math.random() * color_array.length)];
    this.speedX = Math.random() * (maxSpeed - minSpeed) + minSpeed;
    if (Math.random() > 0.5) {
        this.speedX = -this.speedX;
    }
    this.speedY = Math.random() * (maxSpeed - minSpeed) + minSpeed;
    if (Math.random() > 0.5) {
        this.speedY = -this.speedY;
    }
    this.alpha = Math.random() * (1 - minAlpha) + minAlpha;
    this.isGrow = true;
}

Circle.prototype.draw = function () {
    ctx.beginPath();
    ctx.arc(this.x, this.y, this.r, 0, Math.PI * 2);
    ctx.fillStyle = this.color;
    ctx.globalAlpha = this.alpha;
    ctx.fill();
}
Circle.prototype.update = function () {
    //缩放，到达一定大小后变小再变大
    if (this.isGrow) {
        if (this.r < maxSize) {
            this.r += maxGrow;
        } else {
            this.isGrow = false;
        }
    } else {
        if (this.r > minRadius) {
            this.r -= maxGrow;
        } else {
            this.isGrow = true;
        }
    }
    //移动
    this.x += this.speedX;
    this.y += this.speedY;
    //碰撞检测
    if (this.x > w || this.x < 0) {
        this.speedX = -this.speedX;
    }
    if (this.y > h || this.y < 0) {
        this.speedY = -this.speedY;
    }
    this.draw();

}
for (var i = 0; i < maxTGenCircles; i++) {
    circles.push(new Circle());
}
var makeOne = 0;

function animate() {
    requestAnimationFrame(animate);
    ctx.clearRect(0, 0, w, h);
    for (var i = 0; i < circles.length; i++) {
        circles[i].update();
    }
    if (circles.length < maxCountCircles) {
        if (makeOne < makeOneTime) {
            makeOne++;
        } else {
            makeOne = 0;
            circles.push(new Circle());
        }
    }
}

animate();