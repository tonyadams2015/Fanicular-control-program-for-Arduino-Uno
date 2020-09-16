#define NUM_STATES 7

enum events {EVT_STATE_CHANGE, EVT_LS_ROAD, EVT_LS_BASEMENT, EVT_LS_HOUSE, EVT_CALL_ROAD,
             EVT_CALL_BASEMENT, EVT_CALL_HOUSE, EVT_ESTOP};
enum states {OFF, STARTING, IDLE, UP, DOWN, STOPPING, FAILURE};

void setup() {
  Serial.begin(9600);  
  Serial.println("--- Start Serial Monitor\n");
  pins_init ();
  sm_init ();
}

void loop() {
}
