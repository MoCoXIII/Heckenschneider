#include <M5Dial.h>

void setup() {
  M5Dial.begin();
  Serial.begin(115200);

  M5Dial.Display.setTextColor(ORANGE);
  M5Dial.Display.setTextDatum(middle_center);
  M5Dial.Display.setTextFont(&fonts::Orbitron_Light_24);
  M5Dial.Display.setTextSize(1);

  M5Dial.Display.fillRect(0, 0, M5Dial.Display.width(),
                          M5Dial.Display.height(), BLACK);
  M5Dial.Display.drawString("connecting...", M5Dial.Display.width() / 2,
                            M5Dial.Display.height() / 2);
  
  while (!Serial) {
    delay(10);
  }
}

void loop() {
  M5Dial.update();

  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    if (message == "connected") {
      Serial.println("connected");
    }
    M5Dial.Display.fillRect(0, 0, M5Dial.Display.width(),
                            M5Dial.Display.height(), BLACK);
    M5Dial.Display.drawString(message, M5Dial.Display.width() / 2,
                              M5Dial.Display.height() / 2);
  }

  if (M5Dial.BtnA.wasPressed()) {
    Serial.println("Button pressed");
  }
}
