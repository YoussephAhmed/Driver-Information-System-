#include <EEPROM.h>
#include <LiquidCrystal.h>
// EEPROM is good 100,000 write/erase cycles
// 3.3 ms per write
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
float vRef = 5.07;

//Current's measure Circuit
int ifChargedPin = 8 ; 
int savePin = 13; 
float mAh = 0.0;
float current = 0.0;
float battVolt = 0.0;
float shuntVolt = 0.0;
float soc = 100;
float rShunt = 0.39;
unsigned long previousMillis = 0;
unsigned long millisPassed = 0;
uint8_t EEPROMaddress = 130; //SOC



//Battery's volt measure circuit
float rBattShunt= 3820.0;
float rTotal=13650.0;
double sensorValue ;
double vBattary;
void setup() {
lcd.begin(16, 4);
pinMode(ifChargedPin,INPUT);
pinMode(savePin,INPUT);
}

void loop() {
 
  //lcd.println( millis() / 1000  );
  
  if( digitalRead(savePin) == HIGH ) //Push Button to Save
  
  {
    
  EEPROM.write(EEPROMaddress , soc);
  
  //lcd.setCursor(12,1);
  lcd.println("Svd");
  }
  
 if(digitalRead(ifChargedPin) == HIGH) // if the battery is Fully Charged 
  soc = 100;

else if(digitalRead(ifChargedPin == LOW )) // back from stop
soc = EEPROM.read(EEPROMaddress);



soc = EEPROM.read(EEPROMaddress);


// Amp.h & SOC calculations  
shuntVolt = (analogRead(A2) - analogRead(A1))  * 5.07/1024;
current = shuntVolt / rShunt;

millisPassed = millis() - previousMillis;
mAh = mAh + (current * 1000.0) * (millisPassed / 3600000.0);
previousMillis = millis();

soc = soc - (mAh*100/3000.0) ; // Capacity of battery = 3000




//Baterry's volt calculations
sensorValue = analogRead(A0) * (vRef/1024.0);
vBattary = sensorValue*(rTotal/rBattShunt);



  
//lcd.print("A1 : ");
//lcd.println( analogRead(A1) * 5.07/1024);

//lcd.print("A2 : ");
//lcd.println( analogRead(A2) * 5.07/1024);
 lcd.setCursor(0, 0);
lcd.print("Vdeff=");
lcd.print(shuntVolt );

lcd.print(",I= ");
lcd.print( current );
lcd.setCursor(0, 1);

lcd.print("SOC=");
lcd.print(soc);
lcd.setCursor(0, 2);
lcd.print("Qlost=");
lcd.print(mAh);
lcd.setCursor(0, 3);

lcd.print("v.bat=");
lcd.print(vBattary);





delay(5000); //Sampling Time 



}
