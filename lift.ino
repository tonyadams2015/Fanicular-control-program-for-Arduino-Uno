#define NUM_STATES 7
enum events {STATE_CHANGE = 1};
enum states {OFF, STARTING, IDLE, UP, DOWN, STOPPING, FAILURE};
enum inputs {LS_ROAD, LS_BASEMENT, LS_HOUSE, CALL_ROAD, CALL_BASEMENT, CALL_HOUSE, ESTOP};
enum outputs {FAN_UP, FAN_DOWN};

void setup() {
  // put your setup code here, to run once:
  sm_init ();
}

void loop() {
  // put your main code here, to run repeatedly:

}
