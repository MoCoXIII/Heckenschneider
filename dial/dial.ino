#include <M5Dial.h>

// ---------- Menu ----------
enum Menu {
  SERVO,
  LIFT,
  DRIVE
};

Menu currentMenu = SERVO;

enum UiMode {
  MENU_SELECTION,
  MENU_ACTION
};

UiMode currentMode = MENU_SELECTION;

// ---------- Layout ----------
int screenW, screenH;
int statusY;

// ---------- Helpers ----------
const char* menuName(Menu m) {
  switch (m) {
    case SERVO: return "SERVO";
    case LIFT: return "LIFT";
    case DRIVE: return "DRIVE";
  }
  return "";
}

Menu prevMenu(Menu m) {
  return (m == SERVO) ? DRIVE : (Menu)(m - 1);
}

Menu nextMenu(Menu m) {
  return (m == DRIVE) ? SERVO : (Menu)(m + 1);
}

// ---------- Display helpers ----------
void clearScreen() {
  M5Dial.Display.fillRect(0, 0, screenW, screenH, BLACK);
}

void drawStatusText(const String& text) {
  M5Dial.Display.setTextDatum(middle_center);
  M5Dial.Display.drawString(text, screenW / 2, statusY);
}

void drawMenuLabel(const char* text, int x, int y, float angle, bool arrowLeft) {
  uint16_t color = (currentMode == MENU_SELECTION) ? ORANGE : DARKGREY;

  drawRotatedLabel(text, x, y, angle, color);

  int h = 16;
  int w = 10;
  int cx = (x - screenW / 2) / 4 + screenW / 2;
  int cy = screenH - 10;

  if (arrowLeft) {
    M5Dial.Display.fillTriangle(
      cx, cy,
      cx + w, cy - h / 2,
      cx + w, cy + h / 2,
      color);
  } else {
    M5Dial.Display.fillTriangle(
      cx, cy,
      cx - w, cy - h / 2,
      cx - w, cy + h / 2,
      color);
  }
}

void redrawUI(const String& status) {
  clearScreen();
  drawStatusText(status);

  int bottomY = screenH - 50;

  // Left (CCW)
  drawMenuLabel(
    menuName(prevMenu(currentMenu)),
    50,
    bottomY,
    45,
    true);

  // Right (CW)
  drawMenuLabel(
    menuName(nextMenu(currentMenu)),
    screenW - 50,
    bottomY,
    -45,
    false);
}

// ---------- Serial ----------
void handleSerial(String& status) {
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    message.trim();

    if (message == "connected") {
      Serial.println("connected");
    }

    status = message;
    redrawUI(status);
  }
}

LGFX_Sprite label(&M5Dial.Display);

void drawRotatedLabel(const char* text, int x, int y, int angle, uint16_t color) {
  label.createSprite(100, 40);
  label.fillSprite(BLACK);
  label.setTextDatum(middle_center);
  label.setTextColor(color);
  label.setTextFont(&fonts::Orbitron_Light_24);
  label.drawString(text, 50, 20);

  label.setPivot(50, 20);
  label.pushRotateZoom(x, y, angle, 1.0, 1.0, BLACK);

  label.deleteSprite();
}

void setup() {
  M5Dial.begin(true);
  Serial.begin(115200);

  screenW = M5Dial.Display.width();
  screenH = M5Dial.Display.height();
  statusY = screenH / 4;

  M5Dial.Display.setTextFont(&fonts::Orbitron_Light_24);
  M5Dial.Display.setTextSize(1);
  M5Dial.Display.setTextColor(ORANGE);
  M5Dial.Display.setTextDatum(middle_center);

  redrawUI("connecting...");

  while (!Serial) {
    delay(10);
  }
}

// ---------- Loop ----------
void loop() {
  M5Dial.update();

  static String statusText = "connecting...";
  handleSerial(statusText);

  if (M5Dial.BtnA.wasPressed()) {
    currentMode = (currentMode == MENU_SELECTION) ? MENU_ACTION : MENU_SELECTION;
    redrawUI(menuName(currentMenu));
  }

  if (currentMode == MENU_SELECTION) {
    long position = M5Dial.Encoder.read();
    int step = 3;

    if (!(-step <= position && position <= step)) {
      if (position < -step) {
        currentMenu = nextMenu(currentMenu);
      } else if (position > step) {
        currentMenu = prevMenu(currentMenu);
      }

      M5Dial.Encoder.write(0);
      redrawUI(menuName(currentMenu));
    }
  }
}
