

static int sm_curr_state = OFF;

void sm_enter_off (void);
void sm_enter_starting (void);
void sm_enter_idle (void);
void sm_enter_up (void);
void sm_enter_down (void);
void sm_enter_stopping (void);
void sm_enter_failure (void);

void sm_off (int, long);
void sm_starting (int, long);
void sm_idle (int, long);
void sm_up (int, long);
void sm_down (int, long);
void sm_stopping (int, long);
void sm_failure (int, long);

void sm_exit_off (void);
void sm_exit_starting (void);
void sm_exit_idle (void);
void sm_exit_up (void);
void sm_exit_down (void);
void sm_exit_stopping (void);
void sm_exit_failure (void);

void (*sm_enter_cb [NUM_STATES])(void) = {sm_enter_off,
                                          sm_enter_manual,
                                          sm_enter_idle,
                                          sm_enter_up,
                                          sm_enter_down};
                                          
void (*sm_cb [NUM_STATES])(int, long) =   {sm_off,
                                           sm_manual,
                                           sm_idle,
                                           sm_up,
                                           sm_down};
                            
void (*sm_exit_cb [NUM_STATES])(void)  = {sm_exit_off,
                                          sm_exit_manual,
                                          sm_exit_idle, 
                                          sm_exit_up,
                                          sm_exit_down};
                                          
void sm_init (void)                              
{
  sm_next_state (OFF);
}

void sm_event_send (int event, long value)
{
  Serial.println ("Processing switch/button activation, event " + String(event) + "\n");

  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      lift_location_set (event);
      break;  
  }
  
  sm_update (event, value);
}

int sm_get_curr_state (void)
{
  return sm_curr_state;
}

void sm_next_state (int new_state)
{
  sm_exit ();
  sm_curr_state = new_state;
  sm_enter ();
}

void sm_enter (void)
{
  if (sm_enter_cb [sm_curr_state] != NULL)
  {
    sm_enter_cb [sm_curr_state] ();
  }
}

void sm_update (int event, long value)
{
  if (sm_cb [sm_curr_state] != NULL)
  {
    sm_cb [sm_curr_state] (event, value);
  }
}

void sm_exit (void)
{
  if (sm_exit_cb [sm_curr_state] != NULL)
  {
    sm_exit_cb [sm_curr_state] ();
  }
}

void sm_enter_off (void)
{
  Serial.println ("Entering Off state\n");

  lift_off ();
  
  if (check_inputs_ready () == true && lift_location_get () != I_AM_LOST)
  {
    sm_next_state (IDLE);
  }
}

void sm_enter_manual (void)
{
  Serial.println("Entering manual state\n");
  lift_off ();
}

void sm_enter_idle (void)
{
  Serial.println("Entering Idle state\n");
  lift_off ();
}

void sm_enter_up (void)
{

}

void sm_enter_down (void)
{

}

void sm_off (int event, long value)
{
  if (check_inputs_ready () == true)
  {
    if (lift_location_get () != I_AM_LOST)
    {
    sm_next_state (IDLE);
    }
    else
    {
      sm_next_state (MANUAL);
    }
  }
}

void sm_manual (int event, long value)
{
  
}

void sm_idle (int event, long value)
{
}

void sm_up (int event, long value)
{

}
 
void sm_down (int event, long value)
{

}

void sm_exit_off (void)
{
  
}

void sm_exit_manual (void)
{
  
}

void sm_exit_idle (void)
{
  
}

void sm_exit_up (void)
{
  
}

void sm_exit_down (void)
{

}
