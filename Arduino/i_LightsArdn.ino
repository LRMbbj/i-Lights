#include "ESP8266.h"
#include "SoftwareSerial.h"
#include <Servo.h>
#include <string.h>

#define yawPin 9
#define pitchPin 10
#define focusPin 11
#define lightPin 7
#define SSID "CHINA-Mobile"
#define password "01234567"

#define HOST_NAME "api.heclouds.com"  
#define DEVICE_ID "657528284"     
#define HOST_PORT (80)                
#define APIKey "IgWaniwBvM2II6A8fcco36Ero6U="; 

SoftwareSerial mySerial(5, 4);
ESP8266 wifi(mySerial);
//Servo yArm,pArm,fCtrl;


uint8_t buffer[400];
int len;
byte data[3]={0};

void servopulse(int servopin,int angle)//控制单个舵机
{
  for(int i=0;i<50;i++)  //发送50个脉冲   
  {
     int pulsewidth=(angle*11)+500;  //将角度转化为500-2480的脉宽值，每多转1度，对应高电平多11us
 
     digitalWrite(servopin,HIGH);    //将舵机接口电平至高
 
     delayMicroseconds(pulsewidth);  //延时脉宽值的微秒数
 
     digitalWrite(servopin,LOW);     //将舵机接口电平至低
 
     delayMicroseconds(20000-pulsewidth);   
  }
 
}

byte TrslData(char num[]) //将字符串转为数字
{
  byte res=0;
  for(int i=0;i<strlen(num);i++)
  {
    res=res*10+(num[i]-'0');
  }
  return res;
}

void MoveTo(byte Y,byte P,byte F) //联合控制3个舵机
{
  servopulse(yawPin,Y);
  servopulse(pitchPin,P);
  servopulse(focusPin,F);
}

void LightSetOn(bool state) //控制LED开关
{
  if(state) 
  {
    digitalWrite(lightPin,HIGH);
    Serial.print("Light On");
  }
  else
  {
    digitalWrite(lightPin,LOW);
    Serial.print("Light Off");
  }
}


void setup() {
  mySerial.begin(115200); //初始化软串口
  Serial.begin(9600);     //初始化串口

  
  Init:
  delay(1000);
  wifi.restart();
  
  Serial.print("setup begin\r\n");

  //以下为ESP8266初始化的代码
  Serial.print("FW Version: ");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStation()) {
    Serial.print("to station ok\r\n");
  } else {
    Serial.print("to station err\r\n");
    //goto Init;
  }
  APconn:
  //ESP8266接入WIFI
  if (wifi.joinAP(SSID, password)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
    //goto APconn;
  }

  mySerial.println("AT+UART_CUR=9600,8,1,0,0");
  mySerial.begin(9600);
  Serial.println("setup end\r\n");
  
  Serial.println("Init Done!");
}

void loop() {

    //从onenet读取偏转角数据

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
      Serial.print("connect ok\r\n");
      
      //拼接http GET请求
      String postString = "GET /devices/";
      postString += DEVICE_ID;
      postString += "/datastreams/yaw HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
      postString += "\r\n";
      postString += "\r\n";
      postString += "\r\n";

      const char *postArray = postString.c_str(); //将str转化为char数组

      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray)); //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
      Serial.println("send success");
      
      len=wifi.recv(buffer,sizeof(buffer),20000);

      //截取数据
      for(int i=0;i<int(len);i++){
        if((char)buffer[i]=='y' && (char)buffer[i+1]=='a' && (char)buffer[i+2]=='w')
        {
          int j=38,k=0;
          char num[3]={0};
          while(j<=40 &&  (char)buffer[i+j] != '}' )
          {
            num[k]=(char)buffer[i+j];
            j++;
            k++;
          }
          data[0]=TrslData(num);//转换数据
        }
      }
      
      delay(100);
      if (wifi.releaseTCP()) { //释放TCP连接
        Serial.print("\r\n\r\nrelease tcp ok\r\n");
      } else {
        Serial.print("\r\n\r\nrelease tcp err\r\n");
      }
      postArray = NULL; //清空数组，等待下次传输数据

      
      
    } else {
      Serial.print("create tcp err\r\n");
    }

    Serial.println("");


    //从onenet读取俯仰角数据

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
      Serial.print("connect ok\r\n");

      //拼接POST请求字符串
      String postString = "GET /devices/";
      postString += DEVICE_ID;
      postString += "/datastreams/pitch HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
      postString += "\r\n";
      postString += "\r\n";
      postString += "\r\n";

      const char *postArray = postString.c_str(); //将str转化为char数组

      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray)); //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
      Serial.println("send success");
      
      len=wifi.recv(buffer,sizeof(buffer),20000);

      for(int i=0;i<int(len);i++){
        
        //Serial.print((char)buffer[i]);
        
        if((char)buffer[i]=='p' && (char)buffer[i+1]=='i' && (char)buffer[i+2]=='t' )
        {
          int j=40,k=0;
          char num[3]={0};
          while(j<=45 &&  (char)buffer[i+j] != '}' )
          {
            num[k]=(char)buffer[i+j];
            j++;
            k++;
          }
          data[1]=TrslData(num);
        }
      }
      
      if (wifi.releaseTCP()) { //释放TCP连接
        Serial.print("\r\n\r\nrelease tcp ok\r\n");
      } else {
        Serial.print("\r\n\r\nrelease tcp err\r\n");
      }
      postArray = NULL; //清空数组，等待下次传输数据

      
      
    } else {
      Serial.print("create tcp err\r\n");
    }

    Serial.println("");


    //从onenet读取光圈大小数据

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
      Serial.print("connect ok\r\n");

      //拼接POST请求字符串
      String postString = "GET /devices/";
      postString += DEVICE_ID;
      postString += "/datastreams/focus HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
      postString += "\r\n";
      postString += "\r\n";
      postString += "\r\n";

      const char *postArray = postString.c_str(); //将str转化为char数组

      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray)); //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
      Serial.println("send success");
      
      len=wifi.recv(buffer,sizeof(buffer),20000);

      for(int i=0;i<int(len);i++){

        if((char)buffer[i]=='f' && (char)buffer[i+1]=='o' && (char)buffer[i+2]=='c' )
        {
          int j=40,k=0;
          char num[3]={0};
          while(j<=45 &&  (char)buffer[i+j] != '}' )
          {
            num[k]=(char)buffer[i+j];
            j++;
            k++;
          }
          data[2]=TrslData(num);
        }
      }
      
      if (wifi.releaseTCP()) { //释放TCP连接
        Serial.print("\r\n\r\nrelease tcp ok\r\n");
      } else {
        Serial.print("\r\n\r\nrelease tcp err\r\n");
      }
      postArray = NULL; //清空数组，等待下次传输数据

      
      
    } else {
      Serial.print("create tcp err\r\n");
    }

    //输出控制参数以供检查
    Serial.println("");

    Serial.println("data:");
    Serial.print("yaw:");
    Serial.println(data[0]);
    Serial.print("pitch:");
    Serial.println(data[1]);
    Serial.print("focus:");
    Serial.println(data[2]);

    //控制舵机动作
    MoveTo(data[0],data[1],data[2]);

    
    //从onenet读取LED状态数据
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
      Serial.print("connect ok\r\n");

      String postString = "GET /devices/";
      postString += DEVICE_ID;
      postString += "/datastreams/switch HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
      postString += "\r\n";
      postString += "\r\n";
      postString += "\r\n";

      const char *postArray = postString.c_str(); //将str转化为char数组

      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray)); //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
      Serial.println("send success");
      
      len=wifi.recv(buffer,sizeof(buffer),20000);
      
      for(int i=0;i<int(len);i++){
        if((char)buffer[i]=='s' && (char)buffer[i+1]=='w' && (char)buffer[i+2]=='i' && (char)buffer[i+3]=='t' && (char)buffer[i+4]=='c' && (char)buffer[i+5]=='h')
        {
          if((char)buffer[i+41]=='t') LightSetOn(true);//开灯
          if((char)buffer[i+41]=='f') LightSetOn(false);//关灯
        }
      }
      
      if (wifi.releaseTCP()) { //释放TCP连接
        Serial.print("\r\n\r\nrelease tcp ok\r\n");
      } else {
        Serial.print("\r\n\r\nrelease tcp err\r\n");
      }
      postArray = NULL; //清空数组，等待下次传输数据

      
      
    } else {
      Serial.print("create tcp err\r\n");
    }

    Serial.println("");
}
