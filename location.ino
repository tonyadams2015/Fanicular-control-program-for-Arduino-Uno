
static int loc_addr = 0x0;
static int cmd_addr = 0x01;
static byte lift_location = I_AM_LOST;

void location_cmd_set (byte cmd)
{
  EEPROM.write(cmd_addr, cmd);
  Serial.println ("Writing command " + event_desc [cmd] + " to EEPROM\n");
}

void location_set (byte event)
{
  lift_location = event;
  EEPROM.write(loc_addr, event);
  Serial.println ("Writing new location " + location_desc [event] + " to EEPROM\n");
}

int location_load (void)
{
  byte last_cmd;

  /* Get location */
  lift_location = EEPROM.read(loc_addr);
  if (lift_location >= LOCATION_MAX)
  {
    Serial.println ("Could not load location - EEPROM Failure\n");
    return -1;
  }
  
  last_cmd = EEPROM.read(cmd_addr);

  if (last_cmd > EVT_MAX)
  {
    Serial.println ("Could not load last command - EEPROM Failure\n");
    return -1;
  }

  /* Are command and location consistent */
  switch (last_cmd)
  {
    case EVT_CALL_ROAD:
    case EVT_CALL_HOUSE_LONG:
      if (lift_location != ROAD)
      {
        lift_location = I_AM_LOST;
      }
      break;
    case EVT_CALL_BASEMENT:
      if (lift_location != BASEMENT)
      {
        lift_location = I_AM_LOST;
      }
      break;
    case EVT_CALL_HOUSE:
    case EVT_CALL_ROAD_LONG:
      if (lift_location != HOUSE)
      {
        lift_location = I_AM_LOST;
      }    
      break;
  }

  Serial.println ("We are at the " + location_desc [lift_location] + "\n");
}

int location_get ()
{
  if (lift_location == I_AM_LOST)
  {
    Serial.println ("I am lost.\n");
  }
  return lift_location;
}

byte how_to_move_to_location (byte location)
{
  switch (location)
  {
    case EVT_CALL_ROAD:
      switch (lift_location)
      {
        case EVT_LS_ROAD:
          return IDLE;
        case EVT_LS_BASEMENT:
        case EVT_LS_HOUSE:
          return DOWN; 
      }
    case EVT_CALL_BASEMENT:
      switch (lift_location)
      {
        case EVT_LS_ROAD:
          return UP;
        case EVT_LS_BASEMENT:
          return IDLE;
        case EVT_LS_HOUSE:
          return DOWN; 
      }
      case EVT_CALL_HOUSE:
      switch (lift_location)
      {
        case EVT_LS_ROAD:
          return UP;
        case EVT_LS_BASEMENT:
          return UP;
        case EVT_LS_HOUSE:
          return IDLE; 
      }
  }
}
