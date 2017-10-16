void SENSORS_Initialize()          //initialise sensors
{
  AVCC = 0.0;
  VoltageDividerAnalogInput = 0.0;
  VoltageValue = 0.0;
  CurrentSensorAnalogInput = 0;
  CurrentValue = 0.0;
  TemperatureValue = 0.0;
  HumidityValue = 0.0;
}

long SENSORS_Read_AVCC()          //read AVCC from internal registers
{
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2);
    ADCSRB &= ~_BV(MUX5);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA,ADSC));
  uint8_t low  = ADCL;
  uint8_t high = ADCH;
  long result = (high<<8) | low;
  result = 1076852L / result;
  return result;
}

void SENSORS_Collect_Data()          //collect sensors data
{
  SENSORS_Initialize();
  
  for(int i=0; i<SENSORS_SAMPLES; i++)
  {
    VoltageDividerAnalogInput += Sampler2->read();
    delay(1);
    CurrentSensorAnalogInput += Sampler1->read();
    delay(1);
    AVCC += SENSORS_Read_AVCC()/1000.0;
    delay(1);
  }  
        
  VoltageDividerAnalogInput = VoltageDividerAnalogInput/(float)SENSORS_SAMPLES;
  CurrentSensorAnalogInput = CurrentSensorAnalogInput/(float)SENSORS_SAMPLES;
  AVCC = AVCC/(float)SENSORS_SAMPLES;
  
  VoltageDividerAnalogInput = constrain(VoltageDividerAnalogInput, 0, 1023);
  VoltageValue = (((((VoltageDividerAnalogInput*AVCC)/1023)-1.25)*2)*((R1 + R2)/R2)) + 1.4;
  
  CurrentSensorAnalogInput = constrain(CurrentSensorAnalogInput, 0, 1023);
  CurrentValue = (1/0.063)*(((CurrentSensorAnalogInput*AVCC)/1023)-(AVCC/2)) + 0.26;

  if(VoltageValue < 0)
  {
    VoltageValue = 0;
  }

  if(CurrentValue < 0)
  {
    CurrentValue = 0;
  }
  
  TemperatureValue = 0.0;
  HumidityValue = 0.0;

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t))
  {
    TemperatureValue = 0.0;
    HumidityValue = 0.0;
  }
  else
  {
    TemperatureValue = t;
    HumidityValue = h;
  }
}
