#include <SoftwareSerial.h>
#include <Cytron_PS2Shield.h>
#include <Wire.h>
#include <MechaQMC5883.h>

MechaQMC5883 qmc;

Cytron_PS2Shield ps2(2, 3); // SoftwareSerial: Rx and Tx pin
//Cytron_PS2Shield ps2; // HardwareSerial


#define FRpwm 5
#define FLpwm 9
#define RLpwm 11
#define RRpwm 10

#define FRdir 4
#define FLdir 8
#define RLdir 12
#define RRdir 13

#define rackup1 6
#define rackup2 7

void movebackward(byte vel = 40);
void moveforward(byte vel = 40);
void moveleft(byte vel = 40);
void moveright(byte vel = 40);

int initor;
//float acc = 0.1;

void setup() {
  pinMode(FRpwm, OUTPUT);
  pinMode(FLpwm, OUTPUT);
  pinMode(RLpwm, OUTPUT);
  pinMode(RRpwm, OUTPUT);
 
  pinMode(FRdir, OUTPUT);
  pinMode(FLdir, OUTPUT);
  pinMode(RLdir, OUTPUT);
  pinMode(RRdir, OUTPUT);

  pinMode(rackup1, OUTPUT);
  pinMode(rackup2, OUTPUT);
  digitalWrite(rackup1,HIGH);
  digitalWrite(rackup2,LOW);

  Serial.begin(9600);
  delay(1000);
  ps2.begin(9600);
  delay(1000);

  Wire.begin();
  qmc.init();

  int x, y, z;
  int azimuth;
  //float azimuth; //is supporting float too
  qmc.read(&x, &y, &z,&azimuth);
  initor = azimuth;
}

void loop() {

  byte velocity = ps2.readButton(PS2_LEFT_1) == 0 ? 60 : 150;
  Serial.println("pwm: "+String(velocity));

  if(ps2.readButton(PS2_JOYSTICK_RIGHT_X_AXIS) < 40)
  {
    rotateanticlock(velocity);
    Serial.println("anticlock");
  }
  else if(ps2.readButton(PS2_JOYSTICK_RIGHT_X_AXIS) > 150)
  {
    rotateclock(velocity);
    Serial.println("clock");
  }
  else if(ps2.readButton(PS2_JOYSTICK_LEFT_Y_AXIS)>150)
  {
    movebackward(velocity);
    Serial.println("backward");
  }
  else if(ps2.readButton(PS2_JOYSTICK_LEFT_Y_AXIS)<40)
  {
    moveforward(velocity);
    Serial.println("forward");
  }
  else if(ps2.readButton(PS2_JOYSTICK_LEFT_X_AXIS)>150)
  {
    moveright(velocity);
    Serial.println("right");
  }
  else if(ps2.readButton(PS2_JOYSTICK_LEFT_X_AXIS)<40)
  {
    moveleft(velocity);
    Serial.println("left");
  }
  else
  {
    Serial.println("stop");
    stopmotion();
  }

  if(ps2.readButton(PS2_RIGHT_2)==0)
  {  
   digitalWrite(rackup2, HIGH);
   delay(500);
   digitalWrite(rackup1, LOW);
   Serial.println("rack down");
  }
  else if(ps2.readButton(PS2_RIGHT_1)== 0)
  {
    digitalWrite(rackup1, HIGH);
    delay(500);
    digitalWrite(rackup2, LOW);
    Serial.println("rack up");
  }
  
  if(ps2.readButton(PS2_UP)==0)
   digitalWrite(rackup2, HIGH);
  else if(ps2.readButton(PS2_DOWN)== 0)
   digitalWrite(rackup2, LOW); 

  if(ps2.readButton(PS2_CROSS)==0)
  {
   orient();
  }

  if(ps2.readButton(PS2_TRIANGLE)==0)
  {
    initor = readCompass();
  }
   
      
}

void movebackward(byte vel)
{
  analogWrite(FLpwm, vel);
  analogWrite(FRpwm, vel);
  analogWrite(RRpwm, vel);
  analogWrite(RLpwm, vel);

  digitalWrite(FLdir, HIGH);
  digitalWrite(FRdir, HIGH);
  digitalWrite(RLdir, HIGH);
  digitalWrite(RRdir, HIGH);
}

void moveforward(byte vel)
{
  analogWrite(FLpwm, vel);
  analogWrite(FRpwm, vel);
  analogWrite(RRpwm, vel);
  analogWrite(RLpwm, vel);

  digitalWrite(FLdir, LOW);
  digitalWrite(FRdir, LOW);
  digitalWrite(RLdir, LOW);
  digitalWrite(RRdir, LOW);
}

void moveleft(byte vel)
{
  analogWrite(FLpwm, vel);
  analogWrite(FRpwm, vel);
  analogWrite(RRpwm, vel);
  analogWrite(RLpwm, vel);

  digitalWrite(FLdir, HIGH);
  digitalWrite(FRdir, LOW);
  digitalWrite(RLdir, LOW);
  digitalWrite(RRdir, HIGH);
}

void moveright(byte vel)
{
  analogWrite(FLpwm, vel);
  analogWrite(FRpwm, vel);
  analogWrite(RRpwm, vel);
  analogWrite(RLpwm, vel);
 
  digitalWrite(FLdir, LOW);
  digitalWrite(FRdir, HIGH);
  digitalWrite(RLdir, HIGH);
  digitalWrite(RRdir, LOW);
}

void stopmotion()
{
  analogWrite(FLpwm, 0);
  analogWrite(FRpwm, 0);
  analogWrite(RRpwm, 0);
  analogWrite(RLpwm, 0);
  digitalWrite(FLdir, LOW);
  digitalWrite(FRdir, LOW);
  digitalWrite(RLdir, LOW);
  digitalWrite(RRdir, LOW); 
}

void rotateanticlock(byte vel)
{
  analogWrite(FLpwm, vel);
  analogWrite(FRpwm, vel);
  analogWrite(RRpwm, vel);
  analogWrite(RLpwm, vel);
 
  digitalWrite(FRdir, LOW);
  digitalWrite(FLdir, HIGH);
  digitalWrite(RLdir, HIGH);
  digitalWrite(RRdir, LOW);   
}

void rotateclock(byte vel)
{
  analogWrite(FLpwm, vel);
  analogWrite(FRpwm, vel);
  analogWrite(RRpwm, vel);
  analogWrite(RLpwm, vel);

  digitalWrite(FRdir, HIGH);
  digitalWrite(FLdir, LOW);
  digitalWrite(RLdir, LOW);
  digitalWrite(RRdir, HIGH);   
}

int readCompass()
{
  int x, y, z;
  int azimuth;
  //float azimuth; //is supporting float too
  qmc.read(&x, &y, &z,&azimuth);
  return azimuth;
}


void orient()
{
  int curor = readCompass();
  Serial.println(curor);
  Serial.println(initor);
  while(abs(curor-initor) > 3)
  {
    Serial.println(curor-initor);
    if(curor-initor > 0)
      rotateanticlock(40);
    else
      rotateclock(40);
    curor = readCompass(); 
  }
  stopmotion();
}
  
  

