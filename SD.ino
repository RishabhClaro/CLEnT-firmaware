void SD_Initialize()          //initialise SD card
{
  digitalWrite(RFID_CS, HIGH);
  digitalWrite(SD_EN, HIGH);
  digitalWrite(SD_CS, LOW);

  delay(500);
  
  if(SD.begin(SD_CS))
  {
    digitalWrite(LED_SD, HIGH);
  }
  else
  {
    digitalWrite(LED_SD, LOW);
  }
}

byte RFID_Check_In_SD_Card()          //authenticate RFID card
{
  RFID_UID();
  SD_Initialize();
  SD_File = SD.open("RFID.txt", FILE_WRITE);
  SD_File.close();
  SD_File = SD.open("RFID.txt");
  if(SD_File)
  {
    digitalWrite(LED_SD, HIGH);
    String RFIDTempUID = "";
    char Temp;
    while(SD_File.available())
    {
      Temp = char(SD_File.read());
      while(Temp != '\r')
      {
        RFIDTempUID = RFIDTempUID + Temp;
        Temp = char(SD_File.read());
      }
      if(RFIDTempUID == RFIDUID)
      {
        SD_File.close();
        return 1;
      }
      SD_File.read();
      RFIDTempUID = "";
    }
    SD_File.close();
    return 2;
  }
  else
  {
    return 3;
  }
}

boolean RFID_Add_To_SD_Card()          //add new card into SD card
{
  SD_Initialize();
  SD_File = SD.open("RFID.txt", FILE_WRITE);
  if(SD_File)
  {
    delay(100);
    digitalWrite(LED_SD, HIGH);
    for(byte i=0; i<4; i++)
    {
      SD_File.print(RFIDReadUID[i], HEX);
    }
    SD_File.println();
    SD_File.close();
    return true;
  }
  else
  {
    return false;
  }
}

void SD_Store_Recharge_Data()         //store recharge data if no network
{
  SD_Initialize();
  if(!SD.exists(SDRechargeDirectory)) 
  {
    if(SD.mkdir(SDRechargeDirectory)) 
    {
      
    }
  }
  RTC_Date_Time_For_SD();
  String FileName = SDRechargeDirectory + "/" + RTCSDDate + RTCSDTime + ".txt";
  SD_File = SD.open(FileName, FILE_WRITE);
  if(SD_File)
  {
    delay(100);
    digitalWrite(LED_SD, HIGH);
    SD_File.println(GSMSendRechargeData);
    SD_File.close();
  }
}

void SD_Store_Pump_Data()         //store transaction data if no network
{
  SD_Initialize();
  if(!SD.exists(SDTransactionDirectory)) 
  {
    if(SD.mkdir(SDTransactionDirectory)) 
    {
      
    }
  }
  RTC_Date_Time_For_SD();
  String FileName = SDTransactionDirectory + "/" + RTCSDDate + RTCSDTime + ".txt";
  SD_File = SD.open(FileName, FILE_WRITE);
  if(SD_File)
  {
    delay(100);
    digitalWrite(LED_SD, HIGH);
    SD_File.println(GSMSendPumpData);
    SD_File.close();
  }
}

void SD_Store_Location_Data()
{
  SD_Initialize();
  if(!SD.exists(SDLocationDirectory)) 
  {
    if(SD.mkdir(SDLocationDirectory)) 
    {
      
    }
  }
  RTC_Date_Time_For_SD();
  String FileName = SDLocationDirectory + "/" + RTCSDDate + RTCSDTime + ".txt";
  SD_File = SD.open(FileName, FILE_WRITE);
  if(SD_File)
  {
    delay(100);
    digitalWrite(LED_SD, HIGH);
    SD_File.println(GSMSendLocationData);
    SD_File.close();
  }
}
