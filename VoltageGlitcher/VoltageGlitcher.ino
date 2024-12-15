/************************************************************************
 *      Voltage Glitcher
 ***********************************************************************/
/* - Designed with a GroundStudio Jade Nano+
 * - Use a MOS Module based on IRF520N
 
/* Serial Stuff */
#define SERIAL_BRATE 115200
String serialBuffer = "";

/* Glitch Stuff */
#define PIN_GLITCH    9       /* Also PWM Pin if needed */
#define MAX_US_DELAY  16000   /* delayMicroseconds() is using 16 bit integer => round it to 16k */

/* Configuration Stuff */
/* Wait times in microseconds */
static int preWait = 10;
static int glitchWait = 10;
static int postWait = 10;

/*********************************************
 *    Glitch_DelayMicroseconds()
 * Implement longer delay than what Arduino
 * provide.
 ********************************************/
void Glitch_DelayMicroseconds(int microseconds)
{
  while(MAX_US_DELAY < microseconds)
  {
    delayMicroseconds(MAX_US_DELAY);
    microseconds -= MAX_US_DELAY;
  }
  delayMicroseconds(microseconds);
}

/*********************************************
 *    Glitch_PreCallout()
 * Implement whatever custom code before the
 * glitch execution.
 ********************************************/
void Glitch_PreCallout(void)
{
  Glitch_DelayMicroseconds(preWait);
}

/*********************************************
 *    Glitch_PostCallout()
 * Implement whatever custom code after the
 * glitch execution.
 ********************************************/
void Glitch_PostCallout(void)
{
  Glitch_DelayMicroseconds(postWait);
}

/*********************************************
 *    Glitch()
 * Perform the Voltage Glitch.
 ********************************************/
void Glitch(void)
{
  /* Prepare */
  Glitch_PreCallout();

  /* Glitch it */
  digitalWrite(PIN_GLITCH, LOW);
  Glitch_DelayMicroseconds(glitchWait);
  digitalWrite(PIN_GLITCH, HIGH);

  /* Afterwards */
  Glitch_PostCallout();
}

/*********************************************
 *    Config_Set()
 * Configure a target parameter to a specified
 * value.
 ********************************************/
void Config_Set(int *target, int value)
{
  /* Check Value */
  if(0 < value)
  {
    /* Set Value */
    *target = value;
    Serial.print(" set to : ");
    Serial.println(value);
  }
  else
  {
    Serial.println("Invalid Value");
  }  
}

/*********************************************
 *    Serial_WrongCommand()
 * Let the user know that the command is wrong.
 ********************************************/
void Serial_WrongCommand()
{
  Serial.println("Wrong Command");
  Serial.println("Command example:<setting>(<value>)");
}

/*********************************************
 *    Serial Parser()
 * Parse the user input from Serial and decide
 * what to do.
 ********************************************/
void Serial_Parser(void)
{
  /* Parse Command */ 
  int firstIndex = serialBuffer.indexOf('(');
  int secondIndex = serialBuffer.indexOf(')');

  /* If Command is valid */
  if ((-1 != firstIndex) && (-1 != secondIndex)) 
  { 
    /* Break Command in Action and Value */
    String action = serialBuffer.substring(0, firstIndex);
    String valueString = serialBuffer.substring(firstIndex + 1, secondIndex); 
    int value = valueString.toInt();

    /* Check Action */
    if(action.equals("Glitch"))
    {
      Glitch();
      Serial.println("I Glitched");
    }
    else if(action.equals("GetPre"))
    {
      /* Print Pre Wait Time */
      Serial.print("Pre Wait : ");
      Serial.println(preWait);
    }
    else if(action.equals("SetPre"))
    {
      /* Set Pre Wait Time */
      Serial.print("Pre Wait");
      Config_Set(&preWait, value);      
    }
    else if(action.equals("GetGlitch"))
    {
      /* Print Glitch Wait TIme */
      Serial.print("Glitch Wait : ");
      Serial.println(glitchWait);
    }
    else if(action.equals("SetGlitch"))
    {
      /* Set Glitch Time */
      Serial.print("Glitch Wait");
      Config_Set(&glitchWait, value); 
    }
    else if(action.equals("GetPost"))
    {
      /* Print Post Wait Time */
      Serial.print("Post Wait : ");
      Serial.println(postWait);
    }
    else if(action.equals("SetPost"))
    {
      /* Set Post Wait Time */
      Serial.print("Post Wait");
      Config_Set(&postWait, value);
    }
    else
    {
      Serial_WrongCommand();
    }
  }
  else
  {
    Serial_WrongCommand();
  }
}

/*********************************************
 *    setup()
 * Initialization of Arduino program.
 ********************************************/
void setup() 
{
  /* Initialize Glitch Pin */
  pinMode(PIN_GLITCH, OUTPUT);      /* Control Glitcher */
  digitalWrite(PIN_GLITCH, HIGH);   /* Power the Device */

  /* Begin Serial */
  Serial.begin(SERIAL_BRATE);
  Serial.println("I begun");
}

/*********************************************
 *    loop()
 * MainFunction of the Arduino program.
 ********************************************/
void loop() 
{
  /* If Command is available */
  if(Serial.available())
  {
    /* Read line */
    serialBuffer = Serial.readStringUntil('\n');

    /* Parse Input */
    Serial_Parser();
  }
}
