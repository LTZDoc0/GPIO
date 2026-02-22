#define BUF_SIZE 8

char cmd[BUF_SIZE];
byte idx = 0;

// Flags per i pin settati come analogico  (solo PC0–PC5)
bool analogMode[3][8] = {0}; 
// [0]=B, [1]=C, [2]=D

int getArduinoPin(char port, int pin)
{
  if (port == 'b' && pin <= 7) return 8 + pin;      // PB0–PB7
  if (port == 'c' && pin <= 5) return 14 + pin;     // PC0–PC5 (A0–A5)
  if (port == 'd' && pin <= 7) return pin;          // PD0–PD7
  return -1;
}

int portIndex(char port)
{
  if (port == 'b') return 0;
  if (port == 'c') return 1;
  if (port == 'd') return 2;
  return -1;
}

void setup()
 {
  Serial.begin(115200);
}

void loop()
{
  while (Serial.available())
  {
    char c = Serial.read();
    Serial.write(c);  // eco

    if (c == '\r' || c == '\n')
   {
      if (idx > 0)
      {
        cmd[idx] = '\0';
        processCommand(cmd);
        idx = 0;
      }
    }
    else
  {
      if (idx < BUF_SIZE - 1)
      {
        cmd[idx++] = c;
      } else {
        idx = 0;
        Serial.println("err");
      }
    }
  }
}

void processCommand(char *s)
{

  if (s[0] != 'p')
  {
    Serial.println("err");
    return;
  }

  char port = s[1];
  char pinChar = s[2];

  if (pinChar < '0' || pinChar > '7')
  {
    Serial.println("err");
    return;
  }

  int pin = pinChar - '0';
  int aPin = getArduinoPin(port, pin);
  int pIndex = portIndex(port);

  if (aPin < 0 || pIndex < 0)
  {
    Serial.println("err");
    return;
  }

  // ------------------------
  // LETTURA
  // ------------------------
  if (s[3] == '?' && s[4] == '\0')
  {

    if (analogMode[pIndex][pin])
    {
      int val = analogRead(aPin);
      Serial.print("p");
      Serial.print(port);
      Serial.print(pin);
      Serial.print("=");
      Serial.println(val);
    }
    else
   {
      int val = digitalRead(aPin);
      Serial.print("p");
      Serial.print(port);
      Serial.print(pin);
      Serial.print("=");
      Serial.println(val);
    }
    return;
  }

  // ------------------------
  // SCRITTURA / CONFIG
  // ------------------------
  if (s[3] != ',' || s[5] != '\0')
  {
    Serial.println("err");
    return;
  }

  char mode = s[4];

  if (mode == 'i')
  {
    pinMode(aPin, INPUT);
    analogMode[pIndex][pin] = false;
  }
  else if (mode == '0')
  {
    pinMode(aPin, OUTPUT);
    digitalWrite(aPin, LOW);
    analogMode[pIndex][pin] = false;
  }
  else if (mode == '1')
  {
    pinMode(aPin, OUTPUT);
    digitalWrite(aPin, HIGH);
    analogMode[pIndex][pin] = false;
  }
  else if (mode == 'a' && port == 'c' && pin <= 5)
  {
    analogMode[pIndex][pin] = true;
  }
  else
  {
    Serial.println("err");
    return;
  }

  // Risposta sullo stato attuale
  if (analogMode[pIndex][pin])
  {
    int val = analogRead(aPin);
    Serial.print("p");
    Serial.print(port);
    Serial.print(pin);
    Serial.print("=");
    Serial.println(val);
  }
  else
  {
    int val = digitalRead(aPin);
    Serial.print("p");
    Serial.print(port);
    Serial.print(pin);
    Serial.print("=");
    Serial.println(val);
  }
}
