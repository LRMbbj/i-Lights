//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    length:0,
    width:0,
    x1:0,
    y1:0,
    d1:0,
    x2:0,
    y2:0,
    d2:0,
  },
  submitinit(e){
    
    var info=e.detail.value;

    //判断是否输入完全
    if (info.lengthinput != "" && info.widthinput != "" && info.x1input != "" &&  info.y1input != ""  && info.d1input != "" ) {
      this.setData({
        length:info.lengthinput,
        width:info.widthinput,
        x1:info.x1input,
        y1:info.y1input,
        d1:info.d1input,
        x2:info.x2input,
        y2:info.y2input,
        d2:info.d2input,
      });
      var app=getApp();
      app.initInfo = this.data;

      //页面跳转到控制页面
      wx.redirectTo({
        url: "../operator/operator"
      });
    } else {
      console.log(info);
      //扔一个警告
      wx.showModal({
        title: "警告", // 提示的标题
        content: "必须输入全部初始化信息", // 提示的内容
        showCancel: true, // 是否显示取消按钮，默认true
        cancelText: "取消", // 取消按钮的文字，最多4个字符
        cancelColor: "#000000", // 取消按钮的文字颜色，必须是16进制格式的颜色字符串
        confirmText: "确定", // 确认按钮的文字，最多4个字符
        confirmColor: "#576B95", // 确认按钮的文字颜色，必须是 16 进制格式的颜色字符串
        success: function (res) {
            if (res.confirm) {
                console.log('用户点击确定')
            } else if (res.cancel) {
                console.log('用户点击取消')
            }
        },
        fail: function () {
            console.log("接口调用失败的回调函数");
        },
        complete: function () {
            console.log("接口调用结束的回调函数（调用成功、失败都会执行）");
        }
      })
    }
  }
})
