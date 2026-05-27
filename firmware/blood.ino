#include <avr/interrupt.h>
#include <LiquidCrystal.h>

#define on HIGH
#define off LOW

#define startState 0
#define inflate1State 1
#define inflate2State 2
#define deflateState 3
#define displayState 4
#define resetState 5

#define Sys_Measure 6
#define Rate_Measure 7
#define dias_Cal 8

#define valve A2
#define motor A3
#define speaker A4

#define bt_start 4
#define bt_stop 3
#define bt_resume 2

#define ADC0 A0
#define ADC1 A1

LiquidCrystal lcd(12,11,10,9,8,7);

unsigned char currentState;
unsigned char meas_state;

volatile unsigned int timepress0 = 0;
volatile unsigned int timepress1 = 0;
volatile unsigned int timepress2 = 0;

volatile unsigned int timing = 0;
volatile unsigned int timerate = 0;
volatile unsigned int timecount = 0;
volatile unsigned int timedeflate = 0;

volatile unsigned int speaker_timer = 0;

float Vref = 5.0;
float DC_gain = 105;

int data;
float adc_data;
float former;

float filtered_adc0 = 0;
float filtered_adc1 = 0;

float pressure = 0;
float maxpressure = 80;

float systolic = 0;
float diastolic = 0;
float pulse_per_min = 0;

float total_pulse_period = 0;

unsigned char count_average = 0;
unsigned char countpulse = 0;
unsigned char sys_count = 0;
unsigned char stop_count = 0;

float TH_sys = 4.9;
float TH_rate = 2.5;

static inline unsigned int read_u16(volatile unsigned int *v)
{
  unsigned int tmp;

  noInterrupts();
  tmp = *v;
  interrupts();

  return tmp;
}

static inline void write_u16(volatile unsigned int *v,
                             unsigned int val)
{
  noInterrupts();
  *v = val;
  interrupts();
}

void start_state(void);
void inflate1_state(void);
void inflate2_state(void);
void deflatestate(void);
void display_state(void);
void reset_state(void);

void pressuremeasure(void);
void sysmeasure(void);
void ratemeasure(void);
void diascal(void);

void read_adc(int Channel);

void setup()
{
  Serial.begin(9600);

  ADCSRA = (ADCSRA & 0xF8) | 0x07;

  pinMode(bt_start, INPUT_PULLUP);
  pinMode(bt_stop, INPUT_PULLUP);
  pinMode(bt_resume, INPUT_PULLUP);

  pinMode(motor, OUTPUT);
  pinMode(valve, OUTPUT);
  pinMode(speaker, OUTPUT);

  digitalWrite(motor, off);
  digitalWrite(valve, off);
  digitalWrite(speaker, LOW);

  lcd.begin(16,2);

  lcd.setCursor(0,0);
  lcd.print("White: Start");

  lcd.setCursor(0,1);
  lcd.print("Grey: Stop");

  cli();

  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;

  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11) | (1 << CS10);

  OCR1A = 249;

  TIMSK1 |= (1 << OCIE1A);

  sei();

  write_u16(&timing,40);

  currentState = startState;
  meas_state = Sys_Measure;
}

void loop()
{
  switch(currentState)
  {
    case startState:
      start_state();
      break;

    case inflate1State:
      inflate1_state();
      break;

    case inflate2State:
      inflate2_state();
      break;

    case deflateState:
      deflatestate();
      break;

    case displayState:
      display_state();
      break;

    case resetState:
      reset_state();
      break;
  }
}

ISR(TIMER1_COMPA_vect)
{
  if(digitalRead(bt_start) == LOW)
    timepress0++;

  if(digitalRead(bt_stop) == LOW)
    timepress1++;

  if(digitalRead(bt_resume) == LOW)
    timepress2++;

  timecount++;
  timedeflate++;

  if(speaker_timer > 0)
  {
    digitalWrite(speaker,HIGH);
    speaker_timer--;
  }
  else
  {
    digitalWrite(speaker,LOW);
  }

  if(timing > 0)
    timing--;

  if(timerate < 65535)
    timerate++;
}

void start_state(void)
{
  if((digitalRead(bt_start) == LOW) &&
     (read_u16(&timepress0) > 30))
  {
    write_u16(&speaker_timer,100);

    pressure = 0;

    systolic = 0;
    diastolic = 0;
    pulse_per_min = 0;

    total_pulse_period = 0;

    count_average = 0;
    countpulse = 0;
    sys_count = 0;
    stop_count = 0;

    filtered_adc0 = 0;
    filtered_adc1 = 0;

    former = TH_sys - 0.01;

    meas_state = Sys_Measure;

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Inflating");

    digitalWrite(valve,off);

    digitalWrite(motor,on);

    currentState = inflate1State;

    write_u16(&timepress0,0);
    write_u16(&timecount,0);
  }
}

void inflate1_state(void)
{
  if(read_u16(&timecount) >= 1000)
  {
    lcd.setCursor(0,1);
    lcd.print("P=");
    lcd.print(pressure);
    lcd.print("     ");

    write_u16(&timecount,0);
  }

  if((digitalRead(bt_stop) == LOW) &&
     (read_u16(&timepress1) > 30))
  {
    digitalWrite(motor,off);
    digitalWrite(valve,off);

    currentState = resetState;

    write_u16(&timepress1,0);
  }
  else
  {
    currentState = inflate2State;
  }
}

void inflate2_state(void)
{
  read_adc(1);

  pressure = (adc_data / DC_gain) * 2000;

  Serial.print("Pressure: ");
  Serial.println(pressure);

  if(pressure >= maxpressure)
    stop_count++;
  else
    stop_count = 0;

  if(stop_count >= 5)
  {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Deflating");

    digitalWrite(motor,off);

    delay(100);

    currentState = deflateState;

    write_u16(&timedeflate,0);
  }
  else
  {
    currentState = inflate1State;
  }
}

void deflatestate(void)
{
  unsigned int timedeflate_local =
    read_u16(&timedeflate);

  if(timedeflate_local >= 100)
  {
    write_u16(&timedeflate,0);
    timedeflate_local = 0;
  }

  if(timedeflate_local < 50)
  {
    digitalWrite(valve,on);
  }
  else
  {
    digitalWrite(valve,off);
  }

  if((digitalRead(bt_stop) == LOW) &&
     (read_u16(&timepress1) > 30))
  {
    digitalWrite(motor,off);
    digitalWrite(valve,off);

    currentState = resetState;

    write_u16(&timepress1,0);
  }

  pressuremeasure();
}

void pressuremeasure(void)
{
  if(read_u16(&timing) != 0)
    return;

  switch(meas_state)
  {
    case Sys_Measure:
      sysmeasure();
      break;

    case Rate_Measure:
      ratemeasure();
      break;

    case dias_Cal:
      diascal();
      break;
  }
}

void sysmeasure(void)
{
  read_adc(0);

  read_adc(1);

  pressure = (adc_data / DC_gain) * 2000;

  if(former <= TH_sys &&
     adc_data > TH_sys)
  {
    sys_count++;
  }

  former = adc_data;

  if(sys_count >= 2)
  {
    systolic = pressure;

    meas_state = Rate_Measure;

    former = TH_rate - 0.01;

    count_average = 0;
    countpulse = 0;

    total_pulse_period = 0;

    write_u16(&timerate,0);
  }

  if(read_u16(&timecount) >= 1000)
  {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Measuring");

    lcd.setCursor(0,1);
    lcd.print(pressure);

    write_u16(&timecount,0);
  }
}

void ratemeasure(void)
{
  read_adc(0);

  if(read_u16(&timerate) > 5000)
  {
    pulse_per_min = 0;

    meas_state = dias_Cal;

    return;
  }

  if(former < TH_rate &&
     adc_data > TH_rate &&
     read_u16(&timerate) > 450)
  {
    if(countpulse == 0)
    {
      write_u16(&timerate,0);
      countpulse = 1;
    }
    else
    {
      total_pulse_period +=
        read_u16(&timerate);

      write_u16(&timerate,0);

      count_average++;
    }

    write_u16(&speaker_timer,30);
  }

  former = adc_data;

  if(count_average >= 5)
  {
    pulse_per_min =
      (5.0 * 60000.0) /
      total_pulse_period;

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Pulse");

    lcd.setCursor(0,1);
    lcd.print(pulse_per_min);

    meas_state = dias_Cal;
  }
}

void diascal(void)
{
  read_adc(1);

  pressure = (adc_data / DC_gain) * 2000;

  diastolic = pressure;

  digitalWrite(valve,off);

  write_u16(&speaker_timer,500);

  currentState = displayState;

  write_u16(&timecount,0);
}

void display_state(void)
{
  if(read_u16(&timecount) >= 1000)
  {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("S:");
    lcd.print(systolic);

    lcd.setCursor(9,0);
    lcd.print("D:");
    lcd.print(diastolic);

    lcd.setCursor(0,1);
    lcd.print("HR:");
    lcd.print(pulse_per_min);

    write_u16(&timecount,0);
  }

  if((digitalRead(bt_resume) == LOW) &&
     (read_u16(&timepress2) > 30))
  {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("White: Start");

    lcd.setCursor(0,1);
    lcd.print("Grey: Stop");

    currentState = startState;

    write_u16(&timepress2,0);
  }
}

void reset_state(void)
{
  lcd.setCursor(0,0);
  lcd.print("Emergency Stop");

  lcd.setCursor(0,1);
  lcd.print("Red: Resume ");

  if((digitalRead(bt_resume) == LOW) &&
     (read_u16(&timepress2) > 30))
  {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("White: Start");

    lcd.setCursor(0,1);
    lcd.print("Grey: Stop");

    currentState = startState;

    write_u16(&timepress2,0);
  }
}

void read_adc(int Channel)
{
  int buf[5];

  for(int i=0;i<5;i++)
  {
    long sum = 0;

    for(int j=0;j<16;j++)
    {
      if(Channel == 0)
        sum += analogRead(ADC0);
      else
        sum += analogRead(ADC1);
    }

    buf[i] = sum / 16;
  }

  for(int i=0;i<4;i++)
  {
    for(int j=i+1;j<5;j++)
    {
      if(buf[j] < buf[i])
      {
        int t = buf[i];
        buf[i] = buf[j];
        buf[j] = t;
      }
    }
  }

  data = buf[2];

  adc_data =
    (float)data * Vref / 1023.0;

  if(Channel == 0)
  {
    filtered_adc0 =
      0.9 * filtered_adc0 +
      0.1 * adc_data;

    adc_data = filtered_adc0;
  }
  else
  {
    filtered_adc1 =
      0.9 * filtered_adc1 +
      0.1 * adc_data;

    adc_data = filtered_adc1;
  }

  write_u16(&timing,40);

  Serial.print(adc_data);
  Serial.print(",");
  Serial.println(pressure);
}