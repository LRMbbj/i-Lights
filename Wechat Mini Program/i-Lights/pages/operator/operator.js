// pages/operator/operator.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    length:600,
    width:400,
    x1:0,
    y1:0,
    d1:0,
    x2:0,
    y2:0,
    d2:0,
    x:0,
    y:0,
    z:0,
    f:0,
    state:false,
    btntext:"开启",
    stylebtn:"primary",

    yaw1:0,
    pitch1:0,
    focus1:0
  },

  //计算不同位置灯模块的参数
  calcuAng(x,y){
    const PI = 3.1416;
    var data = this.data;
    var v1=[data.x-x,data.y-y,data.z-300]; //计算指向向量
    var disH = Math.sqrt(Math.pow(v1[0],2) + Math.pow(v1[1],2)),//水平距离
        dis = Math.sqrt(Math.pow(v1[0],2) + Math.pow(v1[1],2) + Math.pow(v1[2],2));//空间距离
    var yaw = (v1[1] > 0 ? Math.round(Math.acos(v1[0] / disH) / PI * 180) : Math.round(360 - Math.acos(v1[0] / disH) / PI * 180)) - data.d1, //计算水平偏转角
    pitch = Math.round(Math.asin(-v1[2] / dis)/PI*180); //计算俯仰角
    console.log(yaw,pitch);
    return [yaw,pitch];
  },
  
  //页面加载时将初始化页面输入的数据导入
  onLoad(){
    var app=getApp();
    this.setData(app.initInfo);
    console.log(this.data);
  },

  //点击画布更新数据并显示点击的位置
  start (e) {
    var len = this.data.length,width=this.data.width;
    
    //将画布位置换算成绝对坐标
    this.setData({
      x: Math.round(e.touches[0].x / 300 * len),
      y: Math.round((1-e.touches[0].y / 200) * width)
    });

    //绘图
    const ctx = wx.createCanvasContext('myCanvas');
    ctx.setFillStyle('red');
    ctx.fillRect(e.touches[0].x - 5,e.touches[0].y - 5,10,10);
    ctx.draw();
    console.log(this.data.x);
  },

  //更新目标位置z坐标
  zChange(e){
    this.setData({
      z:e.detail.value
    })
  },

  //更新光圈大小设置
  fChange(e){
    this.setData({
      f:e.detail.value
    })
  },

  //控制灯的开关
  switchLight(){
    if (this.data.state) {
      this.setData({
        state:false,//更新状态
        btntext:"开启",//更新按钮文字
        stylebtn:"primary"//更新按钮颜色
      });

    } else {
      this.setData({
        state:true,
        btntext:"熄灭",
        stylebtn:"warn"
      });
    }

    var updata =  {
      "datastreams": [{
        "id": "switch",
        "datapoints": [{
                "value": this.data.state
            }
        ]
      }] 
    }

    wx.request({
      url: 'http://api.heclouds.com/devices/657528284/datapoints',
      method: 'POST',
      header: { 
        "content-type": 'application/json',
        'api-key':'IgWaniwBvM2II6A8fcco36Ero6U='
      },
      data: JSON.stringify(updata)
      ,
      success: function (res) {
        //console.log(res);
        },
        //wx.showToast({ title: res.data.data[0] })
      
      fail: function (res) {
        wx.showToast({ title: '系统错误' })
      },
      complete: function (res) {
        //console.log(res);
      }
  })
  },

  //上传控制参数
  update(){
    var data = this.data;
    console.log(data);

    //显示提示信息
    var upinfo = "坐标(x,y,z) = (" + data.x + "," + data.y + "," + data.z + ")  光圈f:" + data.f + "%";
    
    //计算各种参数
    data.yaw1 = this.calcuAng(data.x1,data.y1)[0];
    data.pitch1 = 90 - this.calcuAng(data.x1,data.y1)[1];
    data.focus1 = data.f * 0.54 + 126;

    //设定上传参数
    var updata =  {
      "datastreams": [{
        "id": "yaw",
        "datapoints": [{
                "value": this.data.yaw1
            }
        ]
      },
      {
        "id": "pitch",
        "datapoints": [{
                "value": this.data.pitch1
            }
        ]
      },
      {
        "id": "focus",
        "datapoints": [{
                "value": this.data.focus1
            }
        ]
      }] 
    }

    //onenet数据上传函数
    wx.request({
      url: 'http://api.heclouds.com/devices/657528284/datapoints',
      method: 'POST',
      header: { 
        "content-type": 'application/json',
        'api-key':'IgWaniwBvM2II6A8fcco36Ero6U='
      },
      data: JSON.stringify(updata)
      ,
      success: function (res) {
        //console.log(res);
        },
        //wx.showToast({ title: res.data.data[0] })
      
      fail: function (res) {
        wx.showToast({ title: '系统错误' })
      },
      complete: function (res) {
        //console.log(res);
      }
    })

    //将数据显示在对话框中（方便调试）
    wx.showModal({
      title: "提示", // 提示的标题
      content: upinfo, // 提示的内容
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
})