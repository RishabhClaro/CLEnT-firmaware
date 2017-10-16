void GPS_Initialize()         //initialise GPS
{
  Serial3.begin(9600);         //set the baud rate of GPS
  digitalWrite(LED_GPS, LOW);
  String LatitudeValue = "0.0";
  String LongitudeValue = "0.0";
  String GPSRead = "";
  unsigned long GPSTempCheckTime = millis();
  while(millis() - GPSTempCheckTime <= GPSCheckMaxTime)
  {
    if(char(Serial3.read()) == '$')
    {
      delay(2);
      if(char(Serial3.read()) == 'G')
      {
        delay(2);
        if(char(Serial3.read()) == 'P')
        {
          delay(2);
          if(char(Serial3.read()) == 'G')
          {
            delay(2);
            if(char(Serial3.read()) == 'L')
            {
              delay(2);
              if(char(Serial3.read()) == 'L')
              {
                delay(2);
                for(int i=0; i<255; i++)
                {
                  GPSRead = GPSRead + char(Serial3.read());
                  delay(2);
                }
                break;
              }
            }
          }
        }
      }
    }
  }
  int i[8], j = 0;
  String s[8];
  i[j++] = GPSRead.indexOf(",");
  if(i[j-1]!=-1)
  {
    while(j!=8)
    {
      i[j++] = GPSRead.indexOf(',', i[j-2] + 1);
      s[j] = GPSRead.substring(i[j-2]+1, i[j-1]);
    }
  }
  if((s[7] == "A") || ((s[2].length() == 9) && (s[4].length() == 10)))
  {
    digitalWrite(LED_GPS, HIGH);
  }
  else
  {
    digitalWrite(LED_GPS, LOW);
  }
}

String GPS_Convert(String s)
{
  int dot = s.indexOf('.');
  String coordinate = "";
  long e = 0;
  if(dot == 4)
  {
    e = (s.substring(2, 9)).toFloat()*1000000/60.0;
    coordinate = String(s[0]) + String(s[1]) + "." + String(e);
    return coordinate;
  }
  else if(dot == 5)
  {
    e = (s.substring(3, 10)).toFloat()*1000000/60.0;
    coordinate = String(s[0]) + String(s[1]) + String(s[2]) + "." + String(e);
    return coordinate;
  }
}

void GPS_Get_Lat_Long()         //get latitude and longitude
{
  LatitudeValue = "0.0";
  LongitudeValue = "0.0";
  String GPSRead = "";
  unsigned long GPSTempCheckTime = millis();
  while(millis() - GPSTempCheckTime <= GPSCheckMaxTime)
  {
    if(char(Serial3.read()) == '$')
    {
      delay(2);
      if(char(Serial3.read()) == 'G')
      {
        delay(2);
        if(char(Serial3.read()) == 'P')
        {
          delay(2);
          if(char(Serial3.read()) == 'G')
          {
            delay(2);
            if(char(Serial3.read()) == 'L')
            {
              delay(2);
              if(char(Serial3.read()) == 'L')
              {
                delay(2);
                for(int i=0; i<255; i++)
                {
                  GPSRead = GPSRead + char(Serial3.read());
                  delay(2);
                }
                break;
              }
            }
          }
        }
      }
    }
  }
  int i[8], j = 0;
  String s[8];
  i[j++] = GPSRead.indexOf(",");
  if(i[j-1]!=-1)
  {
    while(j!=8)
    {
      i[j++] = GPSRead.indexOf(',', i[j-2] + 1);
      s[j] = GPSRead.substring(i[j-2]+1, i[j-1]);
    }
  }
  if((s[7] == "A") || ((s[2].length() == 9) && (s[4].length() == 10)))
  {
    digitalWrite(LED_GPS, HIGH);
    LatitudeValue = GPS_Convert(s[2]);
    LongitudeValue = GPS_Convert(s[4]);
  }
  else
  {
    digitalWrite(LED_GPS, LOW);
  }
}
