#include <LineReceiver.h>

void lineReceived(const char* line)
{
  Serial.printf("callback; Line: %s\n", line);
}

void setup()
{
}

void loop()
{
  // switches back and forth between two line receiver object, one with a callback installed

  static LineReceiver line(10);
  static LineReceiver lineCallback(10, lineReceived);
  static LineReceiver* p = &line;
  if (Serial.available())
  {
    if (p->push(Serial.read()))
    {
      if(p == &line)
      {
        Serial.printf("Line: %s\n", p->get());
        p = &lineCallback;
      }
      else
      {
        p = &line;
      }
    }
  }
}
