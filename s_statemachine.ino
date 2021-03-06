static int sm_curr_state = ST_OFF;

void (*sm_enter_cb [ST_STATE_MAX])(void) = {sm_enter_estop,
                                          sm_enter_off,
                                          sm_enter_train,
                                          sm_enter_idle,
                                          sm_enter_up,
                                          sm_enter_down,
                                          sm_enter_manual_up,
                                          sm_enter_manual_down,
                                          sm_enter_stopping};
                                          
void (*sm_cb [ST_STATE_MAX])(int, long) =   {sm_estop,
                                           sm_off,
                                           sm_train,
                                           sm_idle,
                                           sm_up,
                                           sm_down,
                                           sm_manual_up,
                                           sm_manual_down,
                                           sm_stopping};

void (*sm_exit_cb [ST_STATE_MAX])(void) = {};

String state_desc (byte state) 
{
  switch (state)
  {
    case ST_ESTOPPED:
      return F("Emergency stop state");
    case ST_OFF:
      return F("Off state");
    case ST_TRAIN:
      return F("Train state");
    case ST_IDLE:
      return F("Idle state");
    case ST_UP:
      return F("Up state");
    case ST_DOWN:
      return F("Down state");
    case ST_MANUAL_UP:
      return F("Manual up state");
    case ST_MANUAL_DOWN:
      return F("Manual down state");
    case ST_STOPPING:
      return F("Stopping state");
    default:
      Serial.print (F("No such state "));
      Serial.println (state);
  }
}

String event_desc (byte event)
{
  switch (event)
  {
    case EVT_LS_ROAD: 
      return F("Road limit switch");
   case EVT_LS_BASEMENT: 
      return F("Basement limit switch");
   case EVT_LS_HOUSE: 
      return F("House limit switch");
   case EVT_CALL_ROAD: 
      return F("Call road button");
   case EVT_CALL_BASEMENT: 
      return F("Call basement button");
   case EVT_CALL_HOUSE: 
      return F("Call house button");
   case EVT_ESTOP: 
      return F("Estop");
   case EVT_CALL_ROAD_LONG: 
      return F("Call road button long");
   case EVT_CALL_BASEMENT_LONG: 
      return F("Call basement button long");
   case EVT_CALL_HOUSE_LONG: 
      return F("Call house button long");
   case EVT_LIFT_STOPPED: 
      return F("Stopped");
   case EVT_LIFT_MAN_UP: 
      return F("MANUAL UP");
   case EVT_LIFT_MAN_DOWN: 
      return F("MANUAL DOWN");
   default:
     Serial.print (F("No such event "));
     Serial.println (event);
  }
}

void sm_init (void)                              
{
  sm_next_state (ST_OFF);
}

void sm_event_send (int event, long value)
{
  Serial.print (F ("Processing "));
  Serial.print (event_desc (event));
  Serial.print (F (" = "));
  Serial.println (switch_state_desc (value));
  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      if (value == LOW)
      {
        loc_set (event);
      }
      break;
    case EVT_CALL_ROAD:
    case EVT_CALL_BASEMENT:
    case EVT_CALL_HOUSE:
    case EVT_CALL_ROAD_LONG:
    case EVT_CALL_HOUSE_LONG:
      if (value == LOW)
      {
        loc_cmd_set (event);
      }
      break;
    case EVT_ESTOP:
      if (value == HIGH)
      {
        sm_next_state (ST_ESTOPPED);
      }
      break;  
    case EVT_LIFT_STOPPED:
    case EVT_LIFT_MAN_UP:
    case EVT_LIFT_MAN_DOWN:
      break;
    default:
      Serial.print (F ("No such event "));
      Serial.println (event);
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

static void sm_enter (void)
{
  if (sm_enter_cb [sm_curr_state] != NULL)
  {
    Serial.print (F ("Entering "));
    Serial.println (state_desc (sm_get_curr_state ()));

    sm_enter_cb [sm_curr_state] ();
  }
}

static void sm_update (int event, long value)
{
  if (sm_cb [sm_curr_state] != NULL)
  {
    sm_cb [sm_curr_state] (event, value);
  }
}

static void sm_exit (void)
{
  if (sm_exit_cb && sm_exit_cb [sm_curr_state] != NULL)
  {
    sm_exit_cb [sm_curr_state] ();
  }
}

static void sm_enter_estop (void)
{
  lift_stop ();
  lift_estopped ();
}


static void sm_enter_off (void)
{
  lift_stop ();
  
  if (pin_check_ready () == true)
  {
    if (loc_get () != LOC_LOST)
    {
    sm_next_state (ST_IDLE);
    }
    else
    {
      sm_next_state (ST_TRAIN);
    }
  }
}

static void sm_enter_train (void)
{
  lift_stop ();
}

static void sm_enter_idle (void)
{
  lift_stop ();
  lift_estop_ok ();
}

static void sm_enter_up (void)
{
  lift_up ();
}

static void sm_enter_down (void)
{
  lift_down ();
}

static void sm_enter_manual_up (void)
{
  lift_up ();
}

static void sm_enter_manual_down (void)
{
  lift_down ();
}

static void sm_enter_stopping (void)
{
  lift_stopping ();
  lift_stop ();
}

static void sm_estop (int event, long value)
{
  if (event == EVT_ESTOP && value == LOW)
  {
    sm_next_state (ST_OFF);
  }
}

static void sm_off (int event, long value)
{  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
    case EVT_CALL_ROAD:
    case EVT_CALL_BASEMENT:
    case EVT_CALL_HOUSE:
      if (pin_check_ready () == true)
      {
        if (loc_get () != LOC_LOST)
        {
          sm_next_state (ST_IDLE);
        }
        else
        {
          sm_next_state (ST_TRAIN);
        }
      }
      break;
    case EVT_ESTOP:
      if (value == LOW && pin_check_ready () == true)
      {
        if (loc_get () != LOC_LOST)
        {
          sm_next_state (ST_IDLE);
        }
        else
        {
          sm_next_state (ST_TRAIN);
        }
      }
      break;
  }
}

static void sm_train (int event, long value)
{
  if (value == HIGH)
  {
    /* Don't care */
    return;
  }
  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      if (loc_get () != LOC_LOST)
      {
        sm_next_state (ST_IDLE);
      }
      break;
    case EVT_CALL_ROAD_LONG:
      sm_next_state (ST_DOWN);
      break;
    case EVT_CALL_HOUSE_LONG:
      sm_next_state (ST_UP);
      break;
  }
}

static void sm_idle (int event, long value)
{
  byte next;
  if (value == HIGH)
  {
    /* Don't care */
    return;
  }
  
  switch (event)
  {
    case EVT_CALL_ROAD:
      loc_target_set (LOC_ROAD);
      sm_next_state (loc_direction_lookup ());
      break;
    case EVT_CALL_BASEMENT:
      loc_target_set (LOC_BASEMENT);
      sm_next_state (loc_direction_lookup ());
      break;
    case EVT_CALL_HOUSE:
      loc_target_set (LOC_HOUSE);
      sm_next_state (loc_direction_lookup ());
      break;
    case EVT_LIFT_MAN_UP:
      sm_next_state (ST_MANUAL_UP);
      break;
    case EVT_LIFT_MAN_DOWN:
      sm_next_state (ST_MANUAL_DOWN);
      break;     
  }
}

static void sm_up (int event, long value)
{
  if (value == HIGH)
  {
    /* Don't care */
    return;
  }
  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      sm_next_state (loc_direction_lookup ());
      break;
  }
}
 
static void sm_down (int event, long value)
{
  if (value == HIGH)
  {
    /* Don't care */
    return;
  }
  
  switch (event)
  {
    case EVT_LS_ROAD:
    case EVT_LS_BASEMENT:
    case EVT_LS_HOUSE:
      sm_next_state (loc_direction_lookup ());
      break;
  }
}

/* Only available above HOUSE
 */
static void sm_manual_up (int event, long value)
{
  switch (event)
  {
    case EVT_LS_ROAD:
      sm_next_state (ST_STOPPING);
      break;
    case EVT_LIFT_MAN_UP:
      /* Button released */
      if (value == HIGH)
      {
         sm_next_state (ST_STOPPING);
      }
      break;
  }
}

static void sm_manual_down (int event, long value)
{
  switch (event)
  {
    case EVT_LS_ROAD:
      sm_next_state (ST_STOPPING);
      break;
    case EVT_LIFT_MAN_DOWN:
      /* Button released */
      if (value == HIGH)
      {
        sm_next_state (ST_STOPPING);
      }
      break;
  }
}

static void sm_stopping (int event, long value)
{
  if (event == EVT_LIFT_STOPPED)
  {
    sm_next_state (ST_IDLE);
  }
}
