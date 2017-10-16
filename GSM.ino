void GSM_Initialize()         //initialise GSM
{
  Serial.begin(9600);
}

void GSM_Get_Data(int Delay)         //receive data from GSM
{
  delay(Delay);
  while(Serial.available()!=0)
    Serial.read();
}

boolean GSM_Check_OK()
{
  String tempReceive = "";
  delay(300);
  while(Serial.available()!=0)
  {
    tempReceive = tempReceive + char(Serial.read());
  }
  if(tempReceive.indexOf("OK") != (-1))
  {
    return true;
  }
  return false;
}

boolean GSM_Check_READY()
{
  String tempReceive = "";
  delay(300);
  while(Serial.available()!=0)
  {
    tempReceive = tempReceive + char(Serial.read());
  }
  if(tempReceive.indexOf("READY") != (-1))
  {
    return true;
  }
  return false;
}

boolean GSM_Check_SIM()
{
  delay(300);
  int temp;
  while(Serial.available()!=0)
  {
    temp = Serial.read();
    if(temp == 44)
    {
      temp = Serial.read();
      if(temp == 49)
      {
        return true;
      }
    }
  }
  return false;
}

boolean GSM_Check_REGISTER()
{
  String tempReceive = "";
  delay(300);
  while(Serial.available()!=0)
  {
    char t = char(Serial.read());
    tempReceive = tempReceive + t;
  }
  if((tempReceive.indexOf("0,1") != (-1)) || (tempReceive.indexOf("0,5") != (-1)))
  {
    return true;
  }
  return false;
}

boolean GSM_Check_SGACT()
{
  String tempReceive = "";
  delay(300);
  while(Serial.available()!=0)
  {
    tempReceive = tempReceive + char(Serial.read());
  }
  if(tempReceive.indexOf("1,1") != (-1))
  {
    return true;
  }
  return false;
}

boolean GSM_Check_Network()         //check the network if present or not
{
  Serial.println("AT");
  if(GSM_Check_OK())
  {
    Serial.println("ATE0");
    if(GSM_Check_OK())
    {
      Serial.println("AT#SLED=2");
      GSM_Get_Data(200);
      Serial.println("AT#SLEDSAV");
      GSM_Get_Data(200);
        Serial.println("AT#QSS?");
        if(GSM_Check_SIM())
        {
          Serial.println("AT+CPIN?");
          if(GSM_Check_READY())
          {
            return true;
          }
          else
          {
            return false;
          }
        }
        else 
        {
          return false;
        }
    }
    else 
    {
      return false;
    }
  }
  else 
  {
    return false;
  }
}

/*void GSM_Get_IMEI()         //set the IMEI
{
  int i=0;
  long temp;
  IMEI = "";
  Serial.println("AT+CGSN");
  delay(300);
  while(Serial.available()!=0)
  { 
    i++;
    temp = Serial.read();
    if(i>11 && i<=26)
    {
      IMEI = IMEI + String(temp-48);
    }
  }
}*/

void GSM_Signal_Strength()         //sets the signal strength
{
  long temp1, temp2;
  GSMSignalStrength = 0;
  Serial.println("AT+CSQ");
  delay(300);
  while(Serial.available()!=0)
  {
    temp1 = Serial.read();
    if(temp1 == 58)
    {
      Serial.read();
      temp1 = Serial.read();
      temp2 = Serial.read();
      if(temp2 == 44)
      {
        GSMSignalStrength = (temp1 - 48);
      }
      else
      {
        GSMSignalStrength = ((temp1 - 48)*10) + (temp2 - 48);
      }
    }
  }
}

boolean GSM_Setup_APN()         //setup APN
{
  Serial.println("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"");
  if(GSM_Check_OK())
  {
    Serial.println("AT#SCFG=1,1,300,0,120,10");
    if(GSM_Check_OK())
    {
      Serial.println("AT#SCFGEXT=1,2,0,0,0,0");
      if(GSM_Check_OK())
      {
        Serial.println("AT#SGACT=1,0");
        delay(1800);
        if(GSM_Check_OK())
        {
          Serial.println("AT+CREG?");
          if(GSM_Check_REGISTER())
          {
            Serial.println("AT+CGREG?");
            if(GSM_Check_REGISTER())
            {
              String tempReceive = "";
              Serial.println("AT#SGACT=1,1");
              delay(3000);
              while(Serial.available()!=0)
              {
                char t = char(Serial.read());
                tempReceive = tempReceive + t;
              }
              delay(1000);
              if(tempReceive.indexOf("#SGACT") != (-1))
              {
                digitalWrite(LED_GSM_NETWORK, HIGH);
                GSMSetupAPN = true;
                return true;
              }
              else
              {
                digitalWrite(LED_GSM_NETWORK, LOW);
                GSMSetupAPN = false;
                return false;
              }
            }
            else
            {
              digitalWrite(LED_GSM_NETWORK, LOW);
              GSMSetupAPN = false;
              return false;
            }
          }
          else
          {
            digitalWrite(LED_GSM_NETWORK, LOW);
            GSMSetupAPN = false;
            return false;
          }
        }
        else
        {
          digitalWrite(LED_GSM_NETWORK, LOW);
          GSMSetupAPN = false;
          return false;
        }
      }
      else
      {
        digitalWrite(LED_GSM_NETWORK, LOW);
        GSMSetupAPN = false;
        return false;
      }
    }
    else
    {
      digitalWrite(LED_GSM_NETWORK, LOW);
      GSMSetupAPN = false;
      return false;
    }
  }
  else
  {
    digitalWrite(LED_GSM_NETWORK, LOW);
    GSMSetupAPN = false;
    return false;
  }
}

boolean GSM_Setup_Network()         //setup network and IMEI
{
  if(GSM_Check_Network())
  {
    GSMSetupNetwork = true;
    digitalWrite(LED_GSM_NETWORK, HIGH);
    GSM_Setup_APN();
    return true;
  }
  else
  {
    GSMSetupNetwork = false;
    digitalWrite(LED_GSM_NETWORK, LOW);
    return false;
  }
}

void GSM_Set_Pump_Data()         //set the transaction data into a string
{
  RTC_Date_Time();
  RFID_UID();
  GPS_Get_Lat_Long();
  SENSORS_Collect_Data();
  FLOWMETER_Flow_Rate();
  if(StartPump)
  {
    PumpStatus = 1;
  }
  else if(PausePump)
  {
    PumpStatus = 2;
  }
  else if(StopPump)
  {
    PumpStatus = 0;
  }
  GSMSendPumpData = "AT#HTTPQRY=0,0,\"/IAAS/Listener/Transaction.jsp?data=CLEnT|T|" + IMEI + "|" + RFIDEncryptedUID + "|" + RTCDate + "|" + RTCTime + "|" + String(LatitudeValue) + "|" + String(LongitudeValue) + "|" + String(RFIDCurrentBalance) + "|" + String(VoltageValue) + "|" + String(CurrentValue) + "|" + String(VoltageValue*CurrentValue) + "|" + String(TemperatureValue) + "|" + String(HumidityValue) + "|" + String(FLOWMETERCurrent) + "|" + String(FLOWMETERCurrent - FLOWMETERPastSent) + "|" + String(FLOWMETERFlowRate) + "|" + String(PumpStatus) + "\"";
}

boolean GSM_Send_Pump_Data()         //send the transaction string to server
{
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  Serial.println("AT#SGACT?");
  if(GSM_Check_SGACT() && GSMSetupAPN)
  {
    digitalWrite(LED_GSM_SEND_RECEIVE, HIGH);
    Serial.println("AT#HTTPCFG=0,\"52.66.20.29\",80,0,,,0,120,1");
    GSM_Get_Data(3000);
    Serial.println(GSMSendPumpData);
    delay(4000);
    String tempReceive = "";
    while(Serial.available()!=0)
    {
      tempReceive = tempReceive + char(Serial.read());
    }
    Serial.println("AT#HTTPRCV=0");
    GSM_Get_Data(4000);
    digitalWrite(LED_GSM_SEND_RECEIVE, LOW);
    if(tempReceive.indexOf("200") != (-1))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    digitalWrite(LED_GSM_SEND_RECEIVE, LOW);
    return false;
  }
}

void GSM_Set_Recharge_Data()         //set the recharge data into a string
{
  RTC_Date_Time();
  RFID_UID();
  GPS_Get_Lat_Long();
  GSMSendRechargeData = "AT#HTTPQRY=0,0,\"/IAAS/Listener/Recharge.jsp?data=CLEnT|R|" + IMEI + "|" + RFIDEncryptedUID + "|" + RTCDate + "|" + RTCTime + "|" + String(LatitudeValue) + "|" + String(LongitudeValue) + "|" + String(RFIDCurrentBalance) + "|" + String(RFIDEnteredBalance) + "\"";
}

boolean GSM_Send_Recharge_Data()         //send the recharge string to server
{
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  Serial.println("AT#SGACT?");
  if(GSM_Check_SGACT() && GSMSetupAPN)
  {
    digitalWrite(LED_GSM_SEND_RECEIVE, HIGH);
    Serial.println("AT#HTTPCFG=0,\"52.66.20.29\",80,0,,,0,120,1");
    GSM_Get_Data(3000);
    Serial.println(GSMSendRechargeData);
    delay(4000);
    String tempReceive = "";
    while(Serial.available()!=0)
    {
      tempReceive = tempReceive + char(Serial.read());
    }
    Serial.println("AT#HTTPRCV=0");
    GSM_Get_Data(4000);
    digitalWrite(LED_GSM_SEND_RECEIVE, LOW);
    if(tempReceive.indexOf("200") != (-1))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    digitalWrite(LED_GSM_SEND_RECEIVE, LOW);
    return false;
  }
}

void GSM_Set_Location_Data()          //set the loaction data into a string
{
  RTC_Date_Time();
  GPS_Get_Lat_Long();
  GSMSendLocationData = "AT#HTTPQRY=0,0,\"/IAAS/Listener/Location.jsp?data=CLEnT|L|" + IMEI + "|" + RTCDate + "|" + RTCTime + "|" + String(LatitudeValue) + "|" + String(LongitudeValue) + "\"";
}

boolean GSM_Send_Location_Data()         //send the location string to server
{
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  Serial.println("AT#SGACT?");
  if(GSM_Check_SGACT() && GSMSetupAPN)
  {
    digitalWrite(LED_GSM_SEND_RECEIVE, HIGH);
    Serial.println("AT#HTTPCFG=0,\"52.66.20.29\",80,0,,,0,120,1");
    GSM_Get_Data(3000);
    Serial.println(GSMSendLocationData);
    delay(4000);
    String tempReceive = "";
    while(Serial.available()!=0)
    {
      tempReceive = tempReceive + char(Serial.read());
    }
    Serial.println("AT#HTTPRCV=0");
    GSM_Get_Data(4000);
    digitalWrite(LED_GSM_SEND_RECEIVE, LOW);
    if(tempReceive.indexOf("200") != (-1))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    digitalWrite(LED_GSM_SEND_RECEIVE, LOW);
    return false;
  }
}

void GSM_Send_SD_Stored_Recharge_Data()         //send the left over recharge data in the SD card to server
{
  SD_Initialize();
  String DirectoryName = SDRechargeDirectory;
  File Root = SD.open(DirectoryName);
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  while(GSMSetupAPN)
  {
    File Temp =  Root.openNextFile();
    if(!Temp)
    {
       return;
    }
    else
    {
      LCD.clear();
      LCD.setCursor(0, 1);
      LCD.print("  Sending Recharge  ");
      LCD.setCursor(0, 2);
      LCD.print("        Data        ");
      LCD.setCursor(0, 3);
      LCD.print("    Please  Wait    ");
    }
    String TempFileName = SDRechargeDirectory + "/" + Temp.name();
    SD_File = SD.open(TempFileName);
    if(SD_File)
    {
      digitalWrite(LED_SD, HIGH);
      if(SD_File.available())
      {
        char TempChar = SD_File.read();
        GSMSendRechargeData = "";
        do
        {
          GSMSendRechargeData = GSMSendRechargeData + TempChar;
          TempChar = SD_File.read();
        }
        while((TempChar != char(13)) && (SD_File.available()));
        if(GSM_Send_Recharge_Data())
        {
          SD.remove(TempFileName);
        }
        else
        {
          GSM_Setup_APN();
        }
      }
    }
    SD_File.close();
    Temp.close();
  }
}

void GSM_Send_SD_Stored_Pump_Data()         //send the left over transaction data in the SD card to server
{
  SD_Initialize();
  String DirectoryName = SDTransactionDirectory;
  File Root = SD.open(DirectoryName);
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  while(GSMSetupAPN)
  {
    File Temp =  Root.openNextFile();
    if(!Temp)
    {
       return;
    }
    else
    {
      LCD.clear();
      LCD.setCursor(0, 1);
      LCD.print("Sending  Transaction");
      LCD.setCursor(0, 2);
      LCD.print("        Data        ");
      LCD.setCursor(0, 3);
      LCD.print("    Please  Wait    ");
    }
    String TempFileName = SDTransactionDirectory + "/" + Temp.name();
    SD_File = SD.open(TempFileName);
    if(SD_File)
    {
      digitalWrite(LED_SD, HIGH);
      if(SD_File.available())
      {
        char TempChar = SD_File.read();
        GSMSendPumpData = "";
        do
        {
          GSMSendPumpData = GSMSendPumpData + TempChar;
          TempChar = SD_File.read();
        }
        while((TempChar != char(13)) && (SD_File.available()));
        if(GSM_Send_Pump_Data())
        {
          SD.remove(TempFileName);
        }
        else
        {
          GSM_Setup_APN();
        }
      }
    }
    SD_File.close();
    Temp.close();
  }
}

void GSM_Send_SD_Stored_Location_Data()         //send the left over location data in the SD card to server
{
  SD_Initialize();
  String DirectoryName = SDLocationDirectory;
  File Root = SD.open(DirectoryName);
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  while(GSMSetupAPN)
  {
    File Temp =  Root.openNextFile();
    if(!Temp)
    {
       return;
    }
    else
    {
      LCD.clear();
      LCD.setCursor(0, 1);
      LCD.print("  Sending Location  ");
      LCD.setCursor(0, 2);
      LCD.print("        Data        ");
      LCD.setCursor(0, 3);
      LCD.print("    Please  Wait    ");
    }
    String TempFileName = SDLocationDirectory + "/" + Temp.name();
    SD_File = SD.open(TempFileName);
    if(SD_File)
    {
      digitalWrite(LED_SD, HIGH);
      if(SD_File.available())
      {
        char TempChar = SD_File.read();
        GSMSendLocationData = "";
        do
        {
          GSMSendLocationData = GSMSendLocationData + TempChar;
          TempChar = SD_File.read();
        }
        while((TempChar != char(13)) && (SD_File.available()));
        if(GSM_Send_Location_Data())
        {
          SD.remove(TempFileName);
        }
        else
        {
          GSM_Setup_APN();
        }
      }
    }
    SD_File.close();
    Temp.close();
  }
}

void GSM_Send_SD_Store_Display_Pump_Data()         //send the transaction data to server if network is available else store in SD card and update the balance in RFID
{
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  if(GSMSetupAPN)
  {
    FLOWMETER_Current();
    double RFIDDeductBalance = ((FLOWMETERCurrent - FLOWMETERPast)/PER_KILO_LITRES)*AMOUNT;
    if(RFID_Current_Balance())
    {
      RFIDCurrentBalance = RFIDCurrentBalance - RFIDDeductBalance;
      if(RFIDCurrentBalance <= MIN_AMOUNT)
      {
        StartPump = false;
        PausePump = false;
        StopPump = true;
        if(RFIDCurrentBalance <= 0)
        {
          RFIDCurrentBalance = 0;
        }
      }
      unsigned long TempBalance = RFIDCurrentBalance;
      unsigned long TempDecimalBalance = (RFIDCurrentBalance - TempBalance)*100;
      byte k = 0;
      while(TempBalance)
      {
        TempBalance = TempBalance/10;
        k++;
      }
      for(byte i=0; i<(16-k); i++)
      {
        RFIDWriteBalance[i] = 0;
      }
      byte t = k;
      TempBalance = RFIDCurrentBalance;
      for(byte i=16-k; i<16; i++)
      {
        RFIDWriteBalance[i] = TempBalance/Power(t-1);
        TempBalance = TempBalance - (RFIDWriteBalance[i] * Power(t-1));
        t--;
      }
      for(byte i=0; i<14; i++)
      {
        RFIDWriteDecimalBalance[i] = 0;
      }
      RFIDWriteDecimalBalance[14] = TempDecimalBalance/10;
      RFIDWriteDecimalBalance[15] = TempDecimalBalance%10;
      if(RFID_Update_Balance())
      {
        GSM_Set_Pump_Data();
        GSM_Send_Pump_Data();
        RFID_Check_Match_Pump();
      }
    }
    FLOWMETERPastSent = FLOWMETERCurrent;
    FLOWMETERPast = FLOWMETERCurrent;
  }
  else
  {
    FLOWMETER_Current();
    double RFIDDeductBalance = ((FLOWMETERCurrent - FLOWMETERPast)/PER_KILO_LITRES)*AMOUNT;
    if(RFID_Current_Balance())
    {
      RFIDCurrentBalance = RFIDCurrentBalance - RFIDDeductBalance;
      if(RFIDCurrentBalance <= MIN_AMOUNT)
      {
        StartPump = false;
        PausePump = false;
        StopPump = true;
        if(RFIDCurrentBalance <= 0)
        {
          RFIDCurrentBalance = 0;
        }
      }
      unsigned long TempBalance = RFIDCurrentBalance;
      unsigned long TempDecimalBalance = (RFIDCurrentBalance - TempBalance)*100;
      byte k = 0;
      while(TempBalance)
      {
        TempBalance = TempBalance/10;
        k++;
      }
      for(byte i=0; i<(16-k); i++)
      {
        RFIDWriteBalance[i] = 0;
      }
      byte t = k;
      TempBalance = RFIDCurrentBalance;
      for(byte i=16-k; i<16; i++)
      {
        RFIDWriteBalance[i] = TempBalance/Power(t-1);
        TempBalance = TempBalance - (RFIDWriteBalance[i] * Power(t-1));
        t--;
      }
      for(byte i=0; i<14; i++)
      {
        RFIDWriteDecimalBalance[i] = 0;
      }
      RFIDWriteDecimalBalance[14] = TempDecimalBalance/10;
      RFIDWriteDecimalBalance[15] = TempDecimalBalance%10;
      if(RFID_Update_Balance())
      {
        GSM_Set_Pump_Data();
        SD_Store_Pump_Data();
      }
    }
    FLOWMETERPastSent = FLOWMETERCurrent;
    FLOWMETERPast = FLOWMETERCurrent;
  }
}

void GSM_Send_Store_Pump_Data_No_Card()         //sends transaction data to server or stores in SD card if card is removed suddenly
{
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  if(GSMSetupAPN)
  {
    FLOWMETER_Current();
    GSM_Set_Pump_Data();
    if(!GSM_Send_Pump_Data())
    {
      SD_Store_Pump_Data();
    }
    FLOWMETERPastSent = FLOWMETERCurrent;
    FLOWMETERPast = FLOWMETERCurrent;
  }
  else
  {
    FLOWMETER_Current();
    GSM_Set_Pump_Data();
    SD_Store_Pump_Data();
    FLOWMETERPastSent = FLOWMETERCurrent;
    FLOWMETERPast = FLOWMETERCurrent;
  }
}

void GSM_Received_Extract_Data()         //extract the data received from server and change the status of the pump
{
  if(Serial.available()!=0)
  {
    char Temp = char(Serial.read());
    unsigned long GSMTempTime = millis();
    while((Temp != '_') && (millis() - GSMTempTime <= 5000))
    {
      Temp = char(Serial.read());
    }
    if(char(Serial.read()) == 'U')
    {
      Temp = char(Serial.read());
      if(Temp == '1')
      {
        if(!RELAYOn)
        {
          StartPump = true;
          PausePump = false;
          StopPump = false;
        }
      }
      else if(Temp == '0')
      {
        if(RELAYOn)
        {
          StartPump = false;
          PausePump = true;
          StopPump = false;
        }
      }
    }
  }
}

void GSM_Receive_Data()         //receive the status of the pump from the server
{
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  Serial.println("AT#SGACT?");
  if(GSM_Check_SGACT() && GSMSetupAPN)
  {
    digitalWrite(LED_GSM_SEND_RECEIVE, HIGH);
    String GSMSendData = "AT#HTTPQRY=0,0,\"/IAAS/Listener/Status.jsp?data=CLEnT|C|" + IMEI + "\"";
    Serial.println("AT#HTTPCFG=0,\"52.66.20.29\",80,0,,,0,120,1");
    GSM_Get_Data(3000);
    Serial.println(GSMSendData);
    GSM_Get_Data(4000);
    Serial.println("AT#HTTPRCV=0");
    delay(4000);
    GSM_Received_Extract_Data();
    digitalWrite(LED_GSM_SEND_RECEIVE, LOW);
  }
}

void GSM_Received_Extract_Update_Amount()
{
  String ReadAmount = "";
  char Temp;
  if(Serial.available()!=0)
  {
    Temp = char(Serial.read());
    unsigned long GSMTempTime = millis();
    while((Temp != '_')  && (millis() - GSMTempTime <= 5000))
    {
      Temp = char(Serial.read());
    }
    if(char(Serial.read()) == 'U')
    {
      Temp = char(Serial.read());
      GSMTempTime = millis();
      while((Temp != '\r')  && (millis() - GSMTempTime <= 5000))
      {
        ReadAmount = ReadAmount + Temp;
        Temp = char(Serial.read());
      }
    }
  }
  AMOUNT = 0.00;
  AMOUNT = (ReadAmount[0] - 48) + (ReadAmount[2] - 48)/10.0 + (ReadAmount[3] - 48)/100.0;
  if((AMOUNT < 0) || (AMOUNT >= 10))
  {
    SD_Initialize();
    SD_File = SD.open("AMOUNTUP.txt", FILE_WRITE);
    SD_File.close();
    SD_File = SD.open("AMOUNTUP.txt");
    if(SD_File)
    {
      if(SD_File.available())
      {
        String TempAmount = "";
        char Temp = char(SD_File.read());
        while(Temp != '\r')
        {
          TempAmount = TempAmount + Temp;
          Temp = char(SD_File.read());
        }
        AMOUNT = 0.00;
        AMOUNT = (TempAmount[0] - 48) + (TempAmount[2] - 48)/10.0 + (TempAmount[3] - 48)/100.0;
        if((AMOUNT < 0.00) || (AMOUNT > 10.00))
        {
          AMOUNT = 3.00;
        }
      }
      SD_File.close();
    }
  }
}

void GSM_Update_Amount()
{
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  Serial.println("AT#SGACT?");
  if(GSM_Check_SGACT() && GSMSetupAPN)
  {
    digitalWrite(LED_GSM_SEND_RECEIVE, HIGH);
    String GSMSendData = "AT#HTTPQRY=0,0,\"/IAAS/Listener/Amount.jsp?data=CLEnT|A|" + IMEI + "\"";
    Serial.println("AT#HTTPCFG=0,\"52.66.20.29\",80,0,,,0,120,1");
    GSM_Get_Data(3000);
    Serial.println(GSMSendData);
    GSM_Get_Data(4000);
    Serial.println("AT#HTTPRCV=0");
    delay(4000);
    GSM_Received_Extract_Update_Amount();
    digitalWrite(LED_GSM_SEND_RECEIVE, LOW);
    SD_Initialize();
    SD_File = SD.open("AMOUNTUP.txt", FILE_WRITE);
    SD.remove("AMOUNTUP.txt");
    SD_File.close();
    SD_File = SD.open("AMOUNTUP.txt", FILE_WRITE);
    if(SD_File)
    {
      delay(100);
      digitalWrite(LED_SD, HIGH);
      LCD.setCursor(0, 0);
      SD_File.println(String(AMOUNT));
    }
    SD_File.close();
  }
  else
  {
    SD_Initialize();
    SD_File = SD.open("AMOUNTUP.txt", FILE_WRITE);
    SD_File.close();
    SD_File = SD.open("AMOUNTUP.txt");
    if(SD_File)
    {
      digitalWrite(LED_SD, HIGH);
      if(SD_File.available())
      {
        String TempAmount = "";
        char Temp = char(SD_File.read());
        while(Temp != '\r')
        {
          TempAmount = TempAmount + Temp;
          Temp = char(SD_File.read());
        }
        AMOUNT = 0.00;
        AMOUNT = (TempAmount[0] - 48) + (TempAmount[2] - 48)/10.0 + (TempAmount[3] - 48)/100.0;
        if((AMOUNT < 0.00) || (AMOUNT > 10.00))
        {
          AMOUNT = 3.00;
        }
      }
      SD_File.close();
    }
  }
}

void GSM_Send_Current_Location()
{
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  if(GSMSetupAPN)
  {
    GSM_Set_Location_Data();
    if(!GSM_Send_Location_Data())
    {
      SD_Store_Location_Data();
      GSM_Setup_APN();
    }
  }
  else
  {
    GSM_Set_Location_Data();
    SD_Store_Location_Data();
  }
}

void GSM_Received_Extract_Schedule_Data()
{
  String ReadData = "";
  char Temp;
  if(Serial.available()!=0)
  {
    Temp = char(Serial.read());
    unsigned long GSMTempTime = millis();
    while((Temp != '_') && (millis() - GSMTempTime <= 5000))
    {
      Temp = char(Serial.read());
    }
    if(char(Serial.read()) == 'U')
    {
      Temp = char(Serial.read());
      if(Temp == 'Y')
      {
        ScheduleOn = true;
        StartHour = 10*(char(Serial.read()) - 48) + (char(Serial.read()) - 48);
        StartMinute = 10*(char(Serial.read()) - 48) + (char(Serial.read()) - 48);
        StopHour = 10*(char(Serial.read()) - 48) + (char(Serial.read()) - 48);
        StopMinute = 10*(char(Serial.read()) - 48) + (char(Serial.read()) - 48);
      }
      else
      {
        ScheduleOn = false;
      }
    }
    else
    {
      ScheduleOn = false;
    }
  }
}

void GSM_Receive_Schedule_Data()
{
  digitalWrite(LED_GSM_SEND_RECEIVE, HIGH);
  String GSMSendData = "AT#HTTPQRY=0,0,\"/IAAS/Listener/Schedule.jsp?data=CLEnT|S|" + IMEI + "\"";
  Serial.println("AT#HTTPCFG=0,\"52.66.20.29\",80,0,,,0,120,1");
  GSM_Get_Data(3000);
  Serial.println(GSMSendData);
  GSM_Get_Data(4000);
  Serial.println("AT#HTTPRCV=0");
  delay(4000);
  GSM_Received_Extract_Schedule_Data();
  digitalWrite(LED_GSM_SEND_RECEIVE, LOW);
}

void GSM_Received_Extract_Delay()
{
  if(Serial.available()!=0)
  {
    char Temp = char(Serial.read());
    unsigned long GSMTempTime = millis();
    while((Temp != '_') && (millis() - GSMTempTime <= 5000))
    {
      Temp = char(Serial.read());
    }
    addr = 0;
    if(char(Serial.read()) == 'U')
    {
      Temp = char(Serial.read());
      unsigned long GSMTempTime = millis();
      while((Temp != '_') && (millis() - GSMTempTime <= 5000))
      {
        EEPROM.write(addr++, (Temp - 48));
        delay(500);
        Temp = char(Serial.read());
      }
      EEPROM.write(addr++, '#');
      delay(500);
    }
  }
}

void GSM_Update_Delay()
{
  if(!GSMSetupAPN)
  {
    GSM_Setup_APN();
  }
  Serial.println("AT#SGACT?");
  if(GSM_Check_SGACT() && GSMSetupAPN)
  {
    digitalWrite(LED_GSM_SEND_RECEIVE, HIGH);
    String GSMSendData = "AT#HTTPQRY=0,0,\"/IAAS/Listener/Delay.jsp?data=CLEnT|D|" + IMEI + "\"";
    Serial.println("AT#HTTPCFG=0,\"52.66.20.29\",80,0,,,0,120,1");
    GSM_Get_Data(3000);
    Serial.println(GSMSendData);
    GSM_Get_Data(4000);
    Serial.println("AT#HTTPRCV=0");
    delay(4000);
    GSM_Received_Extract_Delay();
    digitalWrite(LED_GSM_SEND_RECEIVE, LOW);
  }
}
