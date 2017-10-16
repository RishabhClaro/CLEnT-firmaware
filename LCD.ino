void LCD_Initialize()         //initialise LCD
{
  LCDMenu = 1;
  LCD.begin(20, 4);
  LCD.createChar(0, LCDCharLoad);
  LCD.createChar(1, LCDCharSignal1);
  LCD.createChar(2, LCDCharSignal2);
  LCD.createChar(3, LCDCharSignal3);
  LCD.createChar(4, LCDCharSignal4);
  LCD.createChar(5, LCDCharSignal5);
  LCD.createChar(6, LCDCharNoSignal);
  LCD.createChar(7, LCDCharRupee);
}

void LCD_Load(byte Blocks)         //display loading bar
{
  LCD.setCursor(0, 3);
  LCD.print("                    ");
  LCD.setCursor(0, 3);
  for(byte i=0; i<Blocks; i++)
  {
    LCD.write(byte(0));
    delay(100);
  }
}

void LCD_Signal_Strength()         //display signal strength
{
  Serial.println("AT#QSS?");
  if(GSM_Check_SIM())
  {
    GSM_Signal_Strength();
    if((GSMSignalStrength > 0) && (GSMSignalStrength <= 7))
    {
      LCD.write(byte(1));
    }
    else if((GSMSignalStrength > 7) && (GSMSignalStrength <= 14))
    {
      LCD.write(byte(2));
    }
    else if((GSMSignalStrength > 14) && (GSMSignalStrength <= 21))
    {
      LCD.write(byte(3));
    }
    else if((GSMSignalStrength > 21) && (GSMSignalStrength <= 28))
    {
      LCD.write(byte(4));
    }
    else if((GSMSignalStrength > 28) && (GSMSignalStrength <= 31))
    {
      LCD.write(byte(5));
    }
    else
    {
      LCD.write(byte(6));
    }
  }
  else
  {
    LCD.write(byte(6));
  }
}

void LCD_Menu_Head()         //update the menu head
{
  //LCD.setCursor(0, 0);
 // LCD.print("CLARO              ");
  LCD.setCursor(0, 0);
 // LCD.setCursor(5, 0);
 // LCD.print(" ");
  LCD.setCursor(2, 0);
  LCD.print("/");
  LCD.setCursor(5, 0);
  LCD.print("/");
  LCD.setCursor(11, 0);
  LCD.print(":");
  //LCD.setCursor(14, 0);
  //LCD.print(":");
  LCD.setCursor(0, 0);
 if(RTC_Day() < 10)
  {
    LCD.print("0");
    LCD.print(RTC_Day());
  }
  else
  {
    LCD.print(RTC_Day());
  }
 // LCD.print(RTC_Day());
  
  LCD.setCursor(3, 0);
  if(RTC_Month() < 10)
  {
    LCD.print("0");
    LCD.print(RTC_Month());
  }
  else
  {
    LCD.print(RTC_Month());
  }
  //LCD.print(RTC_Month());
  
  LCD.setCursor(6, 0);
 /* if(RTC_Year() < 10)
  {
    LCD.print("0");
    LCD.print(RTC_Year());
  }
  else
  {
    LCD.print(RTC_Year());
  }*/
  
  LCD.print(tmYearToCalendar(tm.Year)-2000);
  
  LCD.setCursor(9, 0);
    if(RTC_Hour()<10)
    {
    LCD.print('0'); 
    LCD.print(RTC_Hour()); 
    }
   else
    {
      LCD.print(RTC_Hour());
    }
 // LCD.print(RTC_Hour());
  
  LCD.setCursor(12, 0);
  
  if(RTC_Minute() < 10)
  {
    LCD.print("0");
    LCD.print(RTC_Minute());
  }
  else
  {
    LCD.print(RTC_Minute());
  }
 // LCD.print(RTC_Minute());

  LCD.setCursor(15,0);

  if(RTC_Second() < 10)
  {
    LCD.print("0");
    LCD.print(RTC_Second());
  }
  else
  {
    LCD.print(RTC_Second());
  }
  
 // LCD.print(RTC_Second());
  
  if(GSM_Check_Network())
  {
    LCD.setCursor(18, 0);
    LCD_Signal_Strength();    
  }
  else
  {
    LCD.setCursor(18, 0);
    LCD.write(byte(6));
  }
}

void LCD_Clear_Except_Menu_Head()         //clear the display except the menu head
{
  LCD.setCursor(0, 1);
  LCD.print("                    ");
  LCD.setCursor(0, 2);
  LCD.print("                    ");
  LCD.setCursor(0, 3);
  LCD.print("                    ");
}

void LCD_Menu()         //displays different menus
{
  if(LCDMenu == 1)         //menu 1
  {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("                    ");
    LCD.setCursor(0, 1);
    LCD.print("   CLARO   ENERGY   ");
    LCD.setCursor(0, 2);
    LCD.print("  PRIVATE  LIMITED  ");
    LCD.setCursor(0, 3);
    LCD.print("                    ");
    delay(10000);
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print(" REMOTE  MONITORING ");
    LCD.setCursor(0, 1);
    LCD.print("CONTROL AND RECHARGE");
    LCD.setCursor(0, 2);
    LCD.print("       SYSTEM       ");
    LCD.setCursor(0, 3);
    LCD.print("    Version  2.0    ");
  }
  else if(LCDMenu == 2)         //menu 2
  {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("                    ");
    LCD.setCursor(0, 1);
    LCD.print("Initialising  System");
    LCD.setCursor(0, 2);
    LCD.print("                    ");
    LCD.setCursor(0, 3);
    LCD.print("                    ");
  }
  else if(LCDMenu == 3)         //menu 3
  {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print(" System Initialised ");
    LCD.setCursor(0, 1);
    LCD.print("                    ");
    LCD.setCursor(0, 2);
    LCD.print("   Network Status   ");
    LCD.setCursor(0, 3);
    if(GSMSetupAPN)
    {
      LCD.print("Connected To Network");
      Serial.println("AT#NITZ?");
      Serial.println("AT&P0");
      Serial.println("AT&W0");
      for(int i=1;i<=3;i++)
{

Serial.println("AT+CCLK?");
delay(500);
get_Data();
parsing();
delay(500);
LCD.clear();
LCD.print("taking time...");
}
  
  setTime(hours,minutes,seconds,days,months,years);
  RTC.set(now());
    
    }
    else
    {
      LCD.print("   Not  Connected   ");
    }
  }
}

void LCD_DashBoard1()         //display Main Dash Board
{
  LCD_Clear_Except_Menu_Head();
  if(RFID_Current_Balance())
  {
    LCD.setCursor(0, 1);
    RFID_Print_Current_Balance(1);
  }
  else
  {
    LCD.setCursor(0, 1);
    LCD.print("   Unable To Read   ");
  }
  LCD.setCursor(0, 2);
  LCD.print("  Start   Recharge  ");
  LCD.setCursor(0, 3);
  LCD.print("     Exit(Stop)     ");
  LCDDashBoard1 = true;
  LCDDashBoard3 = false;
}

void LCD_DashBoard2()         //display Pump Operations Dash Board
{
  if(StartPump)         //if pump started
  {
    LCD.setCursor(0, 1);
    LCD.print("    Pump Started    ");
    LCD.setCursor(0, 3);
    LCD.print("    Pause   Stop    ");
    LCDDashBoard1 = false;
    LCDDashBoard3 = false;
  }
  else if(PausePump)         //if pump paused
  {
    LCD.setCursor(0, 1);
    LCD.print("    Pump  Paused    ");
    LCD.setCursor(0, 3);
    LCD.print("    Start   Stop    ");
    LCDDashBoard1 = false;
    LCDDashBoard3 = false;
  }
  else if(StopPump)         //if pump stopped
  {
    LCD.setCursor(0, 1);
    LCD.print("    Pump Stopped    ");
    LCD.setCursor(0, 2);
    LCD.print("    Please  Wait    ");
    LCD.setCursor(0, 3);
    LCD.print("                    ");
    LCDDashBoard1 = false;
    LCDDashBoard3 = false;
  }
}

void LCD_DashBoard3()         //display Recharge Dash Board
{
  LCD_Clear_Except_Menu_Head();
  LCD.setCursor(0, 1);
  LCD.print("   Enter Pass Key   ");
  LCDDashBoard1 = false;
  LCDDashBoard3 = true;
}

void LCD_DashBoard()         //Dash Board after inserting authenticated card
{
  DashBoard = 1;
  LCDDashBoard1 = false;
  LCDDashBoard3 = false;
  RFIDLastTime = millis();
  if(!RFID_Check_Match())         //if card not inserted or changed then return
  {
    DashBoard = 0;
    return;
  }
  LCD_Menu_Head();
  while(1)
  {
    if(DashBoard == 1)          //Main DashBoard
    {
      if(ExitDashBoard)         //if exit button pressed
      {
        LCD_Clear_Except_Menu_Head();
        LCD.setCursor(0, 2);
        LCD.print(" Please Remove Card ");
        while(RFID_Check());         //waits while card is removed
        DashBoard = 0;
        ExitDashBoard = false;
        return;
      }
      if(!LCDDashBoard1)         //display Main Dash Board if not displayed yet
      {
        LCD_DashBoard1();
      }
      LCD_Menu_Head();
      if(millis() - RFIDLastTime >= RFIDCheckTime)         //check card present or not for every RFIDCheckTime seconds
      {
        RFIDLastTime = millis();
        if(!RFID_Check_Match())
        {
          DashBoard = 0;
          return;
        }
      }
    }
    
    if(DashBoard == 2)          //Pump Operations Dash Board
    {
      LCD_Menu_Head();
      LCD_DashBoard2();          //display Pump Operations Dash Board
      if((ScheduleOn) && !RTC_Check_Schedule_Time())
      {
        StartPump = false;
        PausePump = false;
        StopPump = true;
      }
      if(!RELAYOn && StartPump && !PausePump && !StopPump)          //start pump if start button pressed
      {
        digitalWrite(RELAY, LOW);
        RELAYOn = true;
        if(!FLOWMETERReadFirstTime)
        {
          FLOWMETER_Current();
          if(FLOWMETERCurrent == 0.00)
          {
            digitalWrite(RELAY, HIGH);
            RELAYOn = false;
            StartPump = false;
            PausePump = false;
            StopPump = true;
          }
          FLOWMETERPast = FLOWMETERCurrent;
          FLOWMETERPastSent = FLOWMETERPast;
          FLOWMETERCurrentTime = millis();
          FLOWMETERReadFirstTime = true;
        }
      }
      else if(RELAYOn && !StartPump && PausePump && !StopPump)          //pause pump if pause button pressed
      {
        digitalWrite(RELAY, HIGH);
        RELAYOn = false;
      }
      else if(RELAYOn && !StartPump && !PausePump && StopPump)          //stop pump if stop button pressed
      {
        digitalWrite(RELAY, HIGH);
        RELAYOn = false;
      }
      else if(!RELAYOn && !StartPump && !PausePump && StopPump)          //stop pump if stop button pressed
      {
        digitalWrite(RELAY, HIGH);
        RELAYOn = false;
      }
      if(StopPump)          //if stop button pressed then display current balance and return
      {
        DashBoard = 0;
        GSM_Send_SD_Store_Display_Pump_Data();
        LCD.setCursor(0, 1);
        LCD.print("    Pump Stopped    ");
        LCD.setCursor(0, 2);
        LCD.print("  Current  Balance  ");
        LCD.setCursor(0, 3);
        RFID_Print_Current_Balance(3);
        delay(3000);
        LCD_Clear_Except_Menu_Head();
        LCD.setCursor(0, 2);
        LCD.print(" Please Remove Card ");
        while(RFID_Check());
        return;        
      }
      if(millis() - RFIDLastTime >= RFIDCheckTime)         //check card present or not for every RFIDCheckTime seconds and update balance to RFID according to flow meter value
      {
        RFIDLastTime = millis();
        if(!RFID_Check_Match_Pump())
        {
          DashBoard = 0;
          return;
        }
        if(RFID_Current_Balance())
        {
          FLOWMETER_Current();
          double RFIDDeductBalance = ((FLOWMETERCurrent - FLOWMETERPast)/PER_KILO_LITRES)*AMOUNT;
          if((RFIDCurrentBalance - RFIDDeductBalance) <= MIN_AMOUNT)
          {
            LCD.setCursor(0, 2);
            LCD.print("    Low  Balance    ");
            digitalWrite(RELAY, HIGH);
            RELAYOn = false;
            StartPump = false;
            PausePump = false;
            StopPump = true;
            GSM_Send_SD_Store_Display_Pump_Data();
            delay(3000);
            return;
          }
          else
          {
            FLOWMETERPast = FLOWMETERCurrent;
            if(RFID_Current_Balance())
            {
              RFIDCurrentBalance = RFIDCurrentBalance - RFIDDeductBalance;
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
                LCD.setCursor(0, 2);
                RFID_Print_Current_Balance(2);         //print updated balance
              }
            }
          }
        }
      }
      if(millis() - GSMLastTime >= GSMCheckTime)         //send transaction data to server for every RFIDCheckTime seconds or store in SD card if network not present and update balance to RFID according to flow meter value
      {
        GSMLastTime = millis();
        GSM_Send_SD_Store_Display_Pump_Data();
        GSM_Receive_Data();
      }
    }
    
    if(DashBoard == 3)          //Recharge Dash Board
    {
      if(!LCDDashBoard3)          //display Recharge Dash Board if not displayed yet
      {
        LCD_DashBoard3();
      }
      if(!RFID_Check_Match())          //check RFID is present or not
      {
        DashBoard = 0;
        return;
      }
      LCD_Menu_Head();
      LCD.setCursor(8, 2);
      char KEYPADTempKey;
      byte i = 0;
      String KEYPADTempPassKey = "";
      unsigned long CurrentTime = millis();
      while((i < 4) && ((millis() - CurrentTime) <= 30000))          //wait while Pass Key is pressed for 30 seconds
      {
        char KEYPADTempKey = KEYPAD.getKey();
        if(KEYPADTempKey != NO_KEY)
        {
          KEYPADTempPassKey = KEYPADTempPassKey + KEYPADTempKey;
          i++;
          LCD.print("*");
        }
      }
      if(KEYPADTempPassKey == KEYPADPassKey)           //if correct pass key entered
      {
        if(!RFID_Check_Match())
        {
          DashBoard = 0;
          return;
        }
        LCD_Menu_Head();
        LCD_Clear_Except_Menu_Head();
        LCD.setCursor(0, 1);
        LCD.print("    Enter Amount    ");
        LCD.setCursor(1, 2);
        LCD.write(byte(7));
        byte k = 0, t=0;
        byte RFIDTempRead[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        unsigned long CurrentTime = millis();
        char KEYPADTempKey = KEYPAD.getKey();
        while(!(((KEYPADTempKey != NO_KEY) && ((KEYPADTempKey == '1') || (KEYPADTempKey == '2') || (KEYPADTempKey == '3') || (KEYPADTempKey == '4') || (KEYPADTempKey == '5') || (
          KEYPADTempKey == '6') || (KEYPADTempKey == '7') || (KEYPADTempKey == '8') || (KEYPADTempKey == '9') || (KEYPADTempKey == '0'))) || !(millis() - CurrentTime <= 30000)))          //wait while balance is entered
        {
          KEYPADTempKey = KEYPAD.getKey();
        }
        for(k=0; ((k<16) && ((KEYPADTempKey != NO_KEY) && ((KEYPADTempKey == '1') || (KEYPADTempKey == '2') || (KEYPADTempKey == '3') || (KEYPADTempKey == '4') || (KEYPADTempKey == '5') 
        || (KEYPADTempKey == '6') || (KEYPADTempKey == '7') || (KEYPADTempKey == '8') || (KEYPADTempKey == '9') || (KEYPADTempKey == '0')))); k++)
        {
          RFIDTempRead[k] = byte(KEYPADTempKey - '0');
          LCD.print(KEYPADTempKey);
          CurrentTime = millis();
          KEYPADTempKey = KEYPAD.getKey();
          while(!(((KEYPADTempKey != NO_KEY) && ((KEYPADTempKey == '1') || (KEYPADTempKey == '2') || (KEYPADTempKey == '3') || (KEYPADTempKey == '4') || (KEYPADTempKey == '5') || (
            KEYPADTempKey == '6') || (KEYPADTempKey == '7') || (KEYPADTempKey == '8') || (KEYPADTempKey == '9') || (KEYPADTempKey == '0'))) || !(millis() - CurrentTime <= 1500)))
          {
            KEYPADTempKey = KEYPAD.getKey();
          }
        }
        RFIDEnteredBalance = 0;
        for(byte i=0; i<k; i++)
        {
          RFIDEnteredBalance = RFIDEnteredBalance + (Power(k-i-1) * RFIDTempRead[i]);
        }
        if(RFIDEnteredBalance > MAX_RECHARGE)          //if balance is greater than maximum recharge then go back
        {
          LCD_Clear_Except_Menu_Head();
          LCD.setCursor(0, 2);
          LCD.print("   Amount Entered   ");
          LCD.setCursor(0, 3);
          LCD.print("      Too High      ");
          delay(3000);
          DashBoard = 1;
        }
        else if(RFID_Check_Match())          //if balance is less than or equal to maximum recharge
        {
          LCD_Menu_Head();
          LCD_Clear_Except_Menu_Head();
          LCD.setCursor(0, 1);
          LCD.print("   Amount Entered   ");          //display entered amount
          LCD.setCursor(0, 2);
          LCD.print("       ");
          LCD.write(byte(7));
          LCD.print(String(RFIDEnteredBalance));
          LCD.setCursor(0, 3);
          LCD.print("1.Continue End(Stop)");
          KEYPADTempKey = KEYPAD.getKey();
          CurrentTime = millis();
          while(!(((KEYPADTempKey != NO_KEY) && (KEYPADTempKey == '1')) || !(millis() - CurrentTime <= 30000)))          //wait while option is pressed for 30 seconds
          {
            if(ExitRecharge)          //if exit button is pressed then go back
            {
              DashBoard = 1;
              ExitRecharge = false;
              break;
            }
            else
            {
              KEYPADTempKey = KEYPAD.getKey();
            }
          }
          if(KEYPADTempKey == '1')          //if CONTINUE option is pressed then add balance to RFID and send recharge data to server or store in SD card if connection not available
          {
            if(!RFID_Check_Match())
            {
              DashBoard = 0;
              return;
            }
            LCD_Menu_Head();
            LCD_Clear_Except_Menu_Head();
            if(RFID_Current_Balance())
            {
              double TotalBalance = RFIDEnteredBalance + RFIDCurrentBalance;
              unsigned long TempBalance = TotalBalance;
              unsigned long TempDecimalBalance = (TotalBalance - TempBalance)*100;
              k = 0;
              while(TempBalance)
              {
                TempBalance = TempBalance/10;
                k++;
              }
              for(byte i=0; i<(16-k); i++)
              {
                RFIDWriteBalance[i] = 0;
              }
              t = k;
              TempBalance = TotalBalance;
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
                LCD_Menu_Head();
                LCD.setCursor(0, 1);
                LCD.print("Recharge  Successful");
                LCD.setCursor(0, 2);
                LCD.print("  Current  Balance  ");
                if(RFID_Current_Balance())
                {
                  LCD.setCursor(0, 3);
                  RFID_Print_Current_Balance(3);          //display updated balance
                }
                else
                {
                  LCD.setCursor(0, 3);
                  LCD.print("   Unable To Read   ");
                }
                delay(3000);
                DashBoard = 1;
                LCD_Menu_Head();
                LCD_Clear_Except_Menu_Head();
                LCD.setCursor(0, 2);
                LCD.print("    Please  Wait    ");
                GSM_Set_Recharge_Data();
                if(!GSM_Send_Recharge_Data())
                {
                  SD_Store_Recharge_Data();
                }
                DashBoard = 1;
              }
              else
              {
                LCD.setCursor(0, 2);
                LCD.print(" Unable To Recharge ");
                LCD.setCursor(0, 3);
                LCD.print("     Try  Again     ");
                delay(3000);
                DashBoard = 1;
              }
            }
            else
            {
              LCD.setCursor(0, 2);
              LCD.print("   Unable To Read   ");
              LCD.setCursor(0, 3);
              LCD.print("  Current  Balance  ");
              delay(3000);
              DashBoard = 1;
            }
          }
          else          //if no option is pressed for 30 seconds
          {
            LCD_Clear_Except_Menu_Head();
            LCD.setCursor(0, 1);
            LCD.print(" Option Not Pressed ");
            LCD.setCursor(0, 2);
            LCD.print("         OR         ");
            LCD.setCursor(0, 3);
            LCD.print("    Exit Pressed    ");
            delay(3000);
            DashBoard = 1;
          }
        }
      }
      else          //if invalid Pass Key is pressed
      {
        LCD_Clear_Except_Menu_Head();
        LCD.setCursor(0, 2);
        LCD.print("  Invalid Pass Key  ");
        delay(3000);
        DashBoard = 1;
      }
    }
  }
}
