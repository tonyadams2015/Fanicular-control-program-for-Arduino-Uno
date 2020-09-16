#define NUM_STATES 7


#define PIN_LS_ROAD 0
#define PIN_LS_BASEMENT 1
#define PIN_LS_HOUSE 2
#define PIN_CALL_ROAD 4 
#define PIN_CALL_BASEMENT 5
#define PIN_CALL_HOUSE 6
#define PIN_ESTOP 3
#define PIN_FAN_UP 9
#define PIN_FAN_DOWN 9

enum events {EVT_STATE_CHANGE, EVT_LS_ROAD, EVT_LS_BASEMENT, EVT_LS_HOUSE, EVT_CALL_ROAD, EVT_CALL_BASEMENT, EVT_CALL_HOUSE, EVT_ESTOP};
enum states {OFF, STARTING, IDLE, UP, DOWN, STOPPING, FAILURE};

void setup() {
  pins_init ();
  sm_init ();
}

void loop() {
  // put your main code here, to run repeatedly:

}
