
#include "DHT.h"

//水位传感器参数定义
#define water_pin A5
#define water_led_pin A4
int val = 0;
//水位传感器参数定义

//湿度传感器1参数定义
#define DHTPIN1 8
#define DHTTYPE DHT22
DHT dht1(DHTPIN1,DHTTYPE);
unsigned int timecnt;
unsigned char hum_flag;
int h1;
int t1;
int h1_l;
int h1_h;
int t1_l;
int t1_h;
//湿度传感器1参数定义

//湿度传感器2参数定义
#define DHTPIN2 4
DHT dht2(DHTPIN2,DHTTYPE);
int h2;
int t2;
int h2_l;
int h2_h;
int t2_l;
int t2_h;
//湿度传感器2参数定义

//数码管参数定义
#define SCK 6
#define RCK 5
#define SER 7
unsigned char LED_0F[]=
{0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C};
unsigned char LED[8]; 
//数码管参数定义

//风机参数定义
#define motor 2
#define PWR A0
bool power = false;
bool pwr_key = false;
//unsigned int power_cnt = 0;
//风机参数定义

//延时电路参数定义
#define rc_charge 10
#define rc_inspection 9
//延时电路参数定义

//压缩机参数定义
#define SHIFT A1  //切换键
#define compress 3
bool state = false; //模式切换，除湿是true，通风是false
//int compress_time;
//unsigned int compress_cnt = 0;
bool compress_flag;
bool compress_key = false;
//压缩机参数定义

//终止湿度调节参数定义
#define HUM_ADD A2  //湿度加
#define HUM_SUB A3  //湿度减
char hum_zero = 30; //终止湿度调节
char zero_l;
char zero_h;
bool adjust_flag = false; //调整终止湿度
bool add_key = false;
bool sub_key = false;
//unsigned int add_cnt = 0;
//unsigned int sub_cnt = 0;
int adjust_time;
//终止湿度调节参数定义

int tube_cnt = 10;
void setup() {
  // put your setup code here, to run once:
  
  //水满检测接口定义
  pinMode(water_led_pin,OUTPUT);
  digitalWrite(water_led_pin,HIGH);
  //水满检测接口定义
  
  //数码管接口定义
  pinMode(SCK,OUTPUT);
  pinMode(RCK,OUTPUT);
  pinMode(SER,OUTPUT);
  //数码管接口定义
  
  //风机接口定义
  pinMode(PWR,INPUT_PULLUP);
  pinMode(motor,OUTPUT);
  digitalWrite(motor,LOW);
  //风机接口定义、

  //延时电路接口初始化
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(rc_charge,OUTPUT);
  digitalWrite(rc_charge,HIGH);
  pinMode(rc_inspection,INPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  //延时电路接口初始化
  
  //压缩机接口定义
  pinMode(SHIFT,INPUT_PULLUP);
  pinMode(compress,OUTPUT);
  digitalWrite(compress,LOW);
  //压缩机接口定义

  //调节接口定义
  pinMode(HUM_ADD,INPUT_PULLUP);
  pinMode(HUM_SUB,INPUT_PULLUP);
  //调节接口定义
  
  dht1.begin();
  dht2.begin();
  timecnt=0;
  delay(2000);
  hum_flag = 1;
  Serial.begin(9600);
  Serial.println('test');
}

void loop() {
  // put your main code here, to run repeatedly:

  //水满检测
  val = analogRead(water_pin);
  if(val > 700){
    digitalWrite(water_led_pin,LOW);
  }
  else{
    digitalWrite(water_led_pin,HIGH);
  }
  //水满检测
  
  //POWER按钮按下
  if(digitalRead(PWR)==LOW && pwr_key == false)
  {
    pwr_key = true;
    power = !power;
    Serial.println(power);
  }
  if(pwr_key == true && digitalRead(PWR)==HIGH)
  {
    pwr_key = false;
  }
  digitalWrite(motor,power);
  //POWER按钮按下

  //SHIFT按钮按下
  if(power == false){
    state = false;
    digitalWrite(compress,LOW);
    digitalWrite(rc_charge,HIGH);
    compress_flag = false;
    compress_key =false;
  }
  else if(digitalRead(SHIFT)==LOW && compress_key ==false){
    compress_key = true;
    state = !state;
    if(state == true){
      compress_flag = true;
    }
    if(state == false){
      digitalWrite(compress,LOW);
      compress_flag = false;
      digitalWrite(rc_charge,HIGH);
    }
  }
  else if(digitalRead(SHIFT)==HIGH && compress_key == true){
    compress_key =false;
  }

  if(state == true && compress_flag == true){
    if(digitalRead(rc_inspection)==LOW){
      compress_flag = false;
      digitalWrite(compress,state);
      digitalWrite(rc_charge,LOW);
    }
  }
  //SHIFT按钮按下

  timecnt++;
  if((timecnt>200) && (hum_flag ==0))
  {
    hum_flag = 1;
    timecnt = 0;
  }
  if(hum_flag == 1)
  { 
    h1 = int(dht1.readHumidity());
    t1 = int(dht1.readTemperature());
    h2 = int(dht2.readHumidity());
    t2 = int(dht2.readTemperature());

    
    h1_l = h1%10;
    h1_h = h1/10%10;
    t1_l = t1%10;
    t1_h = t1/10%10;
    h2_l = h2%10;
    h2_h = h2/10%10;
    t2_l = t2%10;
    t2_h = t2/10%10;
    hum_flag = 0;
  }
  
  LED[0]=t2_l;
  LED[1]=t2_h;
  LED[2]=h2_l;
  LED[3]=h2_h;
  LED[4]=t1_l;
  LED[5]=t1_h;
  LED[6]=h1_l;
  LED[7]=h1_h;

  //调节终止湿度
  if((h1 <= hum_zero) && (power == true))
  {
    power = false;
  }
  if(digitalRead(HUM_ADD)==LOW && add_key == false && adjust_flag == false){
    add_key = true;
    adjust_flag=true;
    adjust_time = 0;
  }
  if(digitalRead(HUM_ADD)==HIGH && add_key == true && adjust_flag == false){
    add_key = false;
  }
  if(digitalRead(HUM_SUB)==LOW && sub_key == false && adjust_flag == false){
    sub_key = true;
    adjust_flag=true;
    adjust_time = 0;
  }
  if(digitalRead(HUM_SUB)==HIGH && sub_key == true && adjust_flag == false){
    sub_key = false;
  }
  if(adjust_flag){
    if(adjust_time>300){
      adjust_flag = false;
      add_key = false;
      sub_key = false;
    }
    if(digitalRead(HUM_ADD) == LOW && add_key == false){
      add_key = true;
      hum_zero++;
      adjust_time = 0;
    }

    if(digitalRead(HUM_ADD) == HIGH && add_key == true ){
      add_key = false;
    }
    
    if(digitalRead(HUM_SUB) == LOW && sub_key == false){
      sub_key = true;
      hum_zero--;
      adjust_time = 0;
    }

    if(digitalRead(HUM_SUB) == HIGH && sub_key == true){
      sub_key = false;
    }
    adjust_time++;
    zero_l = hum_zero%10;
    zero_h = hum_zero/10%10;
    
    LED[0]=zero_l;
    LED[1]=zero_h;
    LED[2]=zero_l;
    LED[3]=zero_h;
    LED[4]=zero_l;
    LED[5]=zero_h;
    LED[6]=zero_l;
    LED[7]=zero_h;
    
  }
  //调节终止湿度
  
  while(tube_cnt--)
    {
      LED8_Display ();
    } 
  tube_cnt = 10;
  delay(1);
}
void LED8_Display (void)
{
  unsigned char *led_table;          // 查表指针
  unsigned char i;
  //显示第1位
  led_table = LED_0F + LED[0];
  i = *led_table;
  LED_OUT(i);     
  LED_OUT(0x01);    
    digitalWrite(RCK,LOW);
    digitalWrite(RCK,HIGH);
  //显示第2位
  led_table = LED_0F + LED[1];
  i = *led_table;
  LED_OUT(i);   
  LED_OUT(0x02);    
    digitalWrite(RCK,LOW);
    digitalWrite(RCK,HIGH);
  //显示第3位
  led_table = LED_0F + LED[2];
  i = *led_table;
  LED_OUT(i);     
  LED_OUT(0x04);  
    digitalWrite(RCK,LOW);
    digitalWrite(RCK,HIGH);
  //显示第4位
  led_table = LED_0F + LED[3];
  i = *led_table;
  LED_OUT(i);     
  LED_OUT(0x08);
    digitalWrite(RCK,LOW);
    digitalWrite(RCK,HIGH);
    //显示第5位
  led_table = LED_0F + LED[4];
  i = *led_table;
  LED_OUT(i);     
  LED_OUT(0x10);    
    digitalWrite(RCK,LOW);
    digitalWrite(RCK,HIGH);
  //显示第6位
  led_table = LED_0F + LED[5];
  i = *led_table;
  LED_OUT(i);   
  LED_OUT(0x20);    
    digitalWrite(RCK,LOW);
    digitalWrite(RCK,HIGH);
  //显示第7位
  led_table = LED_0F + LED[6];
  i = *led_table;
  LED_OUT(i);     
  LED_OUT(0x40);  
    digitalWrite(RCK,LOW);
    digitalWrite(RCK,HIGH);
  //显示第8位
  led_table = LED_0F + LED[7];
  i = *led_table;
  LED_OUT(i);     
  LED_OUT(0x80);        
    digitalWrite(RCK,LOW);
    digitalWrite(RCK,HIGH);
}
    
void LED_OUT(unsigned char X)
{
  unsigned char i;
  for(i=8;i>=1;i--)
  {
    if (X&0x80) 
      {
        digitalWrite(SER,HIGH);
      }  
      else 
      {
        digitalWrite(SER,LOW);
      }
    X<<=1;
    digitalWrite(SCK,LOW);
    digitalWrite(SCK,HIGH);
  }
}
