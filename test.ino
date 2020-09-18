#include <assert.h>

#define NUM_TESTS 4

typedef struct
{
  byte in;
  byte out;
} test_args_t;

bool test_change_location (byte n);
bool test_move_up (byte n);
bool test_ready_to_stop (byte n);
bool test_stop_at_location (byte n);

bool (*test_fn [NUM_TESTS])(byte) = 
  {
    test_change_location,
    test_move_up,
    test_ready_to_stop,
    test_stop_at_location
  };

static test_args_t args [NUM_TESTS] =
  {
    {EVT_LS_ROAD, ROAD},
    {EVT_CALL_BASEMENT, UP},
    {EVT_LS_BASEMENT, STOPPING},
    {EVT_LIFT_STOPPED, IDLE}
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
    if (test_fn [i](i) == true)
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

bool test_change_location (byte n)
{
  sm_event_send (args [n].in, 0);
  return check (location_get () == ROAD);
}

bool test_move_up (byte n)
{
  sm_event_send (EVT_CALL_BASEMENT, 0);
  return check (sm_get_curr_state () == UP);
}

bool test_ready_to_stop (byte n)
{
  sm_event_send (EVT_LS_BASEMENT, 0);
  return check (sm_get_curr_state () == STOPPING);
}

bool test_stop_at_location (byte n)
{
  sm_event_send (EVT_LIFT_STOPPED, 0);
  return check (sm_get_curr_state () == IDLE);
}
