#include <LiquidCrystal.h>
#include <Keypad.h>

// ===== PARALLEL LCD CONFIG =====
// RS, E, D4, D5, D6, D7
#define LCD_RS A0
#define LCD_E  A1
#define LCD_D4 A3
#define LCD_D5 A4
#define LCD_D6 A5
#define LCD_D7 1   // warning: pin 1 is hardware TX (Serial). Remove Serial prints or remap if you need Serial monitor.

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// ===== KEYPAD CONFIG =====
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {6, 7, 8, 9};
byte colPins[COLS] = {10, 11, 12, 13};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ===== MOTOR PIN ASSIGNMENTS =====
#define P1_STEP 2
#define P1_DIR  3
#define P2_STEP 4
#define P2_DIR  5
#define BTN_PIN A2   // Emergency Stop

// ===== PARAMETERS =====
const float syringe_d_mm = 10.0;
const float lead_pitch_mm = 2.0;
const int steps_per_rev = 200;
const int microsteps = 8;
const float min_flow = 0.1;
const float max_flow = 20.0;

enum EntryState { NONE, ENTER_FLOW, ENTER_VOL };

struct Pump {
  float flow_ml_min = 0.0;
  float volume_ml = 0.0;
  unsigned long stepsTarget = 0;
  unsigned long stepsDone = 0;
  double interval_us = 0.0;
  unsigned long lastStepTime = 0;
  bool running = false;
  EntryState entryState = NONE;
  String inputBuffer = "";
  uint8_t pinStep;
  uint8_t pinDir;
};

Pump pump1, pump2;
uint8_t selectedPump = 1;

unsigned long computeSteps(float vol_ml, float d_mm, float lead_mm) {
  float area_mm2 = 3.14159 * (d_mm/2.0) * (d_mm/2.0);
  float length_mm = (vol_ml * 1000.0) / area_mm2;
  float steps_per_mm = (steps_per_rev * microsteps) / lead_mm;
  return (unsigned long)(length_mm * steps_per_mm + 0.5);
}

double computeInterval(float flow_ml_min, float d_mm, float lead_mm) {
  float area_mm2 = 3.14159 * (d_mm/2.0) * (d_mm/2.0);
  float vol_mm3_min = flow_ml_min * 1000.0;
  float speed_mm_s = vol_mm3_min / 60.0 / area_mm2;
  float steps_per_mm = (steps_per_rev * microsteps) / lead_mm;
  float steps_per_s = speed_mm_s * steps_per_mm;
  if (steps_per_s <= 0.000001) return 1e12;
  return 1000000.0 / steps_per_s;
}

void preparePump(Pump &p) {
  p.stepsTarget = computeSteps(p.volume_ml, syringe_d_mm, lead_pitch_mm);
  p.interval_us = computeInterval(p.flow_ml_min, syringe_d_mm, lead_pitch_mm);
  p.stepsDone = 0;
  p.lastStepTime = micros();
}

void pulseStep(uint8_t pinStep) {
  digitalWrite(pinStep, HIGH);
  delayMicroseconds(2);
  digitalWrite(pinStep, LOW);
}

void stopPump(Pump &p) { p.running = false; }
void startPump(Pump &p) { p.running = true; p.lastStepTime = micros(); }

Pump& curPump() { return (selectedPump == 1) ? pump1 : pump2; }

void showStatusLCD() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("P1:");
  lcd.print(pump1.flow_ml_min,1);
  lcd.print("ml ");
  lcd.print(pump1.running ? "Run" : "Stp");
  lcd.setCursor(0,1);
  lcd.print("P2:");
  lcd.print(pump2.flow_ml_min,1);
  lcd.print("ml ");
  lcd.print(pump2.running ? "Run" : "Stp");
}

void showEntryPrompt(Pump &p) {
  lcd.clear();
  if (p.entryState == ENTER_FLOW) lcd.print("Flow (mL/min):");
  else if (p.entryState == ENTER_VOL) lcd.print("Vol (mL):");
  lcd.setCursor(0,1);
  String disp = p.inputBuffer;
  if (disp.length()==0) disp = "_";
  while (disp.length()<16) disp += " ";
  lcd.print(disp);
}

void showSelectedPumpInfo(uint8_t idx, Pump &p) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("P"); lcd.print(idx);
  lcd.print(": F:"); lcd.print(p.flow_ml_min,2);
  lcd.print(" V:"); lcd.print(p.volume_ml,2);
  lcd.setCursor(0,1);
  lcd.print(p.running ? "RUNNING" : "STOPPED");
}

void setup() {
  // If you want Serial debugging remove/comment the pin 1 LCD connection above.
  // Serial.begin(115200);

  pinMode(P1_STEP, OUTPUT);
  pinMode(P1_DIR, OUTPUT);
  pinMode(P2_STEP, OUTPUT);
  pinMode(P2_DIR, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  digitalWrite(P1_DIR, HIGH);
  digitalWrite(P2_DIR, HIGH);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Dual Syringe Pump");
  delay(800);
  showStatusLCD();

  // assign pins in structs
  pump1.pinStep = P1_STEP; pump1.pinDir = P1_DIR;
  pump2.pinStep = P2_STEP; pump2.pinDir = P2_DIR;
}

void loop() {
  if (digitalRead(BTN_PIN) == LOW) {
    stopPump(pump1); stopPump(pump2);
    lcd.clear(); lcd.print("EMERGENCY STOP");
    delay(300);
    showStatusLCD();
    while (digitalRead(BTN_PIN) == LOW) delay(10);
  }

  char key = keypad.getKey();
  Pump &p = curPump();

  if (p.entryState == ENTER_FLOW || p.entryState == ENTER_VOL) {
    if (key) {
      if (key >= '0' && key <= '9') {
        if (p.inputBuffer.length()<8) p.inputBuffer += key;
        showEntryPrompt(p);
      } else if (key == 'D') {
        if (p.inputBuffer.indexOf('.') == -1) {
          if (p.inputBuffer.length()==0) p.inputBuffer = "0.";
          else p.inputBuffer += '.';
        }
        showEntryPrompt(p);
      } else if (key == '*') { p.inputBuffer=""; showEntryPrompt(p); }
      else if (key == '#') {
        if (p.inputBuffer.length()>0) {
          float val = p.inputBuffer.toFloat();
          p.inputBuffer="";
          if (p.entryState == ENTER_FLOW) {
            if (val<min_flow || val>max_flow) {
              lcd.clear(); lcd.print("Invalid Flow"); delay(800); showEntryPrompt(p);
            } else {
              p.flow_ml_min=val; p.entryState=ENTER_VOL; showEntryPrompt(p);
            }
          } else {
            p.volume_ml=val; preparePump(p); p.entryState=NONE;
            lcd.clear(); lcd.print("Saved P"); lcd.print(selectedPump); delay(600); showStatusLCD();
          }
        } else showEntryPrompt(p);
      }
    }
    return;
  }

  if (key) {
    switch(key) {
      case '1': selectedPump=1; showSelectedPumpInfo(1,pump1); break;
      case '2': selectedPump=2; showSelectedPumpInfo(2,pump2); break;
      case 'B': p.entryState=ENTER_FLOW; p.inputBuffer=""; showEntryPrompt(p); break;
      case 'A':
        if (!p.running) {
          if (p.volume_ml<=0 || p.flow_ml_min<=0) {
            lcd.clear(); lcd.print("Set Flow & Vol"); delay(800); showStatusLCD();
          } else { preparePump(p); startPump(p); lcd.clear(); lcd.print("P"); lcd.print(selectedPump); lcd.print(" Running"); delay(300); showStatusLCD(); }
        } else { stopPump(p); lcd.clear(); lcd.print("P"); lcd.print(selectedPump); lcd.print(" Paused"); delay(300); showStatusLCD(); }
        break;
      case 'C':
        if (p.flow_ml_min>min_flow) {
          p.flow_ml_min-=0.1; if (p.flow_ml_min<min_flow) p.flow_ml_min=min_flow;
          p.interval_us=computeInterval(p.flow_ml_min,syringe_d_mm,lead_pitch_mm);
          showSelectedPumpInfo(selectedPump,p);
        }
        break;
      case 'D':
        if (p.entryState==NONE) {
          if (p.flow_ml_min<max_flow) { p.flow_ml_min+=0.1; if (p.flow_ml_min>max_flow) p.flow_ml_min=max_flow;
            p.interval_us=computeInterval(p.flow_ml_min,syringe_d_mm,lead_pitch_mm); showSelectedPumpInfo(selectedPump,p); }
        }
        break;
      case '*': showStatusLCD(); break;
    }
  }

  unsigned long now = micros();
  if (pump1.running && pump1.stepsDone<pump1.stepsTarget) {
    if ((double)(now-pump1.lastStepTime)>=pump1.interval_us) {
      pump1.lastStepTime=now; pulseStep(pump1.pinStep); pump1.stepsDone++;
      if (pump1.stepsDone>=pump1.stepsTarget) { stopPump(pump1); lcd.clear(); lcd.print("P1 Done"); delay(300); showStatusLCD(); }
    }
  }
  if (pump2.running && pump2.stepsDone<pump2.stepsTarget) {
    if ((double)(now-pump2.lastStepTime)>=pump2.interval_us) {
      pump2.lastStepTime=now; pulseStep(pump2.pinStep); pump2.stepsDone++;
      if (pump2.stepsDone>=pump2.stepsTarget) { stopPump(pump2); lcd.clear(); lcd.print("P2 Done"); delay(300); showStatusLCD(); }
    }
  }
}
