
void Start()         //if start button is pressed
{
  if(DashBoard == 1)         //if in Main Dashboard
  {
    DashBoard = 2;
    if(RFIDCurrentBalance <= MIN_AMOUNT)         //if balance is less than the minimum balance then stop the pump
    {
      StartPump = false;
      PausePump = false;
      StopPump = true;
    }
    else         //start the pump
    {
      StartPump = true;
      PausePump = false;
      StopPump = false;
    }
    RFIDLastTime = 0;
    LCD_Clear_Except_Menu_Head();
  }
  else if(DashBoard == 2)         //if in Pump Operations Dash Board
  {
    if(!RELAYOn)         //if pump is off then switch it on
    {
      DashBoard = 2;
      StartPump = true;
      PausePump = false;
      StopPump = false;
    }
  }
  else if((!SendData) && (DashBoard == 0) && (!RFIDCheckedIn))         //if in Insert Card Menu then send left over data in SD card to server
  {
    SendData = true;
    LCD_Clear_Except_Menu_Head();
    LCD.setCursor(0, 2);
    LCD.print("    Please  Wait    ");
  }
}

void Pulse()         //if pause button is pressed
{
  if(digitalRead(18)==LOW)
  {
 while(digitalRead(18)!=HIGH);
    flag=true; 
  }
}

void Stop()         //if stop button is pressed
{
  if(DashBoard == 1)         //if in Main Dash Board then exit it
  {
    ExitDashBoard = true;
  }
  else if(DashBoard == 2)         //if in Pump Operations Dash Board then stop the pump
  {
    StartPump = false;
    PausePump = false;
    StopPump = true;
  }
  else if(DashBoard == 3)         //if in Recharge Dash Board then exit
  {
    ExitRecharge = true;
  }
}

void Recharge()         //if recharge button is pressed
{
  if(DashBoard == 1)         //if in Main Dash Board then goto Recharge Dash Board
  {
    DashBoard = 3;
  }
}
