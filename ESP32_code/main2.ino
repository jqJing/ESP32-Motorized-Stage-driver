#include <TFT_eSPI.h>
#include <SPI.h>
#include <string.h>
#include "pikapika.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


//硬件IO口
#define LED 13
#define LED_PIN 33
#define LED_COUNT 30



//NEO RGB灯泡 
#define BRIGHTNESS 40

#define SPHERE_POS_X 150
#define SPHERE_Y 8
#define SPHERE_SIZE 6

#define ENA_PIN 19
#define DIR_PIN 20
#define PWM_PIN 13
#define UKN_PIN 22
#define CAM1_TRI 23
#define CAM2_TRI 24

//用于描述运动指令的标识符
#define TAG_X 1
#define TAG_Y 86

#define BAR_X 2
#define BAR_Y 110
#define BAR_WIDTH 140
#define BAR_HEIGHT 14
#define BAR_ROUND_RADIUS 5
#define BAR_CRT_PERCENT 0


//用于描述PWM核向GUI核发送指令的各个指令码
#define MSG_INIT 30
#define MSG_SWITCH 31
#define MSG_SWITCH_ON 310
#define MSG_SWITCH_OFF 311
#define MSG_SETTAG 32
#define MSG_STEP 33
#define MSG_TFTPRINT 34
#define MSG_WORK_MODE 35
#define MSG_WORK_MODE_HARD 351
#define MSG_WORK_MODE_SOFT 352


//用于Serial通讯的指令码
#define SERIAL_INIT 50
#define SERIAL_SWITCH 51
#define SERIAL_SWITCH_ON 510
#define SERIAL_SWITCH_OFF 511
#define SERIAL_SETTAG 52 //这个不用
#define SERIAL_STEP 53
#define SERIAL_TFTPRINT 54

#define SERIAL_SET_TRIGGER_MODE 55 //设置ESP32的触发模式 分为外触发与内触发
#define SERIAL_TRIGGER_MODE_OFF 551 //内触发模式，意味着ESP32自身进行控制与驱动，计算逻辑在ESP32上
#define SERIAL_TRIGGER_MODE_ON 552 //外触发模式，意味着ESP32只执行上位机的命令，自己不做决策

#define SERIAL_SIGN_UP 553
#define SERIAL_START_HARD 554 //触发命令，开始运行


#define SERIAL_SOFT_DIRECTION_ON 601
#define SERIAL_SOFT_DIRECTION_OFF 602
#define SERIAL_SOFT_MOV 61


//全局变量们
struct Coremsg {
  int key;
  char value[50];
  int dir;
  float stepsize;
  int totalstep;
  int state;
};
QueueHandle_t queue; //全局变量
TFT_eSPI tft = TFT_eSPI();
//-----
SemaphoreHandle_t xMutex = NULL;

//系统各个重要的参数
int direction = 0; //系统移动方向 0为左 1为右
float singlestepdistance = 0; //单步移动的距离（单位为毫米）
int totalsteplength = 0; //整体需要移动的步数（单位为 步数，比如100步，每步走 singlestepdistance毫米）
int currentsteplength = 0; //当前已经移动的步数
int movefreq = 0; //滑台移动的频率 即表示 一秒钟输出多少个PWM方波
int tagstate = 0; // 0 1 2 分别表示 Ready Run 以及 Finish
int linkstate = 0; // 0 表示没有连接上， 1 表示成功连接上 （体现问 TFT屏幕上的小圆圈 0时为红色 1时为绿色）
int triggermode = 0; //触发模式， 0 时表示内部触发， 1时表示软件触发 （尚未完全实现） (体现为TFT的方块，0时为中空，1时为紫色填充)
uint16_t intervaltime = 0; //触发间隔。 当前的想法是，当且仅当 triggermode设置为0 的时候，每次等待相机执行拍摄的 所需要的时间。 单位为毫秒。后续可能会有改动



void reset_parameter()
{
  //初始化系统全局变量的时候 还要对输出的各引脚同样进行初始化
  direction = 0;
  singlestepdistance = 0;
  totalsteplength = 0;
  currentsteplength = 0;
  movefreq = 0;
  tagstate = 0;
  linkstate = 0;
  triggermode = 0;
  intervaltime = 0;
  tft_print("All parameter have been reset.");
  
  digitalWrite(PWM_PIN,LOW);
  if (direction==0){digitalWrite(DIR_PIN,LOW);} else{digitalWrite(DIR_PIN,HIGH);}


}

void move_stage(float distance,int movefreq)
{

  //控制PWM端输出PWM方波 注意这里的等待时间 是 时间除以频率得到的 int型量
  int coefficient = 80; //考虑到电机的细分设置 不同所以认为 所需PWM总数量=距离*ciefficient 后续需要调整 
  int delaytime = 500/movefreq; //1000/movefreq/2 因为一个周期要有两次延时
  digitalWrite(PWM_PIN,LOW); //首先初始化一下，考虑共阳和共阴的接法不同，对于我的情况，这里将其进行拉低

  for(int i=0;i<(int)distance*coefficient;i++)
  {
     digitalWrite(PWM_PIN,HIGH);
      vTaskDelay(delaytime); //后续验证一下这里是使用delay还是 vTaskDelay
     digitalWrite(PWM_PIN,LOW);
      vTaskDelay(delaytime);
  }
  
}

void Output_change_direction()
{
  direction = 1 - direction;
  digitalWrite(DIR_PIN,direction);
}





void setup(void)
{
  Serial.begin(9600);

  // pinMode(ENA_PIN,OUTPUT);
  // pinMode(DIR_PIN,OUTPUT);
  pinMode(PWM_PIN,OUTPUT);
  // pinMode(UKN_PIN,OUTPUT);
  // pinMode(CAM1_TRI,OUTPUT);
  // pinMode(CAM2_TRI,OUTPUT);
  pinMode(LED,OUTPUT);
  xMutex = xSemaphoreCreateMutex();
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  int queue_length = 5;
  queue = xQueueCreate(queue_length, sizeof(Coremsg)); //创建队列
  
  init_widget();
  xTaskCreatePinnedToCore(TaskCoreGUI, "TaskCoreGUI", 20000, NULL, 2, NULL, 0); // Core 0
  xTaskCreatePinnedToCore(Taskpikapika,"Taskpikapika",20000,NULL,1,NULL,1); //Core 0
  xTaskCreatePinnedToCore(TaskCorePWM, "TaskCorePWM", 50000, NULL,1 , NULL, 1); // Core 1
}

//现在考虑上位机如何传输指令，以及对应下面如何解析
//上位机




void __response_to_case_serial_sign_up(char* dir,char* steplen,char* ttlen,char* freq,char* interval)
{
  direction = atoi(dir);
  singlestepdistance = atof(steplen);
  totalsteplength = atoi(ttlen);
  movefreq = atoi(freq);
  intervaltime =  strtoul(interval, NULL, 10);
  currentsteplength = 0;
  call_gui_set_tag(direction,singlestepdistance,totalsteplength,0);
  return;

}

void start_work()
{
  call_gui_set_tag(direction,singlestepdistance,totalsteplength,1);
  //开始工作
  for(int i=0;i<totalsteplength;i++)
  {
    call_gui_update_progressBar();
    //1. 先拍照
    cam1_capture();
    vTaskDelay(intervaltime);
    //2. 移动滑台
    move_stage(singlestepdistance,movefreq);
    //tft_updateProgressBar(++currentsteplength,totalsteplength); % 这个函数用错了！应该是call_tft_updateprogresssbar  
    
    vTaskDelay(1000);//滑台移动之后多等1秒让系统稳下来
  }
  //工作结束
  call_gui_set_tag(direction,singlestepdistance,totalsteplength,2);
}


void call_gui_update_progressBar()
{
  Coremsg msg = {0};
  msg.key = MSG_STEP;
  if (xQueueSend(queue, &msg, portMAX_DELAY) != pdPASS) {blinkled(10);}
}



void cam1_capture()
{
  char txt[50];
  sprintf(txt, "take %d\/%d picture.", currentsteplength, totalsteplength);
  call_gui_print(txt);
  vTaskDelay(1000);
  call_gui_print("capture ok."); 
  vTaskDelay(80);
  return;
}

void parse_serial_order(char** splitDataArray,int splitDataCount )
{
  if (splitDataCount < 1)
  {
    Serial.println("Error, No data to parse");
    return;
  }
  char* firstToken = splitDataArray[0];

  switch (atoi(firstToken))
  {
    case SERIAL_SOFT_DIRECTION_OFF:
      direction = 0;
      digitalWrite(DIR_PIN,LOW);
      break;
    case SERIAL_SOFT_DIRECTION_ON:
      direction=1;
      digitalWrite(DIR_PIN,HIGH);
      break;
    case SERIAL_SOFT_MOV:

      move_stage(atof(splitDataArray[1]),int(atof(splitDataArray[2])));
      vTaskDelay(20);
      break;
    

    case SERIAL_START_HARD:
      start_work();
      break;

    case SERIAL_SIGN_UP:
      //char* mydirection = splitDataArray[1]; //移动方向，字符串的0和1
      // char* dingleStepDistance = splitDataArray[2]; //单步步长，字符串的Float
      // char* totalStepLength = splitDataArray[3]; // 移动的总步数，字符串的int
      // char* moveFreq = splitDataArray[4]; //频率，字符串的int
      // char* intervaltime = splitDataArray[5]; //间隔时间  字符串的 uint16_t 
      //本是想这样处理的，但是没办法，C++规定在case中不允许定义新的变量，因此这里由一个外部函数实现
      //这样处理不安全，缺少数据的检错  先凑合使用Pyside6凑合
      __response_to_case_serial_sign_up(splitDataArray[1],splitDataArray[2],splitDataArray[3],splitDataArray[4],splitDataArray[5]);
      break;
      
    case SERIAL_INIT:
      call_gui_init();
      Serial.println("parsed init order, and i have called gui to reset.");
      break;
    case SERIAL_TFTPRINT:
      //这里只检索第2个字符串了就是要展示的所有内容
      call_gui_print(splitDataArray[1]);
      Serial.println("parsed print order, and i have called gui to print");
      break;
    case SERIAL_SWITCH:
      Serial.print("enter Serial switch ->");
      Serial.print(splitDataArray[1]);
      
      if (atoi(splitDataArray[1])==SERIAL_SWITCH_ON){Serial.println("entered.");call_gui_switch_link(1);Serial.println("zhixinglema?");}
      else if(atoi(splitDataArray[1])==SERIAL_SWITCH_OFF){call_gui_switch_link(0);}
      else{Serial.println("i have got switch order but i don't know flag.");}
      Serial.println("parsed switch link state order, and i have called call_gui_switch_link");
      break;

    case SERIAL_SET_TRIGGER_MODE:
      if(atoi(splitDataArray[1])==SERIAL_TRIGGER_MODE_ON){call_gui_print("set to hard trig.");call_gui_switch_work_mode(0);}
      else if(atoi(splitDataArray[1])==SERIAL_TRIGGER_MODE_OFF){call_gui_print("set to soft trig.");call_gui_switch_work_mode(1);}
      break;

    default:
      Serial.println("enter default.");
      break;
  }

}


//Core0
void TaskCorePWM(void *pvParameters)
{
  int i =0;
  char incomingData[50];
  char* splitDataArray[10];//用于存储分割后的数据
  int splitDataCount = 0;
  while(1)
  {
    if (Serial.available() > 0) 
    {
      
      int size = Serial.readBytesUntil('\n', incomingData, sizeof(incomingData) - 1); //这行代码 不仅仅读到 \n 而是去读到 sizeof(incomingData)-1 比如现在就是 123456\n
      incomingData[size] = '\0'; 
      char ch[50] = "From ESP32: ";
      strcat(ch,incomingData);      
      Serial.println(ch);

      splitData(incomingData,splitDataArray,splitDataCount,10);

      for (int i=0; i<splitDataCount;++i)
      {
        Serial.print("Split data ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(splitDataArray[i]);
      }
      
      parse_serial_order(splitDataArray,splitDataCount);
      


      if (strcmp(incomingData, "123") == 0)//
      {
        // Serial.println(incomingData);
        call_gui_print(incomingData);

        blinkled(3);
      }
    }

  }

}

//Core1
void TaskCoreGUI(void *pvParameters)
{
  Coremsg receivedMsg;
  while(1)
  {
    if (xQueueReceive(queue, &receivedMsg, portMAX_DELAY) == pdPASS)
    {
      if (xSemaphoreTake (xMutex, portMAX_DELAY))
      {
        parse_order_from_PWM(receivedMsg);
        xSemaphoreGive (xMutex);
      }
    }
  }

}

//Core1
void Taskpikapika(void *pvParameters)
{
  int i=1;
  tft.setTextSize(2);
  while(1)
  {
    
    i = i % 32;
    if (xSemaphoreTake (xMutex, portMAX_DELAY))
    {
      tft.pushImage(180, 0, 60, 60, frames[i]); // 直接使用帧数组
      xSemaphoreGive (xMutex);
    }
    i++;
    vTaskDelay(35);
  }
}

//空循环
void loop(){}

void blinkled(int k)
{
  for(int i=0;i<k;i++)
  {
    digitalWrite(LED,HIGH);
    vTaskDelay(500);
    digitalWrite(LED,LOW);
    vTaskDelay(500);
  }
}

void splitData(const char* inputData, char** outputArray, int& outputCount,int maxOutput)
{
  outputCount = 0;
  const char* delimiter = ",";
  char* token = strtok((char*)inputData,delimiter);
  while (token != NULL && outputCount<maxOutput)
  {
    outputArray[outputCount++] = token;
    token = strtok(NULL,delimiter);
  }
}


void call_gui_init() {
  // 用于初始化
  Coremsg msg = {0};
  msg.key = MSG_INIT;
  if (xQueueSend(queue, &msg, portMAX_DELAY) != pdPASS) {blinkled(10);}
}

void call_gui_switch_link(bool v)
{
  Coremsg msg = {0};
  msg.key = MSG_SWITCH;
  if(v){    snprintf(msg.value,sizeof(msg.value),"%d",MSG_SWITCH_ON);    }
  else{     snprintf(msg.value,sizeof(msg.value),"%d",MSG_SWITCH_OFF);   }
  if (xQueueSend(queue, &msg, portMAX_DELAY) != pdPASS) {blinkled(10);}

}

void call_gui_set_tag(int dir,float stepsize,int totalstep,int state)
{
  Coremsg msg = {0};
  msg.key = MSG_SETTAG;
  msg.dir = dir;
  msg.stepsize = stepsize;
  msg.totalstep = totalstep;
  msg.state = state;
  if (xQueueSend(queue, &msg, portMAX_DELAY) != pdPASS) {blinkled(10);}
}

void call_gui_step()
{
  Coremsg msg = {0};
  msg.key = MSG_STEP;
  if (xQueueSend(queue, &msg, portMAX_DELAY) != pdPASS) {blinkled(10);}
}

void call_gui_print(char*v)
{
  Coremsg msg = {0};
  msg.key = MSG_TFTPRINT;
  strncpy(msg.value, v, sizeof(msg.value)-1);
  if (xQueueSend(queue, &msg, portMAX_DELAY) != pdPASS) {blinkled(10);}
}

void call_gui_switch_work_mode(bool v)
{
  Coremsg msg = {0};
  msg.key = MSG_WORK_MODE;
  if(v)
  {
    snprintf(msg.value,sizeof(msg.value),"%d",MSG_WORK_MODE_HARD);
  }
  else{     snprintf(msg.value,sizeof(msg.value),"%d",MSG_WORK_MODE_SOFT);   }
  if (xQueueSend(queue, &msg, portMAX_DELAY) != pdPASS) {blinkled(10);}
}

void parse_order_from_PWM(Coremsg msg)
{
  //用来实现 G 核对 P 核的指令接收与解析
  //如何解析的应该在外面实现 这里仅仅是收到之后如何进行解析
  switch(msg.key)
  { 
    case MSG_WORK_MODE:
      if(atoi(msg.value)==MSG_WORK_MODE_HARD){tft_change_work_state(0);} //默认false为内出发
      else if(atoi(msg.value)==MSG_WORK_MODE_SOFT){tft_change_work_state(1);}
      break;
    case MSG_INIT:
      init_widget();
      reset_parameter();
      break;
    case MSG_SWITCH:
      if(atoi(msg.value)==MSG_SWITCH_ON){tft_change_link_state(1);}
      else if(atoi(msg.value)==MSG_SWITCH_OFF){tft_change_link_state(0);}
      else 
      {
        char log[]="Errk=";
        char log2[10];
        char log3[20];
        sprintf(log2,"%d",MSG_SWITCH);
        strcat(log,log2);
        strcat(log,"v=");
        strcat(log,msg.value);
        tft_print(log);
      }
      break;
    case MSG_SETTAG:
      tft_set_tag(msg.dir,msg.stepsize,msg.totalstep,msg.state);
      break;
    case MSG_STEP:
      // tft_print("under construction");
      tft_updateProgressBar(++currentsteplength,totalsteplength);
      break;
    case MSG_TFTPRINT:
      tft_print(msg.value);
      break;
    default:
      tft_print("pserr:wrongk");

  }
}

void init_widget()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW,TFT_BLACK);
  tft.drawString("Link State:",8,1,1);

  tft.drawSmoothCircle(SPHERE_POS_X, SPHERE_Y, SPHERE_SIZE, TFT_RED,TFT_BLACK);  
  tft.drawString("Pin Defination: ",8,17,1);

  tft.setTextSize(1);
  tft.setCursor(5,34);
  tft.setTextColor(TFT_GREEN);
  tft.print("1. ENA: PIN");
  tft.drawNumber(ENA_PIN,75,34);
  tft.setTextColor(TFT_ORANGE);
  tft.print("    2. DIR: PIN");
  tft.drawNumber(DIR_PIN,168,34);

  tft.setCursor(5,43);
  tft.setTextColor(TFT_SKYBLUE);
  tft.print("3. PWM: PIN");
  tft.drawNumber(PWM_PIN,75,43);
  tft.setTextColor(TFT_BROWN);
  tft.print("    4. GND: PIN");
  tft.drawNumber(UKN_PIN,168,43);

  tft.setCursor(5,52);
  tft.setTextColor(TFT_CYAN);
  tft.print("5. Cam1Tri:");
  tft.drawNumber(CAM1_TRI,75,52);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.print("    6. Cam2Tri:");
  tft.drawNumber(CAM2_TRI,168,52);

  tft.drawLine(1,63,240,63,TFT_WHITE);

  tft.setTextSize(2);
  tft.setTextColor(TFT_LIGHTGREY,TFT_BLACK);
  tft.drawString("CurrTask: ",2,68,1);
  tft.setTextColor(tft.color565(240,41,61),TFT_BLACK);
  // tft.drawString("<<0.0@00 [Rdy]",TAG_X,TAG_Y,1);
  //drawProgressBar(BAR_X, BAR_Y, BAR_WIDTH,BAR_HEIGHT, BAR_ROUND_RADIUS, BAR_CRT_PERCENT);  // 绘制进度条，进度为0%
  tft_change_link_state(0);
  tft_change_work_state(0);
  //tft_set_tag(0,0.0,0,0,TAG_X,TAG_Y);
  tft_initProgressBar();
  // tft_updateProgressBar(273,300);

}





void tft_print(char* ss)
{
  tft.setTextSize(1);
  tft.setTextColor(TFT_RED,TFT_BLACK);
  tft.fillRect(120,68,120,16,TFT_BLACK);
  tft.setCursor(120,70);
  tft.print(ss);
  vTaskDelay(20);
}
 
void tft_change_link_state(bool flag)
{
  if(flag)
  {
    tft.drawSmoothCircle(SPHERE_POS_X,SPHERE_Y,SPHERE_SIZE,TFT_GREEN,TFT_BLACK);
    
  }
  else
  {
    tft.drawSmoothCircle(SPHERE_POS_X,SPHERE_Y,SPHERE_SIZE,TFT_RED,TFT_BLACK);
    
  }

}

void tft_set_tag(int DIR, float STPSIZE, int TTSTP, int STATE) 
{
  //   用来实现对标识符的GUI更新。
  // DIR: <type 'int'> 0代表往左 1 代表往右
  // STPSIZE <type 'float'> 表示每一步的移动距离
  // TTSTP <type 'int'> 为 TotalStep 的缩写，表示位移的总步数
  // MOVFREQ <type 'int'> 为PWM波的输出频率
  // STATE <type 'int'> 用来表示当前运动状态 0 为 Rdy 准备状态 1为 Run 表示运行状态 2 为 Fns 表示结束状态
  // DIR <>
  char tag1[3];    // Direction
  char tag2[16];   // Step size formatted string, ensure enough space for float
  char tag3[5];    // Total steps formatted string, enough for max 4 digits   // Move frequency, enough for max 5 digits
  char tag5[10];   // Status

  // Set direction
  if (DIR==0) {
    strcpy(tag1, "<<");
  } else {
    strcpy(tag1, ">>");
  }

  // Format step size to two decimal places
  sprintf(tag2, "%.2f", STPSIZE);

  // Format total steps
  sprintf(tag3, "%d", TTSTP);

  // Set status and text color based on state
  switch (STATE) {
    case 0:
      strcpy(tag5, "[Rdy]");
      tft.setTextColor(TFT_SKYBLUE , TFT_BLACK);
      break;
    case 1:
      strcpy(tag5, "[Run]");
      tft.setTextColor(tft.color565(240, 41, 61), TFT_BLACK);
      break;
    case 2:
      strcpy(tag5, "[Fns]");
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      break;
    default:
      strcpy(tag5, "[ERR]");
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      break;
  }



  // Calculate total length needed for the result
  int totalLength = strlen(tag1) + strlen(tag2) + 1 + strlen(tag3) + 2  + strlen(tag5) + 1;
  char result[totalLength];
  strcpy(result, tag1);
  strcat(result, tag2);
  strcat(result, "@");
  strcat(result, tag3);
  strcat(result, tag5);
  tft.fillRect(TAG_X,TAG_Y,240,16,TFT_BLACK);
  tft.setTextSize(2);
  vTaskDelay(10);
  tft.drawString(result,TAG_X,TAG_Y,1);
}

void tft_change_work_state(bool v)
{
  // false 为内触发也就是硬件触发  true 为外触发也就是软件触发
  if(v)
  {
    
    tft.fillRect(SPHERE_POS_X+10,SPHERE_Y-5,10,10,TFT_BLACK);
    vTaskDelay(100);
    tft.fillRect(SPHERE_POS_X+10,SPHERE_Y-5,10,10,TFT_VIOLET  );
  }
  else{
      tft.fillRect(SPHERE_POS_X+10,SPHERE_Y-5,10,10,TFT_BLACK);
      vTaskDelay(100);
       tft.drawRect(SPHERE_POS_X+10,SPHERE_Y-5,10,10,TFT_VIOLET  );}
}

void drawProgressBar(int x, int y, int w, int h, int r, int percent) {
  // 绘制外框，白色圆角矩形
  tft.drawRoundRect(x, y, w, h, r, TFT_WHITE);
  
  // 计算内部填充的宽度
  int fillW = (percent * (w - 2)) / 100;
  // 绘制进度，绿色填充
  tft.fillRoundRect(x + 1, y + 1, fillW, h - 2, r, TFT_GREEN);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  tft.drawFloat(0.00,2,x+w+3,y-1);
  tft.drawChar('%',x+w+78,y-1);
}

void tft_initProgressBar()
{
  //用来替代drawProgressBar 这里什么参数都不需要，后续更新都在updateProgressBar中
  tft.drawRoundRect(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, BAR_ROUND_RADIUS, TFT_WHITE);
  // tft.fillRoundRect(BAR_X + 1, BAR_Y + 1, fillW, h - 2, r, TFT_GREEN);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("0/0",BAR_X+BAR_WIDTH+5,BAR_Y-1);

}





void updateProgressBar(int x,int y,int w,int h, int r, float percent)
{
  int fillW = (int(percent)*(w-2))/100;
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.fillRoundRect(x+1,y+1,w-2,h-2,r,TFT_BLACK);
  tft.fillRoundRect(x + 1, y + 1, fillW, h - 2, r, TFT_GREEN);
  tft.fillRect(BAR_X+BAR_WIDTH+3,BAR_Y-1,71,BAR_HEIGHT,TFT_BLACK);
  
  tft.drawFloat(percent,2,x+w+3,y-1);
}

void tft_updateProgressBar(int crt,int ttl)
{
  //用来替代updateProgressBar函数现在应该展示的是步数/总步数
  int fillW = BAR_WIDTH * crt / ttl;
  char label[50];
  sprintf(label,"%d\/%d",crt,ttl);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.fillRoundRect(BAR_X+1,BAR_Y+1,BAR_WIDTH-2,BAR_HEIGHT-2,BAR_ROUND_RADIUS,TFT_BLACK);
  tft.fillRoundRect(BAR_X+1,BAR_Y+1,fillW,BAR_HEIGHT-2,BAR_ROUND_RADIUS,TFT_GREEN);
  tft.fillRect(BAR_X+BAR_WIDTH+3,BAR_Y-1,71,BAR_HEIGHT,TFT_BLACK);
  //我想在这里 将 crt 和 ttl 转化成 字符串 连接起来赋值给label
  tft.drawString(label,BAR_X+BAR_WIDTH+5,BAR_Y-1);
}