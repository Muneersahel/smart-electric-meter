#include <EEPROM.h> 
#include <SoftwareSerial.h> 
#include <LiquidCrystal.h> 

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
//SoftwareSerial SIM900(10, 9);

char inchar;

float Voltage1 = A0;
float Current1 = A1;

float I1 = 0;
int V1 = 0;

int out1 = 8;
int buz = 13;

double energyCount;
double powerCount;
double sum_inst_power;
double inst_power = 0;


#define addr_recharged_amount 0 
#define addr_remaining_units 4 
#define addr_total_consumption 8 
#define addr_last_recharge 12 
#define addr_monthly_consumption 16 
#define addr_energy_count 20 


float recharged_amount = 0.00f;
float remaining_units = 0.00f; 
float total_consumption = 0.00f; 
float last_recharge = 0.00f;
float monthly_consumption = 0.00f;;  
float averagePower = 0.00f;
float temp_recharged = 0.00f;
float supply_pf = 1; 
int mon_reset = 0; 

int power;
int energy;

int unt = 0;
int price = 0;
int pricePerUnit = 5;

int p1 = 0;
int p2 = 0;

int con1 = 0;
int con2 = 0;

int d = 1; 
int r = 1; 
int t = 1; 
long time_int = 0; 

int token;

const float frequencyPerKiloWatt = 0.8889; 

void setup(){ 
  Serial.begin(9600); 
  
  pinMode(Voltage1, INPUT);        
  pinMode(Current1, INPUT);   
 
  pinMode(out1, OUTPUT);
  pinMode(buz, OUTPUT); 
  
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(6,1);
  lcd.print("WELCOME");
  lcd.setCursor(5,2);
  lcd.print("Energy Meter");
  digitalWrite(buz, HIGH);

  writeEnergyDatas();
    
  Serial.print("AT\r\n");
  delay(500);
  Serial.print("AT+CMGF=1\r\n");
  delay(500);
  Serial.print("AT+CNMI=2,2,0,0,0\r\n"); 
  delay(500);
  sim1(); 
  delay(500);
  Serial.println("ON GSM");
  delay(500);
  Serial.write(byte(26));
  delay(500);
  Serial.println("AT+CMGD=1,4\r\n");
  delay(500);
  digitalWrite(buz, LOW);
  lcd.clear();
  readfromEEPROM(); 
} 
void loop(){ 
  long millisec = millis();
  long timeUp = millisec/1000;
  
  if(Serial.available() > 0){
    inchar = Serial.read(); 
    if (inchar == 'R'){
      delay(10);
      inchar = Serial.read(); 
      if (inchar == 'U'){
        delay(10);
        inchar = Serial.read();
        if (inchar == '1'){
          token = 100;
          sim1();
          delay(500);
          Serial.println("Recharge Meter by 100 TZS\r\n");
          delay(250);
          Serial.write(byte(26));
          delay(500);  
          generateReconSignal();
          price = price + 100;
          unt = price / pricePerUnit;
          remaining_units += 100/pricePerUnit;
          con1 = 0;
          con2 = 0;
        }
        else if (inchar == '2'){
          token = 200;
          sim1();
          delay(500);
          Serial.println("Recharge Meter by 200 TZS\r\n");
          delay(250);
          Serial.write(byte(26));
          delay(500);     
          generateReconSignal();
          price = price + 200;
          unt = price / pricePerUnit;
          remaining_units += 200/pricePerUnit;
          con1 = 0;
          con2 = 0;
        }
        else if (inchar == '3'){
          token = 300;
          sim1();
          delay(500);
          Serial.println("Recharge Meter by 300 TZS\r\n");
          delay(250);
          Serial.write(byte(26));
          delay(500);   
          generateReconSignal();
          price = price + 300;
          unt = price / pricePerUnit;
          remaining_units += 300/pricePerUnit;
          con1=0;
          con2=0;
        }
        else if (inchar=='4'){
          token = 400;
          sim1();
          delay(500);
          Serial.println("Recharge Meter by 400 TZS\r\n");
          delay(250);
          Serial.write(byte(26));
          delay(500);  
          generateReconSignal();
          price = price + 400;
          unt = price / pricePerUnit;
          remaining_units += 400/pricePerUnit;
          con1=0;
          con2=0;
        }
        else if (inchar == '5'){
          token = 500;
          sim1();
          delay(500);
          Serial.println("Recharge Meter by 500 TZS\r\n");
          delay(250);
          Serial.write(byte(26));
          delay(500);    
          generateReconSignal();
          price = price + 500;
          unt = price / pricePerUnit;
          remaining_units += 500/pricePerUnit;
          con1 = 0;
          con2 = 0;
        }
        else if (inchar== 'D'){
          sim1();
          delay(500);
          Serial.print("TOTAL UNITS=");
          Serial.println(unt);
          Serial.print("PAID PRICE=");
          Serial.print(price);
          Serial.println("TZS");
          Serial.print("UNITS REMAINED=");
          Serial.println(remaining_units);
          Serial.println("\r\n");
          delay(500);
          Serial.write(byte(26));
          delay(500);    
          con1 = 0;
          con2 = 0;
        }
        delay(10);
        Serial.println("AT+CMGD=1\r\n");
      }
    }
  }
  if(remaining_units <= 10){
    if(con1 ==0) {
      digitalWrite(buz, HIGH);
      sim1();
      delay(250);
      Serial.println("Recharge Your Meter\r\n");
      delay(250);
      Serial.write(byte(26));
      delay(250);
    }
    con1 = 1;
  }
  if(unt == 0) {
    unt = 0;
    price = 0;
    digitalWrite(buz, HIGH);
    generateDiconSignal();
  }
  if(remaining_units == 0) {
    digitalWrite(buz, HIGH);
    generateDiconSignal();
    if(con2 == 0) {
      sim1();
      delay(500);
      Serial.println("Unit Finished \r\n");
      delay(250);
      Serial.write(byte(26));
      delay(2500);
      digitalWrite(buz, LOW);
    }
    con2 = 1;
  } 
  float percRemain = (remaining_units/(token/pricePerUnit))*100;
  if (percRemain == 50 ){
    Serial.print("You have used 50% of your units. Remain ");
    Serial.print(remaining_units);
    Serial.println(" Units\r\n");  
  }
  if (percRemain == 25 ){
    Serial.print("You have used 75% of your units. Remain ");
    Serial.print(remaining_units);
    Serial.println(" Units\r\n");  
  }
  
  for (int n = 0; n < 1000; n++){
    V1 = ((analogRead(Voltage1) * (4.5/1023) * 60));
    I1 = ((analogRead(Current1) * (4.5/1023) * 0.4545));
    inst_power = V1 * I1; 
    sum_inst_power = sum_inst_power + abs(inst_power);
    delay(0.05);
  }
  
  powerCount = sum_inst_power/1000.0;
  energyCount = (powerCount * timeUp)/3600;

  time_check();
  reconnection();
  energyCalculation();
  displayEnergyDatas();
  writeEnergyDatas();
}

void sim1(){
  Serial.println("AT+CMGS=\"+255677747688\"\r\n"); 
}
void readfromEEPROM(){ 
  EEPROM.get(addr_recharged_amount, recharged_amount); 
  delay(100); 
  EEPROM.get(addr_remaining_units, remaining_units);
  delay(100); 
  EEPROM.get(addr_total_consumption, total_consumption); 
  delay(100); 
  EEPROM.get(addr_last_recharge, monthly_consumption); 
  delay(100); 
  EEPROM.get(addr_monthly_consumption, last_recharge);
  delay(100); 
  EEPROM.get(addr_energy_count, energyCount); 
  delay(100); 
  if(remaining_units > 1){
    generateReconSignal();
  }
}  
void generateReconSignal(){ 
  digitalWrite(out1, HIGH);
} 
void generateDiconSignal(){ 
  digitalWrite(out1, LOW);
} 
void displayEnergyDatas(){
  lcd.setCursor(5,0);
  lcd.print("ENERGY METER");
  delay(10); 
  lcd.setCursor(0,1);
  lcd.print("U="); 
  lcd.print(remaining_units);
  delay(10); 
  lcd.setCursor(0,3);
  lcd.print("Consumption="); 
  lcd.print(total_consumption);
  lcd.print("KWh"); 
  delay(10); 
  lcd.setCursor(0,4);  
  delay(10); 
  lcd.setCursor(11,4); 
  delay(10); 
}
void reconnection(){ 
  if(recharged_amount >= 6 && r == 1 ){ 
     r = 0; 
     d = 1; 
     generateReconSignal(); 
  } 
} 

void energyCalculation(){
   if(recharged_amount >= 6.00){ 
      remaining_units += recharged_amount ; 
      last_recharge = recharged_amount ; 
      temp_recharged = recharged_amount ; 
      recharged_amount = 0.00; 
    } 
  if (energyCount >= 1
      && remaining_units > 0
      && analogRead(Current1) > 0){ 
    energyCount = 0;
    monthly_consumption += 1; //0.1kWh = 1Wh 
    total_consumption += 1 ; //0.1kWh = 1Wh 
    remaining_units -= 1; //0.1kWh = 1Wh 
  }    
}   
void powerCalculation(){ 
  float averageFrequency = powerCount/(900); 
  averagePower = averageFrequency/frequencyPerKiloWatt ; 
  powerCount = 0; 
}  
void writeEnergyDatas(){ 
  EEPROM.put(addr_recharged_amount, recharged_amount); 
  delay(100); 
  EEPROM.put(addr_remaining_units, remaining_units); 
  delay(100); 
  EEPROM.put(addr_total_consumption, total_consumption); 
  delay(100); 
  EEPROM.put(addr_last_recharge, last_recharge);
  delay(100); 
  EEPROM.put(addr_monthly_consumption, monthly_consumption); 
  delay(100); 
  EEPROM.put(addr_energy_count, energyCount); 
  delay(100);
}
void month_reset(){ 
  if(mon_reset == 1){ 
    monthly_consumption = 0; 
  } 
}        
void time_check() { 
  if(millis() < 60000 && t == 1){
    time_int = 0; 
    t = 0; 
  } 
  if(millis() - time_int > 60000){ 
    time_int = millis(); 
    t = 1; 
    powerCalculation(); 
  } 
}
void writeEngDataGSM(){ 
  EEPROM.put(addr_recharged_amount, recharged_amount); 
  delay(100); 
  EEPROM.put(addr_monthly_consumption, monthly_consumption); 
  delay(100); 
}  
