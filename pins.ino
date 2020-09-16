#include <PinChangeInt.h>

// manually defined pcint number

#define PIN_LS_ROAD 0
#define PIN_LS_BASEMENT 1
#define PIN_LS_HOUSE 2
#define PIN_CALL_ROAD 4 
#define PIN_CALL_BASEMENT 5
#define PIN_CALL_HOUSE 6
#define PIN_ESTOP 3
#define PIN_FAN_UP 9
#define PIN_FAN_DOWN 9

void setup1() {
  // set pin to input with a pullup, led to output
  //pinMode(pinBlink, INPUT_PULLUP);
  //pinMode(LED_BUILTIN, OUTPUT);

  // Manually blink once to test if LED is functional
  //blinkLed();
  //delay(1000);
  //blinkLed();

  // Attach the new PinChangeInterrupt and enable event function below
  //attachPCINT(digitalPinToPCINT(pinBlink), blinkLed, CHANGE);
}

//void blinkLed(void) {
  // Switch Led state
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//}
