#include "IRremote.h"
int calibrationTime = 10; 
byte CellingLed = 9; /* Лента потолка */ 
byte ledB = 6;  // лента пола
byte pirDoor = 5; // Датчик Движения
byte pirMirror = 4; //Датчик у Зеркала 
IRrecv irrecv(2); //  IR сенсор 
byte dayNight = 7; // режим управления, кнопка - день/ночь 
decode_results results;
byte irSignal;
byte irDay;
byte irNight;
byte irAllOff;
byte buttonstate; 
long unsigned int moveTime; // Время в которое принят сигнал отсутсвия движения
long unsigned int pause = 6000; // Пауза, после которой движение считается оконченным
boolean movementDetected = true; // true - движения нет
boolean moveFlag; // Сигнализирует о необходимости запомнить время начала отсутствия движения
void setup ()
{
  irrecv.enableIRIn();
  Serial.begin(9600);
  pinMode(CellingLed,OUTPUT);
  pinMode(ledB,OUTPUT);
  digitalWrite(dayNight, HIGH); // включение внутренней подтяжки на входе 7
  pinMode(pirDoor,INPUT);
  pinMode(pirMirror,INPUT);
  digitalWrite(pirDoor, LOW);
  digitalWrite(pirMirror, LOW);
// Калибровка датчиков движения
  Serial.print("Calibrating");
  for(int i = 0; i < calibrationTime; i++)
    {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

void loop()
{
if ( irrecv.decode( &results)){
  delay(300);
  if (results.value == 0xFF3AC5) {irSignal = 0;}  /* Задание заранее считанных ИК сигналов пульта */
  if (results.value == 0xFFBA45) {irSignal = 1;}

  irrecv.resume();  
}
// Выбор режима управления - ручной/автоматический
  byte DayNightSwitch = !digitalRead(dayNight);
  byte a;
  byte b;
  if (DayNightSwitch == LOW) {a=1;} else {a=0;}
  if (irSignal == 1)         {b=1;} else {b=0;}
  if (a==b) {buttonstate = 1;} 
  else      {buttonstate = 0;}

  
byte pirstate;
if (digitalRead(pirDoor) == HIGH || digitalRead(pirMirror) == HIGH)
{ pirstate = HIGH;}
// лента пола со счетчиком движения
if( pirstate==HIGH & buttonstate == 0 )
{
 if(movementDetected)
 {
  movementDetected = false;
  Serial.println("Motion detected");
  delay(50); 
  }
  moveFlag = true;
  analogWrite(ledB,240);// приглушенный синий 
}
else
{
  if(moveFlag)
  {
    moveTime = millis();
    moveFlag = false;
    }
  if(!movementDetected && millis() - moveTime > pause )
  {
    movementDetected = true;
    Serial.println("Motion finished");
    delay(50);    
    analogWrite(ledB,255);
    }
  
}
// лента потолка со счетчиком движения
if (pirstate==HIGH & buttonstate == 1 )
{
  // установка таймера ожидания движения 5сек
 if(movementDetected)
 {
   movementDetected = false;
   Serial.println("Motion detected");
   delay(50); 
  }
  moveFlag = true;
  digitalWrite(CellingLed,HIGH);
}
else
{
  if(moveFlag)
  {
    moveTime = millis();
    moveFlag = false;
    }
  if(!movementDetected && millis() - moveTime > pause )
  {
    movementDetected = true;
    Serial.println("Motion finished");
    delay(50);    
    digitalWrite(CellingLed,LOW);
    } 
}
/* Информация для отладки
Serial.print("IR signal  ");
Serial.println(irSignal);
Serial.print("dayNight  ");
Serial.println(DayNightSwitch);
Serial.print("buttonstate  ");
Serial.println(buttonstate);
delay(1000);
*/
}
