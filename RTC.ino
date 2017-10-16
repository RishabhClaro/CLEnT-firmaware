void RTC_Initialize()          //initialise RTC
{
  Wire.begin();
  RTC_Begin();
}

boolean RTC_Begin()          //begin RTC
{
  if(RTC.chipPresent())
  {
    digitalWrite(LED_RTC, HIGH);
    return true;
  }
  else
  {
    digitalWrite(LED_RTC, LOW);
    return false;
  }
}

int RTC_Day()          //get day
{
  if(RTC_Begin())
  {
     RTC.read(tm);
    return tm.Day;
  }
  else
  {
    return 0;
  }
}

int RTC_Month()          //get month
{
  if(RTC_Begin())
  {
    RTC.read(tm);
    return tm.Month;
  }
  else
  {
    return 0;
  }
}

int RTC_Year()          //get year
{
  if(RTC_Begin())
  {
    RTC.read(tm);
    return tmYearToCalendar(tm.Year);
  }
  else
  {
    return 0;
  }
}

int RTC_Hour()          //get hour
{
  if(RTC_Begin())
  {
    RTC.read(tm);
    return tm.Hour;
  }
  else
  {
    return 0;
  }
}

int RTC_Minute()          //get minute
{
  if(RTC_Begin())
  {
    RTC.read(tm);
    return tm.Minute;
  }
  else
  {
    return 0;
  }
}

int RTC_Second()          //get second
{
  if(RTC_Begin())
  {
    RTC.read(tm);
    return tm.Second;
  }
  else
  {
    return 0;
  }
}

void RTC_Date_Time()          //set date and time into a string
{
  RTCDate = "";
  RTCTime = "";
  int TempMonth = RTC_Month();
  String RTCMonth = "";
  if(TempMonth < 10)
  {
    RTCMonth = "0" + String(TempMonth);
  }
  else
  {
    RTCMonth = String(TempMonth);
  }
  int TempDay = RTC_Day();
  String RTCDay = "";
  if(TempDay < 10)
  {
    RTCDay = "0" + String(TempDay);
  }
  else
  {
    RTCDay = String(TempDay);
  }
  int TempMinute = RTC_Minute();
  String RTCMinute = "";
  if(TempMinute < 10)
  {
    RTCMinute = "0" + String(TempMinute);
  }
  else
  {
    RTCMinute = String(TempMinute);
  }
  int TempSecond = RTC_Second();
  String RTCSecond = "";
  if(TempSecond < 10)
  {
    RTCSecond = "0" + String(TempSecond);
  }
  else
  {
    RTCSecond = String(TempSecond);
  }
  RTCDate = String(RTC_Year()) + "-" + RTCMonth + "-" + RTCDay;
  RTCTime = String(RTC_Hour()) + ":" + RTCMinute + ":" + RTCSecond;
}

void RTC_Date_Time_For_SD()          //generate file name based on date and time for storing data into SD card if network is not present
{
  RTCSDDate = "";
  RTCSDTime = "";
  RTCSDDate = String(RTC_Month()) + String(RTC_Day());
  RTCSDTime = String(RTC_Hour()) + String(RTC_Minute());
}

boolean RTC_Check_Schedule_Time()         //check if the current time is in the scheduled time frame
{
  if(RTC_Hour() > StartHour)
  {
    if(RTC_Hour() < StopHour)
    {
      return true;
    }
    else if(RTC_Hour() == StopHour)
    {
      if(RTC_Minute() <= StopMinute)
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
  else if(RTC_Hour() == StartHour)
  {
    if(RTC_Minute() >= StartMinute)
    {
      if(RTC_Hour() < StopHour)
      {
        return true;
      }
      else if(RTC_Hour() == StopHour)
      {
        if(RTC_Minute() <= StopMinute)
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
