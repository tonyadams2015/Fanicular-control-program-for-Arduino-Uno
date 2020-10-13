#include <MsTimer2.h>
#include <PinChangeInterrupt.h>
#include <ArduinoQueue.h>
#include <EEPROM.h>
#include <SimpleTimer.h>
#include <Arduino.h>

#define NUM_EVENTS 11
#define SUCCESS 0
#define FAILURE -1

/* Pin GPIO numbers */
#define PIN_LS_ROAD 2
#define PIN_LS_BASEMENT 3
#define PIN_LS_HOUSE 4
#define PIN_CALL_ROAD 5 
#define PIN_CALL_BASEMENT 6
#define PIN_CALL_HOUSE 7
#define PIN_ESTOP 8
#define PIN_MAN_UP 9
#define PIN_MAN_DOWN 10
#define PIN_LIFT_UP 11
#define PIN_LIFT_DOWN 12
#define PIN_LIFT_ESTOPPED 19

#define MOTOR_STOP_TIME 5000

enum events 
{
  EVT_LS_ROAD = 1,
  EVT_LS_BASEMENT,
  EVT_LS_HOUSE,
  EVT_CALL_ROAD,
  EVT_CALL_BASEMENT,
  EVT_CALL_HOUSE,
  EVT_ESTOP,
  EVT_CALL_ROAD_LONG,
  EVT_CALL_BASEMENT_LONG,
  EVT_CALL_HOUSE_LONG,
  EVT_LIFT_STOPPED,
  EVT_LIFT_MAN_UP,
  EVT_LIFT_MAN_DOWN,
  EVT_MAX};

enum states
{
  ST_ESTOPPED,
  ST_OFF,
  ST_TRAIN,
  ST_IDLE,
  ST_UP,
  ST_DOWN,
  ST_MANUAL_UP,
  ST_MANUAL_DOWN,
  ST_STOPPING,
  ST_STATE_MAX
};

enum locations 
{
  LOC_LOST,
  LOC_ROAD, 
  LOC_BASEMENT, 
  LOC_HOUSE, 
  LOC_MAX
};

SimpleTimer timer;

void setup() {
  Serial.begin(9600);  
  Serial.println(F ("Lift control program starting..."));

  sm_init ();
  tests_run ();

  if (loc_load () != SUCCESS)
  {
    return;
  }

  pins_init ();
  sm_init ();
}

void loop()
{
  timer.run ();
}

void lift_stop (void)
{
  Serial.println("stop");
  digitalWrite(PIN_LIFT_UP, LOW);
  digitalWrite(PIN_LIFT_DOWN, LOW);
} 

void lift_estopped(void)
{
  digitalWrite(PIN_LIFT_ESTOPPED, LOW);  
}

void lift_estop_ok(void)
{
  digitalWrite(PIN_LIFT_ESTOPPED, HIGH);  
}

void lift_up (void)
{
  Serial.println("up");
  digitalWrite(PIN_LIFT_UP, HIGH);
}

void lift_down (void)
{
  digitalWrite(PIN_LIFT_DOWN, HIGH);
}

void lift_stopping (void)
{
  timer.setTimeout (MOTOR_STOP_TIME, lift_stopped);
}

void lift_stopped (void)
{
  sm_event_send (EVT_LIFT_STOPPED, 0);
}

template <typename T> void PROGMEM_readAnything (const T * sce, T& dest)
  {
  memcpy_P (&dest, sce, sizeof (T));
  }

template <typename T> T PROGMEM_getAnything (const T * sce)
  {
  static T temp;
  memcpy_P (&temp, sce, sizeof (T));
  return temp;
  }
