#define NAME "PWMCtrl"
#define VERSION "0.2"

int echo = 0;


void setup() {
  Serial.begin(9600);
}


void ShowHelp (void)
{
  Serial.print ("\r\n----------------\r\n");
  Serial.print (NAME);
  Serial.print (" v");
  Serial.print (VERSION);
  Serial.print ("\r\n----------------\r\n");
}


void PrintEcho (void)
{
  Serial.println (echo ? "Echo is on" : "Echo is off");
}


void PrintPrompt (void)
{
  Serial.print ("# ");
}


/* ===================================================== */
/* ===================================================== */


#define ARGVCMP(_i, _x) (strcmp(argv[_i], _x) == 0)


void cmdUnknown (int argc, char *argv[])
{
  Serial.print ("Unknown command \"");
  Serial.print (argv[0]);
  Serial.println ("\"");
}


void cmdRead (int argc, char *argv[])
{
  //...
}


void cmdWrite (int argc, char *argv[])
{
  //...
}


void cmdMillis (int argc, char *argv[])
{
  Serial.print (millis());
  Serial.println (" ms");
}


void cmdEcho (int argc, char *argv[])
{
  if (argc > 1)
    echo = ARGVCMP(1, "on");
  PrintEcho ();
}


void cmdHelp (int argc, char *argv[])
{
  ShowHelp ();
}


/* ===================================================== */
/* ===================================================== */

char c_buf[256];
int c_idx = 0;
char *c_argv[16];
int c_argc;

#define MAX_ARGV (sizeof(c_argv) / sizeof(c_argv[0]))

void Cmd_Parse (void)
{
  char *s;

  c_argc = 0;
  for (s = c_buf; ; )
  {
    while (*s != '\0' && *s <= ' ')
      s++;
    if (*s == '\0')
    {
      /* end of input */
      break;
    }

    if (c_argc == MAX_ARGV)
    {
      /* too many args; just drop extras on the floor */
      break;
    }

    if (*s == '"')
    {
      /* a quoted token */

      c_argv[c_argc++] = ++s;
      while (*s != '\0' && *s != '"')
        s++;
      if (*s == '\0')
      {
        /* unterminated quoted token; just use what we got */
        break;
      }
      else
        *s++ = '\0';
    }
    else
    {
      /* a whitespace-delimited token */

      c_argv[c_argc++] = s++;
      while (*s != '\0' && *s > ' ')
        s++;
      if (*s == '\0')
      {
        /* end of input */
        break;
      }
      else
        *s++ = '\0';
    }
  }
}

void Cmd_Run (void)
{
  Cmd_Parse ();

  if (c_argc < 1)
    return;

  struct
  {
    const char *name;
    void (*func)(int, char **);
  } funcs[] =
  {
    { "rd", cmdRead },
    { "wr", cmdWrite },
    { "millis", cmdMillis },
    { "echo", cmdEcho },
    { "help", cmdHelp },
    //...
  };

  int i;

  for (i = 0; i < sizeof(funcs) / sizeof(funcs[0]); i++)
  {
    if (strcmp(c_argv[0], funcs[i].name) == 0)
    {
      funcs[i].func (c_argc, c_argv);
      break;
    }
  }
  
  if (i == sizeof(funcs) / sizeof(funcs[0]))
    cmdUnknown (c_argc, c_argv);
}

void Cmd_Clear (void)
{
  c_buf[(c_idx = 0)] = '\0';
}

/* ===================================================== */
/* ===================================================== */


void loop()
{
  ShowHelp ();
  
  Cmd_Clear ();
  PrintEcho ();
  
  PrintPrompt ();

  while (1)
  {
    while (Serial.available() > 0)
    {
      int c = Serial.read();

      if (c == '\r')
      {
        if (echo)
        {
          Serial.write('\r');
          Serial.write('\n');
        }
        Cmd_Run ();
        Cmd_Clear ();
        PrintPrompt ();
      }
      else if (c == 8 || c == 127) /* backspace */
      {
        if (c_idx > 0)
        {
          c_buf[--c_idx] = '\0';
          if (echo)
            Serial.write(c);
        }
      }
      else if (c_idx < sizeof(c_buf) - 1)
      {
        c_buf[c_idx++] = c;
        c_buf[c_idx] = '\0';
        if (echo)
          Serial.write(c);
      }
      else
      {
        /* input buffer full, drop excess bytes on the floor */
      }
    }
  }
}


  
  /*
//  pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
      delay(25);
      digitalWrite(pin, LOW);
  */

