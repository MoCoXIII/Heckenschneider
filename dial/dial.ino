#include <M5Dial.h>

// ---------- Display helpers ----------
void clearScreen() {
  M5Dial.Display.fillRect(
    0, 0,
    M5Dial.Display.width(),
    M5Dial.Display.height(),
    BLACK
  );
}

void drawCenteredText(const String& text) {
  clearScreen();
  M5Dial.Display.drawString(
    text,
    M5Dial.Display.width() / 2,
    M5Dial.Display.height() / 2
  );
}

// ---------- Serial helpers ----------
void handleSerial() {
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    message.trim();

    // Handshake confirmation
    if (message == "connected") {
      Serial.println("connected");
    }

    drawCenteredText(message);
  }
}

// ---------- Setup ----------
void setup() {
  M5Dial.begin();
  Serial.begin(115200);

  M5Dial.Display.setTextColor(ORANGE);
  M5Dial.Display.setTextDatum(middle_center);
  M5Dial.Display.setTextFont(&fonts::Orbitron_Light_24);
  M5Dial.Display.setTextSize(1);

  drawCenteredText("connecting...");

  while (!Serial) {
    delay(10);
  }
}

// ---------- Loop ----------
void loop() {
  M5Dial.update();

  handleSerial();

  if (M5Dial.BtnA.wasPressed()) {
    Serial.println("Button pressed");
  }
}
