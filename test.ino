#include <assert.h>

#define NUM_TESTS 8

typedef struct
{
  byte in;
  byte out;
} test_args_t;

bool test_event_new_location (byte n);
bool test_event (byte n);

bool (*test_fn [NUM_TESTS])(byte) = 
  {
    test_event_new_location,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
    test_event,
  };

static test_args_t args [NUM_TESTS] =
  {
    {EVT_LS_ROAD, ROAD},
    {EVT_CALL_BASEMENT, UP},
    {EVT_LS_BASEMENT, STOPPING},
    {EVT_LIFT_STOPPED, IDLE},
    {EVT_CALL_HOUSE, UP},
    {EVT_LS_HOUSE, STOPPING},
    {EVT_LIFT_STOPPED, IDLE},
    {EVT_CALL_HOUSE_LONG, MANUAL},
  };

byte check (bool test)
{
  if (test)
  {
    return true;
  }

  return false;
}

void tests_run ()
{
  int i;

  for (i = 0; i < NUM_TESTS; i++)
  {
    if (test_fn [i] (i) == true)
    {
      Serial.print ("Test ");
      Serial.print (i);
      Serial.println (" passed");
    }
    else
    {
      Serial.print ("Test ");
      Serial.print (i);
      Serial.println (" failed");
    }
  }
}

bool test_event_new_location (byte n)
{
  sm_event_send (args [n].in, 0);
  return check (location_get () == args [n].out);
}

bool test_event (byte n)
{
  sm_event_send (args [n].in, 0);
  return check (sm_get_curr_state () == args [n].out);
}
