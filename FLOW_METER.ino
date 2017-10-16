int x[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int y[4] = {0, 0, 0, 0};

float p;

void FLOW_METER_Initialize()         //initialise flow meter
{
  
}

int return_int(char ch)
{
  switch(ch)
  {
    case '0': return 0;
              break;
    case '1': return 1;
              break;
    case '2': return 2;
              break;
    case '3': return 3;
              break;
    case '4': return 4;
              break;
    case '5': return 5;
              break;
    case '6': return 6;
              break;
    case '7': return 7;
              break;
    case '8': return 8;
              break;
    case '9': return 9;
              break;
    case 'a': return 10;
              break;
    case 'b': return 11;
              break;
    case 'c': return 12;
              break;
    case 'd': return 13;
              break;
    case 'e': return 14;
              break;
    case 'f': return 15;
              break;
  }
}

boolean FLOWMETER_Extract()
{
  char data[256];
  FLOWMETERData.toCharArray(data, 256);
  int i = 0, count = 0;
  while(data[i]!=NULL)
  {
    if(data[i] == '-')
    {
      count++;
    }
    i++;
  }
  if(FLOWMETERData.substring(0, 5) == "1-3-4")
  {
    count = 0;
    i = 0;
    while(count!=3)
    {
      if(data[i] == '-')
      {
        count++;
      }
      i++;
    }
    for(int m=0; m<4; m++)
    {
      if(data[i+1] == '-')
      {
        x[m] = return_int(data[i]);
        i = i + 2;
      }
      else if(data[i+2] == '-')
      {
        x[m] = (return_int(data[i])*16) + return_int(data[i+1]);
        i = i + 3;
      }
      else
      {
        return false;
      }
    }
    ((byte*)&p)[3]= x[2];
    ((byte*)&p)[2]= x[3];
    ((byte*)&p)[1]= x[0];
    ((byte*)&p)[0]= x[1];
  }
  else
  {
    return false;
  }
  return true;
}

void FLOWMETER_Current()         //get the current totaliser value
{
  modbus_construct(&packets[PACKET1], 1, READ_HOLDING_REGISTERS, 0x0504, 2, 0);
  modbus_configure(&Serial2, baud, SERIAL_8N2, timeout, polling, retry_count, FLOW_METER_EN, packets, TOTAL_NO_OF_PACKETS, FLOWMETERRegs);
  FLOWMETERCurrentTime = millis();
  while((millis() - FLOWMETERCurrentTime) <= 5000)
  {
    idle();
    delay(100);
    FLOWMETERData = waiting_for_reply();
    if(FLOWMETER_Extract())
    {
      if(((p) != 0.00) && ((p) >= FLOWMETERCurrent))
      {
        FLOWMETERCurrent = p;
        LCD.setCursor(0, 0);
        LCD.print("                    ");
        LCD.print(FLOWMETERCurrent);
        return;
      }
    }
    delay(100);
  }
  LCD.setCursor(0, 0);
  LCD.print("  Flow Meter Error  ");
}

void FLOWMETER_Flow_Rate()         //get the current flow rate
{
  modbus_construct(&packets[PACKET1], 1, READ_HOLDING_REGISTERS, 0x0708, 2, 0);
  modbus_configure(&Serial2, baud, SERIAL_8N2, timeout, polling, retry_count, FLOW_METER_EN, packets, TOTAL_NO_OF_PACKETS, FLOWMETERRegs);
  FLOWMETERCurrentTime = millis();
  while((millis() - FLOWMETERCurrentTime) <= 5000)
  {
    idle();
    delay(100);
    FLOWMETERData = waiting_for_reply();
    if(FLOWMETER_Extract())
    {
      FLOWMETERFlowRate = p;
      return;
    }
    delay(100);
  }
}
