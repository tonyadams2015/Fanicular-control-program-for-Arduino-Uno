#include <MsTimer2.h>
#include <PinChangeInterrupt.h>
#include <ArduinoQueue.h>
#include <EEPROM.h>

#define NUM_STATES 7
#define NUM_EVENTS 7
#define PIN_LS_ROAD 2
#define PIN_LS_BASEMENT 3
#define PIN_LS_HOUSE 4
#define PIN_CALL_ROAD 5 
#define PIN_CALL_BASEMENT 6
#define PIN_CALL_HOUSE 7
#define PIN_ESTOP 8
#define PIN_FAN_UP 9
#define PIN_FAN_DOWN 10
#define SUCCESS 0
#define FAILURE -1

enum events {EVT_LS_ROAD = 1, EVT_LS_BASEMENT, EVT_LS_HOUSE, EVT_CALL_ROAD,
             EVT_CALL_BASEMENT, EVT_CALL_HOUSE, EVT_ESTOP, EVT_CALL_ROAD_LONG,
             EVT_CALL_BASEMENT_LONG, EVT_CALL_HOUSE_LONG, EVT_MAX};

enum states {ESTOPPED, OFF, TRAIN, IDLE, UP, DOWN, MANUAL};

enum locations {I_AM_LOST, ROAD, BASEMENT, HOUSE, LOCATION_MAX};

String event_desc [NUM_EVENTS + 1] =
  {"",
   "Road limit switch",
   "Basement limit switch",
   "House limit switch",
   "Call road button",
   "Call basement button",
   "Call house button",
   "Estop"};

String location_desc [4] = {"I am lost", "Road", "Basement", "House"};

void setup() {
  Serial.begin(9600);  
  Serial.println("--- Start Serial Monitor\n");

  if (location_load () != SUCCESS)
  {
    return;
  }
  
  pins_init ();
  sm_init ();
}

void loop()
{
}

void lift_stop (void)
{
  digitalWrite(PIN_FAN_UP, HIGH);
  digitalWrite(PIN_FAN_DOWN, HIGH);
}

void lift_up (void)
{
  digitalWrite(PIN_FAN_UP, LOW);
}

void lift_down (void)
{
  digitalWrite(PIN_FAN_DOWN, LOW);
}
