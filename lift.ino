#define NUM_STATES 7
enum events {STATE_CHANGE = 1};
enum states {OFF, STARTING, IDLE, UP, DOWN, STOPPING, FAILURE};

void setup() {
  // put your setup code here, to run once:
  sm_init ();
}

void loop() {
  // put your main code here, to run repeatedly:

}
