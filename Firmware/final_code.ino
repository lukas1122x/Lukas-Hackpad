#include <Keyboard.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int keys[9] = {28, 1, 2, 29, 27, 4, 0, 26, 3};
bool lastState[9];
int layer = 0; 
unsigned long lastUpdate = 0;
int animFrame = 0;

void setup() {
  Keyboard.begin();
  Wire.setSDA(6);
  Wire.setSCL(7);
  Wire.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      for(;;); 
    }
  }

  for (int i = 0; i < 9; i++) {
    pinMode(keys[i], INPUT_PULLUP);
    lastState[i] = HIGH;
  }

  // Úvodná animácia loga
  introAnimation();
}

void loop() {
  bool actionTriggered = false;

  for (int i = 0; i < 9; i++) {
    bool current = digitalRead(keys[i]);
    if (lastState[i] == HIGH && current == LOW) {
      handleKey(i);
      actionTriggered = true;
      // Krátka vizuálna odozva pri stlačení
      display.invertDisplay(true);
      delay(100);
      display.invertDisplay(false);
    }
    lastState[i] = current;
  }

  // Rýchlejšia aktualizácia pre plynulú animáciu (každých 50ms)
  if (millis() - lastUpdate > 50) {
    updateOLED();
    lastUpdate = millis();
    animFrame++;
  }
}

void handleKey(int i) {
  if (i == 0) {
    layer++;
    if (layer > 3) layer = 0; // Zvýšené na 3 vrstvy
    return;
  }
  
  if (layer == 0) { 
    // Vrstva 0: Písmená
    char letters[8] = {'A','B','C','D','E','F','G','H'};
    Keyboard.write(letters[i-1]);
  } else if (layer == 1) {
    // Vrstva 1: Čísla
    char numbers[8] = {'1','2','3','4','5','6','7','8'};
    Keyboard.write(numbers[i-1]);
  } else if (layer == 2) {
    // Vrstva 2: Základné makrá
    if(i==1) { Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('c'); delay(10); Keyboard.releaseAll(); }
    if(i==2) { Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('v'); delay(10); Keyboard.releaseAll(); }
    if(i==3) { Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('z'); delay(10); Keyboard.releaseAll(); }
    if(i==4) { Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('y'); delay(10); Keyboard.releaseAll(); }
    if(i==5) Keyboard.write(KEY_F5);
    if(i==6) { Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('s'); delay(10); Keyboard.releaseAll(); }
    if(i==7) { Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('a'); delay(10); Keyboard.releaseAll(); }
    if(i==8) { Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('f'); delay(10); Keyboard.releaseAll(); }
  } else if (layer == 3) {
    // Vrstva 3: Fusion 360 skratky
    switch(i) {
      case 1: // L - Line
        Keyboard.write('L');
        break;
      case 2: // C - Circle
        Keyboard.write('C');
        break;
      case 3: // R - Rectangle
        Keyboard.write('R');
        break;
      case 4: // E - Extrude
        Keyboard.write('E');
        break;
      case 5: // S - Sketch
        Keyboard.write('S');
        break;
      case 6: // F - Fillet
        Keyboard.write('F');
        break;
      case 7: // Esc - Exit tool
        Keyboard.write(KEY_ESC);
        break;
      case 8: // Space - Repeat last command
        Keyboard.write(' ');
        break;
    }
  }
}

void introAnimation() {
  display.clearDisplay();
  for(int i=0; i<SCREEN_WIDTH; i+=4) {
    display.drawLine(0, 0, i, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
    delay(10);
  }
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 10);
  display.print("READY!");
  display.display();
  delay(1000);
}

void updateOLED() {
  display.clearDisplay();
  
  // 1. Dekoratívna vlnovka (animácia pozadia)
  for (int x = 0; x < 128; x += 8) {
    int y = 30 + sin((animFrame + x) * 0.2) * 2;
    display.drawPixel(x, y, SSD1306_WHITE);
  }

  // 2. Indikátor vrstvy
  display.setCursor(5, 2);
  display.print("LAYER ");
  for(int b=0; b<4; b++) { // Zmenené na 4 vrstvy
    if(b == layer) display.fillRect(40 + (b*8), 2, 6, 6, SSD1306_WHITE);
    else display.drawRect(40 + (b*8), 2, 6, 6, SSD1306_WHITE);
  }

  // 3. Hlavný text s efektom "trasenia" pri stlačení
  display.setTextSize(2);
  display.setCursor(5, 14);
  
  switch(layer) {
    case 0: display.print("LETTERS"); break;
    case 1: display.print("NUMBERS"); break;
    case 2: display.print(" MACROS"); break;
    case 3: display.print("FUSION"); break; // Nová vrstva
  }

  // 4. Názvy kláves pre aktuálnu vrstvu (iba pre Fusion 360)
  if (layer == 3) {
    display.setTextSize(1);
    display.setCursor(0, 25);
    display.print("L C R E S F ESC SP");
  }

  display.display();
}