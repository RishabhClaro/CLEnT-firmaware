/*
 *         CLARO ENERGY PRIVATE LIMITED
 *              Saket, New Delhi
 * ---------------------------------------------
 * Title      : Remote Monitoring Control and Recharge System
 * Author     : Sai Venkata Manohar V, NIT Delhi
 * E-Mail     : saimanohar.venkata@gmail.com
 * Phone      : +91 8130129479
 * Date       : May, 2017
 * Version    : 2.0
 * Target MCU : Atmel ATmega 2560
 */

//Include Libraries
#include <SoftwareSerial.h>         //Software Serial
#include <Wire.h>         //I2C
#include <SPI.h>         //SPI
#include "MFRC522.h"         //RFID
#include "SD.h"         //SD
//#include "RTClib.h"         //RTC
#include "LiquidCrystal.h"         //LCD
#include "Key.h"         //Keypad
#include "Keypad.h"         //Keypad
#include "SimpleModbusMaster.h"         //Flow Meter
#include "DHT.h"          //Temperature and Humidity
#include "Oversample.h"
#include <EEPROM.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

//Define Pins
//For GSM
#define GSM_RX 0
#define GSM_TX 1
//For RFID
#define RFID_RST 25
#define RFID_MISO 50
#define RFID_MOSI 51
#define RFID_SCK 52
#define RFID_CS 27
//For SD
#define SD_EN 23
#define SD_MISO 50
#define SD_MOSI 51
#define SD_SCK 52
#define SD_CS 53
//For RTC
#define RTC_SDA 20
#define RTC_SCL 21
//For LCD
#define LCD_RS 8
#define LCD_EN 9
#define LCD_D4 10
#define LCD_D5 11
#define LCD_D6 12
#define LCD_D7 13
//For KEYPAD
#define KEYPAD_R0 22
#define KEYPAD_R1 24
#define KEYPAD_R2 26
#define KEYPAD_R3 28
#define KEYPAD_C0 30
#define KEYPAD_C1 32
#define KEYPAD_C2 34
//For CURRENT SENSOR
#define CURRENT_SENSOR A0
//For VOLTAGE SENSOR
#define VOLTAGE_SENSOR A1
//For FLOW METER
#define FLOW_METER_EN 29
#define FLOW_METER_RX 17
#define FLOW_METER_TX 16
//For RELAY
#define RELAY 36
//For GPS
#define GPS_RX 15
#define GPS_TX 14
//For HUMIDITY AND TEMPERATURE SENSOR
#define HUMIDITY_AND_TEMPERATURE_SENSOR 33
#define DHTTYPE DHT11
DHT dht(HUMIDITY_AND_TEMPERATURE_SENSOR, DHTTYPE);
uint32_t delayMS;
//For LED
#define LED_GSM_NETWORK 4
#define LED_GSM_SEND_RECEIVE 5
#define LED_SD 6
#define LED_RTC 7
#define LED_GPS 31
//For INTERRUPTS
#define INTERRUPT_START 19
#define INTERRUPT_PULSE 18
#define INTERRUPT_STOP 2
#define INTERRUPT_RECHARGE 3

//Create Objects and Global Variables
//For GSM
String IMEI = "359855071513901";         //IMEI
boolean GSMSetupNetwork = false;         //If SIM is there or not
boolean GSMSetupAPN = false;          //if APN is set or not
byte GSMSignalStrength = 0;         //Signal Strength
String GSMSendRechargeData = "";         //Recharge String
String GSMSendPumpData = "";         //Transaction String
String GSMSendLocationData = "";          //Location String
unsigned long GSMCheckTime = 300000;         //Time to send new data to server (in MilliSeconds)
unsigned long GSMLocationTime = (GSMCheckTime / 2);         //Time to send new location data to server (in MilliSeconds)
unsigned long GSMLastTime = 0;         //Last time GSM sent new data to server
unsigned long GSMLastLocationTime = 0;         //Last time GSM sent new location data to server
byte PumpStatus = 2;          //Status of the pump (1-Start 2-Pause 0-Stop)
//SoftwareSerial GSM_Serial(GSM_RX, GSM_TX);
//For RFID
#define MAX_RECHARGE 9999         //Maximum recharge that can be done once
MFRC522 RFID(RFID_CS, RFID_RST);         //RFID object
MFRC522::MIFARE_Key RFIDKey;         //RFID key object
MFRC522::MIFARE_Key key;
MFRC522::MIFARE_Key keyA;
MFRC522::MIFARE_Key keyB;
byte RFIDReadUID[4];         //RFID UID
double RFIDCurrentBalance = 0;         //Current Balance in RFID including Current Decimal Balance
double RFIDCurrentDecimalBalance = 0;         //Only Current Decimal Balance in RFID
long RFIDEnteredBalance = 0;         //Entered Balance during recharge
#define SectorName 11
#define SectorName2 12
#define SectorUID 13
#define SectorBalance 14
#define SectorBalanceDecimal 15
#define BlockName (SectorName)*4
#define BlockName2 (SectorName2)*4
#define BlockUID (SectorUID)*4
#define BlockBalance (SectorBalance)*4
#define BlockBalanceDecimal (SectorBalanceDecimal)*4
const byte RFIDBlockNumber = (SectorBalance)*4;         //Block number where the balance is written into RFID
const byte RFIDDecimalBlockNumber = (SectorBalanceDecimal)*4;         //Block number where the decimal balance is written into RFID
byte RFIDReadBalance[18];         //Array to read balance
byte RFIDReadDecimalBalance[18];         //Array to read decimal balance
byte RFIDWriteBalance[17];         //Array to write balance
byte RFIDWriteDecimalBalance[17];         //Array to write decimal balance
byte RFIDReadEncryptedUID[18];
String RFIDEncryptedUID = "";
#define RFIDCheckTime 10000         //Time to check RFID (in MilliSeconds)
unsigned long RFIDLastTime = 0;         //Last time RFID was checked
boolean RFIDCheckedIn = false;          //if card is present or not
String RFIDUID = "";         //RFID UID in string
#define MIN_AMOUNT 5          //Minimum amount that has to be there in the RFID (in Rupees)
double AMOUNT = 3.00;          //Amount to be reduced (in Rupees)
#define PER_KILO_LITRES 1.0         //Amount to be reduced per these number of kilo litres (in Kilo Litres)
//For SD
File SD_File;
String SDRechargeDirectory = "RECHARGE";         //Recharge directory after root
String SDTransactionDirectory = "TRANSACT";         //Transaction directory after root
String SDLocationDirectory = "LOCATION";
//For RTC
//RTC_DS1307 RTC;         //RTC object
String RTCDate = "";         //Date in string
String RTCTime = "";         //Time in string
String RTCSDDate = "";         //Date in string for SD card
String RTCSDTime = "";         //Time in string for SD card
int StartHour = 0;         //Hour to start while scheduling
int StartMinute = 0;         //Minute to start while scheduling
int StopHour = 0;         //Hour to stop while scheduling
int StopMinute = 0;         //Minute to stop while scheduling
boolean ScheduleOn = false;         //if scheduling is there or not

tmElements_t tm;
String test_string,yr,mnth,da,mins,hr,sec;
int years,months,days,minutes,seconds,hours;
char character;
bool flag=false;

//For LCD
byte LCDMenu = 0;         //Controls display while system is initialising
byte LCDCharLoad[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B00000};         //Custom Character for loading bar
byte LCDCharSignal1[8] = {B00000, B00000, B00000, B00000, B10000, B10000, B10000, B00000};         //Custom Character for Signal1
byte LCDCharSignal2[8] = {B00000, B00000, B00000, B01000, B11000, B11000, B11000, B00000};         //Custom Character for Signal2
byte LCDCharSignal3[8] = {B00000, B00000, B00100, B01100, B11100, B11100, B11100, B00000};         //Custom Character for Signal3
byte LCDCharSignal4[8] = {B00000, B00010, B00110, B01110, B11110, B11110, B11110, B00000};         //Custom Character for Signal4
byte LCDCharSignal5[8] = {B00001, B00011, B00111, B01111, B11111, B11111, B11111, B00000};         //Custom Character for Signal5
byte LCDCharNoSignal[8] = {B10100, B01000, B10101, B00011, B00101, B01001, B11111, B00000};         //Custom Character for No Signal
byte LCDCharRupee[8] = {B11111, B11100, B00010, B11111, B00100, B01000, B00100, B00000};         //Custom Character for Rupee
LiquidCrystal LCD(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);         //LCD object
boolean LCDDashBoard1 = false;         //Main Dashboard
boolean LCDDashBoard3 = false;         //Recharge Dashboard
//For KEYPAD
const byte KEYPADRows = 4;         //no. of rows
const byte KEYPADCols = 3;         //no. of columns
byte KEYPADRowPins[KEYPADRows] = {KEYPAD_R0, KEYPAD_R1, KEYPAD_R2, KEYPAD_R3};         //holds all rows
byte KEYPADColPins[KEYPADRows] = {KEYPAD_C0, KEYPAD_C1, KEYPAD_C2};         //holds all columns
char KEYPADKey[KEYPADRows][KEYPADCols] = {         //Keypad characters
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
Keypad KEYPAD = Keypad(makeKeymap(KEYPADKey), KEYPADRowPins, KEYPADColPins, KEYPADRows, KEYPADCols);         //Keypad characters
String KEYPADPassKey = "1234";         //Pass Key for Recharge
//For SENSORS
#define R1 99500.0         //R1 for voltage sensor (in Ohms)
#define R2 750.0         //R2 for voltage sensor (in Ohms)
#define SENSORS_SAMPLES 10         //no. of samples to be taken at once
float VoltageDividerAnalogInput = 0.0;         //voltage analog input value
float VoltageValue = 0.0;         //final calculated volatge value
float CurrentSensorAnalogInput = 0;         //current analog input value
float CurrentValue = 0.0;         //final calculated current value
float TemperatureValue = 0.0;         //final calculated temperature value
float HumidityValue = 0.0;         //final calculated humidity value
float AVCC = 0.0;         //AVCC
byte analogPin1 = A0;
byte analogPin2 = A1;
byte Resolution = 16;
Oversample * Sampler1;
Oversample * Sampler2;

int pulse_count=0;

//For FLOW METER
#define baud 9600
#define timeout 1000
#define polling 200
#define retry_count 10
#define TOTAL_NO_OF_REGISTERS 2
String FLOWMETERData = "";
enum
{
  PACKET1,
  TOTAL_NO_OF_PACKETS
};
Packet packets[TOTAL_NO_OF_PACKETS];
unsigned int FLOWMETERRegs[TOTAL_NO_OF_REGISTERS];
unsigned long FLOWMETERCurrentTime = 0;         //Last time flow meter has given totalizer value
float FLOWMETERCurrent = 0;         //Current totalizer value
float FLOWMETERPast = 0;         //Previous totalizer value
float FLOWMETERPastSent = 0;          //Previous totalizer value that was used while sending to server
float FLOWMETERFlowRate = 0;          //Current flow rate in m^3/hr
boolean FLOWMETERReadFirstTime = false;         //to read flow meter value for first time
//For RELAY
boolean RELAYOn = false;         //Status of pump
//For GPS
String LatitudeValue = "";         //(Latitude*100)
String LongitudeValue = "";         //(Longitude*100)
unsigned long GPSCheckMaxTime = 2000;          //Check GPS for a maximum of GPSCheckMaxTime (in Milliseconds)
//For INTERRUPTS
boolean SendData = false;         //Send all the leftover data in the SD card
boolean StartPump = false;         //start pump
boolean PausePump = false;         //pause pump
boolean StopPump = true;         //stop pump
boolean ExitDashBoard = false;         //exit the main dashboard
boolean ExitRecharge = false;         //exit the recharge dashboard
byte DashBoard = 0;         //Dashboard Number (1-Main, 2-Pump operations, 3-Recharge)
//For EEPROM
int addr = 0;
long Delay = 0;
byte D[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int m = 0;

//Setup Function
void setup()
{
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
  
  LCD_Initialize();         //initialise LCD

  //MENU-1
  LCDMenu = 1;         //goto menu 1
  LCD_Menu();         //display menu 1
  delay(10000);

  //MENU-2
  LCDMenu = 2;         //goto menu 2
  LCD_Menu();   //display menu 2

 
  //Set all the output pins
  pinMode(RFID_CS, OUTPUT);
  pinMode(SD_EN, OUTPUT);  
  pinMode(SD_CS, OUTPUT);
  pinMode(FLOW_METER_EN, OUTPUT);
  pinMode(LED_GSM_NETWORK, OUTPUT);
  pinMode(LED_GSM_SEND_RECEIVE, OUTPUT);
  pinMode(LED_SD, OUTPUT);
  pinMode(LED_RTC, OUTPUT);
  pinMode(LED_GPS, OUTPUT);
  //attach the interrupts
  pinMode(INTERRUPT_START, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_START), Start, FALLING);
  pinMode(INTERRUPT_PULSE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PULSE), Pulse, CHANGE);
  pinMode(INTERRUPT_STOP, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_STOP), Stop, FALLING);
  pinMode(INTERRUPT_RECHARGE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_RECHARGE), Recharge, FALLING);
  LCD_Load(2);         //load 2 bars
  GSM_Initialize();         //initialise GSM
  LCD_Load(4);         //load 4 bars
  GSM_Setup_Network();         //try to setup
  LCD_Load(6);         //load 6 bars
  SPI.begin();         //begin SPI
  RFID_Initialize();         //initialise RFID
  for(byte i=0; i<6; i++)         //set key to access RFID data
  {
    RFIDKey.keyByte[i] = 0xFF;
  }
  LCD_Load(8);         //load 8 bars
  SD_Initialize();         //initialise SD
  LCD_Load(10);         //load 10 bars
  //RTC_Initialize();         //initialise RTC
  LCD_Load(12);         //load 12 bars
  KEYPAD_Initialize();         //initialise keypad
  LCD_Load(14);         //load 14 bars
  Sampler1 = new Oversample(analogPin1, Resolution);
  Sampler2 = new Oversample(analogPin2, Resolution);
  Sampler1->getResolution();
  Sampler2->getResolution();
  Sampler1->getPrescaler();
  Sampler2->getPrescaler();
  SENSORS_Initialize();         //initialise sensors
  dht.begin();
  LCD_Load(16);         //load 16 bars
  FLOW_METER_Initialize();         //initialise flow meter
  LCD_Load(18);         //load 18 bars
  GPS_Initialize();         //initialise GPS
  LCD_Load(20);         //load 20 bars

  LCD.clear();         //clear full display
  LCD.setCursor(0, 0);
  LCD.print("                    ");
  LCD.setCursor(0, 1);
  LCD.print("  Checking  Memory  ");
  LCD.setCursor(0, 2);
  LCD.print("    Please  Wait    ");
  LCD.setCursor(0, 3);
  LCD.print("                    ");
  GSM_Send_SD_Stored_Pump_Data();         //sending all the transaction data left in SD card
  GSM_Send_SD_Stored_Recharge_Data();         //sending all the recharge data left in SD card
  GSM_Send_SD_Stored_Location_Data();         //sending all the location data left in SD card
  LCD.clear();
  LCD.setCursor(0, 2);
  LCD.print("     Data  Sent     ");         //all the data would have been sent if there was no disturbance in SD card and network connection
  delay(3000);
  
  LCD.clear();         //clear full display
  LCD.setCursor(0, 0);
  LCD.print("                    ");
  LCD.setCursor(0, 1);
  LCD.print("  Updating  Amount  ");
  LCD.setCursor(0, 2);
  LCD.print("    Please  Wait    ");
  LCD.setCursor(0, 3);
  LCD.print("                    ");
  GSM_Update_Amount();          //Updating amount per kilo litres
  if((AMOUNT < 0) || (AMOUNT > 10))
  {
    AMOUNT = 3.0;
  }
  LCD.clear();         //clear full display
  LCD.setCursor(0, 0);
  LCD.print("                    ");
  LCD.setCursor(0, 1);
  LCD.print("   Updated Amount   ");
  LCD.setCursor(0, 2);
  LCD.print("      ");
  LCD.write(byte(7));
  LCD.print(String(AMOUNT));
  LCD.print("/KL      ");
  LCD.setCursor(0, 3);
  LCD.print("                    ");
  delay(3000);

  LCD.clear();         //clear full display
  LCD.setCursor(0, 0);
  LCD.print("                    ");
  LCD.setCursor(0, 1);
  LCD.print("Checking  Scheduling");
  LCD.setCursor(0, 2);
  LCD.print("    Please  Wait    ");
  LCD.setCursor(0, 3);
  LCD.print("                    ");
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  Serial.println("AT#SGACT?");
  if(GSM_Check_SGACT() && GSMSetupAPN)
  {
    GSM_Receive_Schedule_Data();
    SD_Initialize();
    SD_File = SD.open("SCHEDULE.txt");
    SD.remove("SCHEDULE.txt");
    SD_File.close();
    SD_File = SD.open("SCHEDULE.txt", FILE_WRITE);
    if(SD_File)
    {
      delay(100);
      digitalWrite(LED_SD, HIGH);
      RTC_Date_Time_For_SD();
      SD_File.print(RTCSDDate);
      if(ScheduleOn)
      {
        SD_File.print("Y");
        if(StartHour < 10)
        {
          SD_File.print("0");
        }
        SD_File.print(String(StartHour));
        if(StartMinute < 10)
        {
          SD_File.print("0");
        }
        SD_File.print(String(StartMinute));
        if(StopHour < 10)
        {
          SD_File.print("0");
        }
        SD_File.print(String(StopHour));
        if(StopMinute < 10)
        {
          SD_File.print("0");
        }
        SD_File.println(String(StopMinute));
      }
      else
      {
        SD_File.println("N");
      }
      SD_File.close();
    }
    else
    {
      ScheduleOn = false;         //if unable to write into SD card after reading values from server
    }
  }
  else
  {
    SD_Initialize();
    SD_File = SD.open("SCHEDULE.txt");
    if(SD_File)
    {
      digitalWrite(LED_SD, HIGH);
      char Temp = char(SD_File.read());
      String TempDate = "";
      unsigned long TempTime = millis();
      while((Temp != 'Y') && (Temp != 'N') && (millis() - TempTime <= 5000))
      {
        TempDate = TempDate + Temp;
        Temp = char(SD_File.read());
      }
      RTC_Date_Time_For_SD();
      if(TempDate == RTCSDDate)
      {
        if(Temp == 'Y')
        {
          ScheduleOn = true;
          StartHour = 10*(char(SD_File.read()) - 48) + (char(SD_File.read()) - 48);
          StartMinute = 10*(char(SD_File.read()) - 48) + (char(SD_File.read()) - 48);
          StopHour = 10*(char(SD_File.read()) - 48) + (char(SD_File.read()) - 48);
          StopMinute = 10*(char(SD_File.read()) - 48) + (char(SD_File.read()) - 48);
        }
        else if(Temp == 'N')
        {
          ScheduleOn = false;
        }
      }
      else
      {
        ScheduleOn = false;
        SD.remove("SCHEDULE.txt");
        SD_File.close();
        SD_File = SD.open("SCHEDULE.txt", FILE_WRITE);
        if(SD_File)
        {
          delay(100);
          digitalWrite(LED_SD, HIGH);
          SD_File.print(RTCSDDate);
          SD_File.println("N");
          SD_File.close();
        }
      }
      SD_File.close();
    }
    else
    {
      ScheduleOn = false;         //if unable to read from SD card
    }
  }

  LCD.clear();         //clear full display
  LCD.setCursor(0, 0);
  LCD.print("                    ");
  LCD.setCursor(0, 1);
  LCD.print("   Updating Delay   ");
  LCD.setCursor(0, 2);
  LCD.print("    Please  Wait    ");
  LCD.setCursor(0, 3);
  LCD.print("                    ");
  GSM_Update_Delay();
  addr = 0;
  Delay = 0;
  for(int i=0; i<10; i++)
  {
    D[i] = 0;
  }
  m = 0;
  unsigned long EEPROMStart = millis();
  byte temp = EEPROM.read(addr++);
  while((temp!=35) && ((millis() - EEPROMStart) <= 5000))
  {
    D[m++] = temp;
    delay(500);
    temp = EEPROM.read(addr++);
  }
  for(byte x = 0; x<m; x++)
  {
    Delay = Delay + D[x]*Power(m-x-1);
  }
  if(Delay > 0)
  {
    GSMCheckTime = Delay * 1000;
    GSMLocationTime = (GSMCheckTime / 2);
  }
  
  //MENU-3
  LCDMenu = 3;         //goto menu 3
  LCD_Menu();  //display menu 3
  for(int i=0;i<=3;i++)  
{
if (RTC.read(tm)) {
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print("Time = ");
    print2digits(tm.Hour);
    LCD.print(':');
    print2digits(tm.Minute);
    LCD.print(':');
    print2digits(tm.Second);
    LCD.setCursor(0,2);
    LCD.print("Date = ");
    LCD.print(tm.Day);
    LCD.print('/');
    LCD.print(tm.Month);
    LCD.print('/');
    LCD.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);
}
  delay(1000);

  LCD.clear();         //clear full display
}

//Loop Function
void loop()
{
if(StartPump==true)
 {

if(flag==true)
  {
  pulse_count++;
  flag=false;
  delay(500);
  }

 } 
  RFIDCheckedIn = false;
  
  if(!SendData)         //if not sending left over data from SD card
  {
    LCD_Menu_Head();         //display the display head
    LCD_Clear_Except_Menu_Head();         //clear display except head
    LCD.setCursor(0, 2);
    if(!SendData)         //if not sending left over data from SD card
    {
      LCD.print("    Insert  Card    ");
    }
  }
  
  if((!ScheduleOn) || ((ScheduleOn) && RTC_Check_Schedule_Time()))         //if there is no scheduling (or) if there is scheduling then card can be scanned only in between that time
  {
    if(RFID_Check() && Serial_Send_Read_UID())         //if card inserted
    {
      RFIDCheckedIn = true;
      LCD_Clear_Except_Menu_Head();
      LCD.setCursor(0, 2);
      LCD.print("    Please  Wait    ");
      for (byte i = 0; i < 4; i++)
      { 
        RFIDReadUID[i] = RFID.uid.uidByte[i];         //read the RFID UID
      }
      
      if(RFID_Check_In_SD_Card() == 1)          //if card is authenticated
      {
        LCD_Clear_Except_Menu_Head();
        LCD.setCursor(0, 2);
        LCD.print(" Card Authenticated ");
        delay(3000);
        
        LCD_Menu_Head();
  
        LCD_Clear_Except_Menu_Head();
        LCD.setCursor(0, 1);
        LCD.print("      Welcome!      ");
        LCD.setCursor(0, 2);
        LCD.print("    Please  Wait    ");
        
        if(RFID_Check())         //check if card is present
        {
          //GSM_Calibrate_Balance_From_Server();
          LCD.setCursor(0, 2);
          LCD.print(" Available  Balance ");
          
          if(RFID_Current_Balance())         //if able to read current balance
          {
            LCD.setCursor(0, 3);
            RFID_Print_Current_Balance(3);         //display current balance
          }
          
          else         //if unable to read current balance
          {
            LCD.setCursor(0, 3);
            LCD.print("   Unable To Read   ");
          }
                    
          delay(3000);
          LCD_DashBoard();         //goto main dashboard
        }
        
        else         //if card is not inserted
        {
          LCD.setCursor(0, 2);
          LCD.print("      No  Card      ");
          delay(3000);
        }
      }
      
      else if(RFID_Check_In_SD_Card() == 2)         //if card is not authenticated
      {
        LCD_Clear_Except_Menu_Head();
        LCD.setCursor(0, 2);
        LCD.print(" Not  Authenticated ");
        LCD.setCursor(0, 3);
        LCD.print("1.Add Card    2.Exit");
        
        unsigned long CurrentTime = millis();
        char KEYPADTempKey;
        while(millis() - CurrentTime <= 30000)         //waiting for option to be pressed for 30 seconds
        {
          KEYPADTempKey = KEYPAD.getKey();
          if((KEYPADTempKey != NO_KEY) && ((KEYPADTempKey == '1') || (KEYPADTempKey == '2')))
          {
            break;
          }
        }
        
        if(KEYPADTempKey == '1')         //if ADD CARD is pressed
        {
          if(RFID_Check() && RFID_Match_Current_Past_UID())         //check and match present RFID with RFID that was inserted at the beginning
          {
            LCD_Clear_Except_Menu_Head();
            LCD.setCursor(0, 2);
            LCD.print("    Please  Wait    ");
            for (byte i = 0; i < 4; i++)
            {
              RFIDReadUID[i] = RFID.uid.uidByte[i];         //read RFID UID
            }
            
            if(RFID_Add_To_SD_Card())         //if card is enlisted to SD card successfully
            {
              LCD_Clear_Except_Menu_Head();
              LCD.setCursor(0, 2);
              LCD.print("     Card Added     ");
              delay(3000);
            }
            
            else         //if card is not enlisted to SD card successfully
            {
              LCD_Clear_Except_Menu_Head();
              LCD.setCursor(0, 2);
              LCD.print(" Failed Adding Card ");
              delay(3000);
            }
          }
          else         //if card is changed or card is not inserted
          {
            LCD_Clear_Except_Menu_Head();
            LCD.setCursor(0, 1);
            LCD.print("      No  Card      ");
            LCD.setCursor(0, 2);
            LCD.print("         OR         ");
            LCD.setCursor(0, 3);
            LCD.print("    Card Changed    ");
            delay(3000);
          }
        }
        
        else if(KEYPADTempKey == '2')         //is EXIT is pressed
        {
          LCD_Clear_Except_Menu_Head();
          LCD.setCursor(0, 2);
          LCD.print(" Please Remove Card ");
          while(RFID_Check());         //waits while card is removed
        }
  
        else         //if no option is pressed for 30 seconds
        {
          LCD_Clear_Except_Menu_Head();
          LCD.setCursor(0, 2);
          LCD.print(" Option Not Pressed ");
          delay(3000);
        }
      }
      
      else if(RFID_Check_In_SD_Card() == 3)         //SD card is not working to check authentication
      {
        LCD_Clear_Except_Menu_Head();
        LCD.setCursor(0, 2);
        LCD.print("Authentication  Fail");
        delay(3000);
      }
    }
    FLOWMETERReadFirstTime = false;
  }
  
  else if((ScheduleOn) && !RTC_Check_Schedule_Time() && (RFID_Check()))          //if scheduling is on and card is inserted other than in that scheduling period
  {
    LCD_Clear_Except_Menu_Head();
    LCD.setCursor(0, 2);
    LCD.print("  System Scheduled  ");
    LCD.setCursor(0, 3);
    LCD.print("     Try  Later     ");
    delay(3000);
    LCD_Menu_Head();
    LCD_Clear_Except_Menu_Head();
    LCD.setCursor(0, 2);
    LCD.print("    Insert  Card    ");
  }
  
  if(SendData)         //if sending left over data from SD card
  {
    SendData = false;
    LCD.clear();         //clear full display
    LCD.setCursor(0, 0);
    LCD.print("                    ");
    LCD.setCursor(0, 1);
    LCD.print("  Checking  Memory  ");
    LCD.setCursor(0, 2);
    LCD.print("    Please  Wait    ");
    LCD.setCursor(0, 3);
    LCD.print("                    ");
    GSM_Send_SD_Stored_Pump_Data();         //sending all the transaction data left in SD card
    GSM_Send_SD_Stored_Recharge_Data();         //sending all the recharge data left in SD card
    GSM_Send_SD_Stored_Location_Data();         //sending all the location data left in SD card
    LCD_Clear_Except_Menu_Head();
    LCD_Menu_Head();
    LCD.setCursor(0, 2);
    LCD.print("     Data  Sent     ");         //all the data would have been sent if there was no disturbance in SD card and network connection
    delay(3000);
    LCD.setCursor(0, 2);
    LCD.print("    Insert  Card    ");
  }
  
  if((millis() - GSMLastLocationTime) >= GSMLocationTime)         //sending location of RMS
  {
    GSM_Send_Current_Location();
    GSMLastLocationTime = millis();
  }
}

void get_Data()
{
test_string="";
delay(1000); 
while(Serial.available()!=0)
{
character=Serial.read();
test_string=test_string + character;       
}
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    LCD.write('0');
  }
  LCD.print(number);
}

void parsing()
{
int index=test_string.indexOf(':');
String sub1=test_string.substring(index+3,index+20);
String sub2=sub1.substring(0,sub1.indexOf(','));
String sub3=sub1.substring(sub1.indexOf(',')+1,sub1.indexOf(',')+9);
yr=sub2.substring(0,2);
mnth=sub2.substring(3,5);
da=sub2.substring(6,8);
hr=sub3.substring(0,2);
mins=sub3.substring(3,5);
sec=sub3.substring(6,8);
years=yr.toInt();
months=mnth.toInt();
days=da.toInt();
hours=hr.toInt();
minutes=mins.toInt();
seconds=sec.toInt();
/*LCD.print(sub1);
delay(2000);
LCD.clear();
LCD.print(sub3);
delay(2000);
LCD.clear();
LCD.print(hours);
LCD.print("-");
LCD.print(minutes);
LCD.print("-");
LCD.print(seconds);
*/}

