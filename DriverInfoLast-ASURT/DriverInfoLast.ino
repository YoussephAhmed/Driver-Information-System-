#include <EEPROM.h>
// EEPROM is good 100,000 write/erase cycles
// 3.3 ms per write


// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8,9,4,5,6,7);




#define rpmConstant 434782.6 //rpmConstant = 10^6 * 60 / 138 ; // 138 -> number of Sector Changes in one full rotation "138 if you are using the three hall sensors interrupts,
// 138 should be 46 if you are using one hall sensor, accuracy decrease & becomes 7.82 degree"


//RPM
volatile unsigned long  T=0,PT=0;
volatile int flag=0;
volatile float rpm = 0;



//Current's measure Circuit
float vRef = 5.0;
int ifChargedPin = 15 ; 
int savePin = 13;
float current;
float Ah;
float soc; // state of charge
float SOC; // precentage of state of charge
float realBattCap = 26; //Ah
float battCap = realBattCap - .25*realBattCap; //fixing of 25% error
float Qdisch = 0;
float Qsaved = battCap; 
float shuntVolt;
float rShunt = 1;
float dis = 0;
float dia = 0.4; //m
unsigned long previousMillis = 0;
unsigned long millisPassed = 0;
uint8_t EEPROMaddress = 0; //Qdischarged
boolean saveFlag = true;
boolean saveFlag2 = true;
boolean saveFlag3 = true;


//Battery's volt measure circuit
float rBattShunt= 10000;
float rTotal=110000;
float sensorValue ;
float vBattery;


void setup()
{
lcd.begin(20,4);
pinMode(ifChargedPin,INPUT);
pinMode(savePin,INPUT);

// for pulling up and init of interrupts  
DDRD = 0X00;
PORTD = 0X07;
MCUCR = (0XEF&MCUCR);

attachInterrupt(2, sensorISR, CHANGE); //  pin 21 on arduino Mega > RED
attachInterrupt(3, sensorISR, CHANGE); //  pin 20 on arduino Mega > BLUE
attachInterrupt(4, sensorISR, CHANGE); //  pin 19 on arduino Mega > YELLOW

Serial.begin(115200);
}

void loop()
{
  
  
//RPM
T =micros();

if(T-PT>=100000 && flag==1) 
{
rpm=0;
  Serial.print("RPM = ");
  Serial.println(rpm);
flag=0;
}
//Distance
dis+=((rpm*3.14*dia)/60)* (millisPassed/1000); //m


//SOC 
  
if( digitalRead(savePin) == HIGH ) //Push Button to Save
  
  {
  EEPROM.put(EEPROMaddress, Qdisch);
  saveFlag2 = true;
  lcd.setCursor(13,2);
  lcd.print("svd");
  
  
  }
  
if(digitalRead(ifChargedPin) == HIGH && saveFlag == true) // if the battery is Fully Charged
{  
Qdisch=0;
saveFlag = false;
}


else if(digitalRead(ifChargedPin)  == LOW && saveFlag2 ==true ) // back from stop
{
  
 EEPROM.get(EEPROMaddress, Qdisch);
saveFlag2 = false;
}


// Amp.h & SOC calculations  
shuntVolt = (analogRead(A2) - analogRead(A1))  * vRef/1024;
current = shuntVolt / rShunt;

millisPassed = millis() - previousMillis;
previousMillis = millis();

Ah = current *  millisPassed / 3600000.0;
Qdisch += Ah; // state of discharge

soc = battCap - Qdisch; // state of charge


SOC = (soc / battCap) * 100.0; // precentage of state of charge 



//some conditions

if(SOC <= 0)
{
    SOC = 0;
}


if(SOC >= 100)
{
    SOC = 100;
}


if(vBattery == 0)
{
    current = 0;
}




//Baterry's volt calculations
sensorValue = analogRead(A0) * (vRef/1024.0);
vBattery = sensorValue*(rTotal/rBattShunt);



lcd.setCursor(0, 0);
lcd.print("Vs=");
lcd.print(vBattery);
lcd.print("V,I=");
lcd.print(current);
lcd.print("A");

lcd.setCursor(0, 1);
lcd.print("P=");
lcd.print(current*vBattery);
lcd.print("W,E=");
lcd.print(Qdisch*vBattery);
lcd.print("Wh");

lcd.setCursor(0,2);
lcd.print("SOC=");
lcd.print(SOC);
lcd.print("%");

lcd.setCursor(0,3);
lcd.print("RPM=");
lcd.print(rpm);
lcd.print(",Dist=");

delay(1000);



}




void sensorISR()
{
 
  PT = T - PT;
  rpm= rpmConstant/PT;
  Serial.print("RPM = ");
  Serial.println(rpm);
  PT=T;
  flag=1;
}


