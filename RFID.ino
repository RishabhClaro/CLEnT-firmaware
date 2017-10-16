void RFID_Initialize()          //initialise RFID
{  
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;  //default key
  }

  keyA.keyByte[0] = 0xFB;
  keyA.keyByte[1] = 0xFA;
  keyA.keyByte[2] = 0x1F;
  keyA.keyByte[3] = 0xF1;
  keyA.keyByte[4] = 0x23;
  keyA.keyByte[5] = 0x92;

  keyB.keyByte[0] = 0xFF;
  keyB.keyByte[1] = 0x95;
  keyB.keyByte[2] = 0x87;
  keyB.keyByte[3] = 0x98;
  keyB.keyByte[4] = 0x78;
  keyB.keyByte[5] = 0x45;
  
  digitalWrite(SD_EN, LOW);
  digitalWrite(SD_CS, HIGH);
  digitalWrite(RFID_CS, LOW);

  RFID.PICC_HaltA();
  RFID.PCD_StopCrypto1();
  delay(500);
  RFID.PCD_Init();
  RFID.PCD_SetAntennaGain(RFID.RxGain_max);
}

boolean RFID_Check()          //check if RFID is present or not
{
  RFID_Initialize();
  if(!RFID.PICC_IsNewCardPresent())
  {
    return false;
  }
  else if(!RFID.PICC_ReadCardSerial())
  {
    return false;
  }
  else
  {
    return true;
  }
}

unsigned long Power(byte a)          //power function (exp(10, a))
{
  unsigned long Temp = 1;
  for(byte i=0; i<a; i++)
  {
    Temp = Temp*10;
  }
  return Temp;
}

boolean RFID_Match_Current_Past_UID()          //match the present RFID with the one inserted at the staring
{
  byte RFIDReadCurrentUID[4];
  for (byte i = 0; i < 4; i++)
  {
    RFIDReadCurrentUID[i] = RFID.uid.uidByte[i];
  }
  for (byte i = 0; i < 4; i++)
  {
    if(!(RFIDReadCurrentUID[i] == RFIDReadUID[i]))
    {
      return false;
    }
  }
  return true;
}

boolean RFID_Read_Balance()          //read balance
{
  if(RFID_Check())
  {
    byte TrailerBlock   = (SectorBalance + 1) * 4 - 1;
    
    byte status = RFID.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TrailerBlock, &keyA, &(RFID.uid));
    
    if(status != MFRC522::STATUS_OK)
    {
      return false;
    }

    byte BufferSize = 18;
    status = RFID.MIFARE_Read(BlockBalance, RFIDReadBalance, &BufferSize);
    
    if(status != MFRC522::STATUS_OK)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    return false;
  }
}

boolean RFID_Read_Decimal_Balance()          //read decimal balance
{
  if(RFID_Check())
  {
    byte TrailerBlock   = (SectorBalanceDecimal + 1) * 4 - 1;
    
    byte status = RFID.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TrailerBlock, &keyA, &(RFID.uid));
    
    if(status != MFRC522::STATUS_OK)
    {
      return false;
    }

    byte BufferSize = 18;
    status = RFID.MIFARE_Read(BlockBalanceDecimal, RFIDReadDecimalBalance, &BufferSize);
    
    if(status != MFRC522::STATUS_OK)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    return false;
  }
}

boolean RFID_Write_Balance()          //write balance
{
  if(RFID_Check())
  {
    byte TrailerBlock   = (SectorBalance + 1) * 4 - 1;
    
    byte status = RFID.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TrailerBlock, &keyA, &(RFID.uid));
    
    if(status != MFRC522::STATUS_OK)
    {
      return false;
    }
    
    status = RFID.MIFARE_Write(BlockBalance, RFIDWriteBalance, 16);
    
    if(status != MFRC522::STATUS_OK)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    return false;
  }
}

boolean RFID_Write_Decimal_Balance()          //write decimal balance
{
  if(RFID_Check())
  {
    byte TrailerBlock   = (SectorBalanceDecimal + 1) * 4 - 1;
    
    byte status = RFID.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TrailerBlock, &keyA, &(RFID.uid));
    
    if(status != MFRC522::STATUS_OK)
    {
      return false;
    }
    
    status = RFID.MIFARE_Write(BlockBalanceDecimal, RFIDWriteDecimalBalance, 16);
    
    if(status != MFRC522::STATUS_OK)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    return false;
  }
}

boolean RFID_Current_Balance()          //generate current balance into a variable
{
  if(RFID_Read_Balance() && RFID_Read_Decimal_Balance())
  {
    RFIDCurrentBalance = 0;
    for (int i=15 ; i>=0 ; i--)
    {
      RFIDCurrentBalance = RFIDCurrentBalance + (Power(15-i) * RFIDReadBalance[i]);
    }
    RFIDCurrentDecimalBalance = 0;
    for (int i=15 ; i>=0 ; i--)
    {
      RFIDCurrentDecimalBalance = RFIDCurrentDecimalBalance + (Power(15-i) * RFIDReadDecimalBalance[i]);
    }
    RFIDCurrentBalance = RFIDCurrentBalance + (RFIDCurrentDecimalBalance/100.0);
    return true;
  }
  else
  {
    return false;
  }
}

boolean RFID_Update_Balance()          //upadte balance into RFID
{
  if(RFID_Write_Balance() && RFID_Write_Decimal_Balance())
  {
    return true;
  }
  else
  {
    return false;
  }
}

void RFID_Print_Current_Balance(byte Column)          //display current balance
{
  unsigned long Temp = RFIDCurrentBalance;
  byte DigitCount = 0;
  while(Temp)
  {
    Temp = Temp/10;
    DigitCount++;
  }
  DigitCount = DigitCount + 4;
  LCD.print("                    ");
  LCD.setCursor(0, Column);
  for(byte i=0; i<((20-DigitCount)/2); i++)
  {
    LCD.print(" ");
  }
  LCD.write(byte(7));
  LCD.print(String(RFIDCurrentBalance));
}

boolean RFID_Check_Match()          //check if card is present and if it is changed or not
{
  if(RFID_Check())
  {
    if(!RFID_Match_Current_Past_UID())
    {
      LCD_Clear_Except_Menu_Head();
      LCD.setCursor(0, 2);
      LCD.print("    Card Changed    ");
      delay(3000);
      return false;
    }
  }
  else
  {
    LCD_Clear_Except_Menu_Head();
    LCD.setCursor(0, 2);
    LCD.print("      No  Card      ");
    delay(3000);
    return false;
  }
  return true;
}

boolean RFID_Check_Match_Pump()          //if card changed or no card is present then turn off the pump
{
  if(RFID_Check())
  {
    if(!RFID_Match_Current_Past_UID())
    {
      LCD_Clear_Except_Menu_Head();
      LCD.setCursor(0, 1);
      LCD.print("    Pump Stopped    ");
      digitalWrite(RELAY, HIGH);
      RELAYOn = false;
      LCD.setCursor(0, 2);
      LCD.print("  Card Not Matched  ");
      delay(3000);
      LCD_Clear_Except_Menu_Head();
      LCD.setCursor(0, 2);
      LCD.print("    Please  Wait    ");
      StartPump = false;
      PausePump = false;
      StopPump = true;
      GSM_Send_Store_Pump_Data_No_Card();
      return false;
    }
    return true;
  }
  else
  {
    LCD_Clear_Except_Menu_Head();
    LCD.setCursor(0, 1);
    LCD.print("    Pump Stopped    ");
    digitalWrite(RELAY, HIGH);
    RELAYOn = false;
    LCD.setCursor(0, 2);
    LCD.print("      No  Card      ");
    delay(3000);
    LCD_Clear_Except_Menu_Head();
    LCD.setCursor(0, 2);
    LCD.print("    Please  Wait    ");
    StartPump = false;
    PausePump = false;
    StopPump = true;
    GSM_Send_Store_Pump_Data_No_Card();
    return false;
  }
  return true;
}

void RFID_UID()          //put RFID UID into a string
{
  RFIDUID = "";
  String a, b, c, d;
  a = String(RFIDReadUID[0], HEX);
  a.toUpperCase();
  b = String(RFIDReadUID[1], HEX);
  b.toUpperCase();
  c = String(RFIDReadUID[2], HEX);
  c.toUpperCase();
  d = String(RFIDReadUID[3], HEX);
  d.toUpperCase();
  RFIDUID = a + b + c + d;
}

boolean RFID_Read_UID()
{
  if(RFID_Check())
  {
    byte TrailerBlock   = (SectorUID + 1) * 4 - 1;
    
    byte status = RFID.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TrailerBlock, &keyA, &(RFID.uid));
    
    if(status != MFRC522::STATUS_OK)
    {
      return false;
    }

    byte BufferSize = 18;
    status = RFID.MIFARE_Read(BlockUID, RFIDReadEncryptedUID, &BufferSize);
    
    if(status != MFRC522::STATUS_OK)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    return false;
  }
}

boolean Serial_Send_Read_UID()
{
  if(RFID_Read_UID())
  {
    String TempUID = "";
    for(byte i = 0; i < 12; i++)
    {
      TempUID = TempUID + char(RFIDReadEncryptedUID[i]);
    }
    RFIDEncryptedUID = TempUID;
    return true;
  }
  else
  {
    return false;
  }
}
