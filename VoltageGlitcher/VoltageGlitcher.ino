/************************************************************************
 *      Voltage Glitcher
 ***********************************************************************/
/* - Designed with a GroundStudio Jade Nano+
 * - Use a MOS Module based on IRF52
/* Serial Stuff */
#define SERIAL_BRATE 115200
String serialBuffer = "";

/* Glitch Stuff */
#define PIN_GLITCH    9       /* Also PWM Pin if needed */
#define MAX_MS_DELAY  16383   /* delayMicroseconds() is using 16 bit integer */

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
  while(MAX_MS_DELAY < microseconds)
  {
    delayMicroseconds(MAX_MS_DELAY);
    microseconds -= MAX_MS_DELAY;
  }
  delayMicroseconds(microseconds);
}

void Glitch_PreCallout(void)
{
  Glitch_DelayMicroseconds(preWait);
}

void Glitch_PostCallout(void)
{
  Glitch_DelayMicroseconds(postWait);
}

/* Glitch the Voltage */
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

void Config_Set(int *target, int value)
{
  /* Check Value */
  if(0 < value)
  {
    *target = value;
    Serial.print(" set to : ");
    Serial.println(value);
  }
  else
  {
    Serial.println("Invalid Value");
  }  
}

void Serial_WrongCommand()
{
  Serial.println("Wrong Command");
  Serial.println("Command example:<setting>(<value>)");
}

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

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_GLITCH, OUTPUT);      /* Control Glitcher */
  digitalWrite(PIN_GLITCH, HIGH);   /* Power the Device */

  /* Begin Serial */
  Serial.begin(SERIAL_BRATE);
  Serial.println("I begun");
}

void loop() {
  // put your main code here, to run repeatedly:
  /* If Command is available */
  if(Serial.available())
  {
    /* Read line */
    serialBuffer = Serial.readStringUntil('\n');

    /* Parse Input */
    Serial_Parser();
  }
}
