#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// ============================================================
// DISPLAY
// ============================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);


// ============================================================
// PINS
// ============================================================

const int BUTTON_PIN = 12;
const int POTI_PIN = A0;
const int BUZZER_PIN = 9;
const int VIBRATION_PIN = 6;
const int SOUND_SWITCH_PIN = 10;
const int VIBRATION_SWITCH_PIN = 11;


// ============================================================
// TIME CONSTANTS
// ============================================================

const unsigned long SECOND = 1000UL;
const unsigned long MINUTE = 60000UL;

const unsigned long STARTUP_DURATION = 1500UL;
const unsigned long COMPLETE_SCREEN_DURATION = 3000UL;

const unsigned long BUTTON_DEBOUNCE_TIME = 40UL;
const unsigned long VIBRATION_DURATION = 300UL;


// ============================================================
// STATES
// ============================================================

enum class State {
  STARTUP,
  MENU,
  COUNTDOWN,
  WORK,
  BREAK,
  CANCEL_CONFIRM,
  SESSION_COMPLETE
};

State currentState = State::STARTUP;
State previousState = State::MENU;


// ============================================================
// MENU
// ============================================================

enum class MenuPage {
  WORK_TIME,
  BREAK_TIME,
  BREAK_COUNT,
  READY
};

MenuPage currentMenuPage = MenuPage::WORK_TIME;


// ============================================================
// TIMER
// ============================================================

unsigned long totalWorkTime = 0;
unsigned long workSessionTime = 0;
unsigned long breakTime = 0;

unsigned long phaseStartTime = 0;
unsigned long phaseDuration = 0;

int numberOfBreaks = 1;
int currentWorkSession = 1;


// ============================================================
// POTENTIOMETER
// ============================================================

long smoothedPoti = -1;
int potiPercent = 0;


// ============================================================
// BUTTON
// ============================================================

bool buttonStableState = HIGH;
bool lastButtonReading = HIGH;

unsigned long lastButtonChangeTime = 0;

bool buttonPressedEvent = false;


// ============================================================
// VIBRATION
// ============================================================

bool vibrationActive = false;
unsigned long vibrationStartTime = 0;

bool halfwayFeedbackTriggered = false;


// ============================================================
// SPECIAL SCREENS
// ============================================================

unsigned long specialScreenStartTime = 0;


// ============================================================
// COUNTDOWN
// ============================================================

unsigned long countdownStartTime = 0;
int lastCountdownValue = -1;


// ============================================================
// CANCEL CONFIRMATION
// ============================================================

bool cancelSelection = false;


// ============================================================
// SOUND SYSTEM
// ============================================================

struct SoundNote {
  int frequency;
  unsigned long duration;
  unsigned long pause;
};

const int MAX_SOUND_NOTES = 4;

SoundNote soundPattern[MAX_SOUND_NOTES];

int soundNoteCount = 0;
int currentSoundNote = 0;

bool soundPlaying = false;

unsigned long soundNoteStartTime = 0;


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(VIBRATION_PIN, OUTPUT);

  pinMode(SOUND_SWITCH_PIN, INPUT_PULLUP);
  pinMode(VIBRATION_SWITCH_PIN, INPUT_PULLUP);

  digitalWrite(VIBRATION_PIN, LOW);

  display.begin(
    SSD1306_SWITCHCAPVCC,
    0x3C
  );

  display.clearDisplay();
  display.display();

  specialScreenStartTime = millis();

  startSoundPattern(
    700,
    100
  );
}


// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

  updateButton();
  readPotentiometer();

  updateVibration();
  updateSound();

  switch (currentState) {

    case State::STARTUP:
      handleStartup();
      break;

    case State::MENU:
      handleMenu();
      break;

    case State::COUNTDOWN:
      handleCountdown();
      break;

    case State::WORK:
      handleWork();
      break;

    case State::BREAK:
      handleBreak();
      break;

    case State::CANCEL_CONFIRM:
      handleCancelConfirm();
      break;

    case State::SESSION_COMPLETE:
      handleSessionComplete();
      break;
  }
}


// ============================================================
// BUTTON HANDLING
// ============================================================

void updateButton() {

  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastButtonChangeTime = millis();
  }

  if (
    millis() - lastButtonChangeTime
    >= BUTTON_DEBOUNCE_TIME
  ) {

    if (reading != buttonStableState) {

      buttonStableState = reading;

      if (buttonStableState == LOW) {
        buttonPressedEvent = true;
      }
    }
  }

  lastButtonReading = reading;
}


bool buttonPressed() {

  if (buttonPressedEvent) {

    buttonPressedEvent = false;

    return true;
  }

  return false;
}


// ============================================================
// POTENTIOMETER
// ============================================================

void readPotentiometer() {

  int raw = analogRead(POTI_PIN);

  if (smoothedPoti < 0) {
    smoothedPoti = raw;
  }

  smoothedPoti =
    (smoothedPoti * 7 + raw) / 8;

  potiPercent =
    map(
      smoothedPoti,
      0,
      1023,
      0,
      100
    );
}


// ============================================================
// STARTUP
// ============================================================

void handleStartup() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(36, 12);

  display.print("POMI");

  display.setTextSize(1);
  display.setCursor(28, 38);

  display.print("Focus better.");

  display.display();


  if (
    millis() - specialScreenStartTime
    >= STARTUP_DURATION
  ) {

    currentMenuPage =
      MenuPage::WORK_TIME;

    currentState =
      State::MENU;
  }
}


// ============================================================
// MENU
// ============================================================

void handleMenu() {

  switch (currentMenuPage) {

    case MenuPage::WORK_TIME:
      handleWorkTimeMenu();
      break;

    case MenuPage::BREAK_TIME:
      handleBreakTimeMenu();
      break;

    case MenuPage::BREAK_COUNT:
      handleBreakCountMenu();
      break;

    case MenuPage::READY:
      handleReadyMenu();
      break;
  }
}


// ============================================================
// WORK TIME SELECTION
// ============================================================

void handleWorkTimeMenu() {

  int selectedMinutes =
    map(
      potiPercent,
      0,
      100,
      5,
      480
    );

  selectedMinutes =
    (selectedMinutes / 5) * 5;

  totalWorkTime =
    (unsigned long)selectedMinutes * MINUTE;


  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);

  display.print("TOTAL WORK TIME");

  display.setTextSize(2);
  display.setCursor(25, 20);

  display.print(selectedMinutes);
  display.print(" min");

  display.setTextSize(1);
  display.setCursor(8, 51);

  display.print("Turn to adjust");

  display.display();


  if (buttonPressed()) {

    playButtonSound();

    currentMenuPage =
      MenuPage::BREAK_TIME;
  }
}


// ============================================================
// BREAK TIME SELECTION
// ============================================================

void handleBreakTimeMenu() {

  int selectedMinutes =
    map(
      potiPercent,
      0,
      100,
      1,
      60
    );

  breakTime =
    (unsigned long)selectedMinutes * MINUTE;


  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);

  display.print("BREAK TIME");

  display.setTextSize(2);
  display.setCursor(35, 18);

  display.print(selectedMinutes);
  display.print(" min");

  display.setTextSize(1);
  display.setCursor(8, 51);

  display.print("Turn to adjust");

  display.display();


  if (buttonPressed()) {

    playButtonSound();

    currentMenuPage =
      MenuPage::BREAK_COUNT;
  }
}


// ============================================================
// BREAK COUNT / WORK SESSION CALCULATION
// ============================================================

void handleBreakCountMenu() {

  numberOfBreaks =
    map(
      potiPercent,
      0,
      100,
      1,
      20
    );


  // The selected number determines
  // how many work sessions the total
  // work time is divided into.

  workSessionTime =
    totalWorkTime / numberOfBreaks;


  unsigned long sessionMinutes =
    workSessionTime / MINUTE;


  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(0, 0);

  display.print("NUMBER OF BREAKS: ");
  display.print(numberOfBreaks);


  display.setCursor(0, 14);

  display.print(
    totalWorkTime / MINUTE
  );

  display.print(" min / ");

  display.print(numberOfBreaks);

  display.print(" =");


  display.setTextSize(2);

  display.setCursor(20, 28);

  display.print(sessionMinutes);
  display.print(" min");


  display.setTextSize(1);

  display.setCursor(8, 51);

  display.print("Press to continue");


  display.display();


  if (buttonPressed()) {

    playButtonSound();

    currentMenuPage =
      MenuPage::READY;
  }
}


// ============================================================
// READY SCREEN
// ============================================================

void handleReadyMenu() {

  unsigned long sessionMinutes =
    workSessionTime / MINUTE;

  unsigned long breakMinutes =
    breakTime / MINUTE;


  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(43, 0);

  display.print("READY");


  display.setCursor(10, 16);

  display.print(
    numberOfBreaks
  );

  display.print(" x ");

  display.print(sessionMinutes);

  display.print(" min work");


  display.setCursor(10, 29);

  display.print(
    breakMinutes
  );

  display.print(" min breaks");


  display.setCursor(10, 51);

  display.print("Press to start");


  display.display();


  if (buttonPressed()) {

    playButtonSound();

    startCountdown();
  }
}


// ============================================================
// COUNTDOWN
// ============================================================

void startCountdown() {

  countdownStartTime =
    millis();

  lastCountdownValue = -1;

  currentState =
    State::COUNTDOWN;
}


void handleCountdown() {

  unsigned long elapsed =
    millis() - countdownStartTime;


  int countdownValue =
    3 - (elapsed / SECOND);


  if (countdownValue < 1) {

    startWorkSession();

    return;
  }


  if (
    countdownValue !=
    lastCountdownValue
  ) {

    lastCountdownValue =
      countdownValue;

    playCountdownSound(
      countdownValue
    );
  }


  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(38, 5);

  display.print("GET READY");


  display.setTextSize(4);

  display.setCursor(50, 20);

  display.print(
    countdownValue
  );

  display.display();
}


// ============================================================
// START WORK SESSION
// ============================================================

void startWorkSession() {

  currentWorkSession = 1;

  phaseDuration =
    workSessionTime;

  phaseStartTime =
    millis();

  halfwayFeedbackTriggered =
    false;

  currentState =
    State::WORK;

  playGoSound();
}


// ============================================================
// WORK
// ============================================================

void handleWork() {

  unsigned long elapsed =
    millis() - phaseStartTime;


  if (buttonPressed()) {

    openCancelConfirmation();

    return;
  }


  handleHalfwayFeedback(
    phaseDuration,
    elapsed
  );


  unsigned long remaining =
    getRemainingTime(
      phaseDuration,
      elapsed
    );


  drawWorkScreen(
    remaining
  );


  if (elapsed >= phaseDuration) {

    playWorkCompleteSound();


    if (
      currentWorkSession >=
      numberOfBreaks
    ) {

      completeSession();

    } else {

      startBreak();
    }
  }
}


// ============================================================
// BREAK
// ============================================================

void startBreak() {

  phaseStartTime =
    millis();

  phaseDuration =
    breakTime;

  halfwayFeedbackTriggered =
    false;

  currentState =
    State::BREAK;
}


void handleBreak() {

  unsigned long elapsed =
    millis() - phaseStartTime;


  if (buttonPressed()) {

    openCancelConfirmation();

    return;
  }


  handleHalfwayFeedback(
    phaseDuration,
    elapsed
  );


  unsigned long remaining =
    getRemainingTime(
      phaseDuration,
      elapsed
    );


  drawBreakScreen(
    remaining
  );


  if (elapsed >= phaseDuration) {

    playBreakCompleteSound();

    currentWorkSession++;

    startNextWorkSession();
  }
}


void startNextWorkSession() {

  phaseStartTime =
    millis();

  phaseDuration =
    workSessionTime;

  halfwayFeedbackTriggered =
    false;

  currentState =
    State::WORK;

  playGoSound();
}


// ============================================================
// CANCEL CONFIRMATION
// ============================================================

void openCancelConfirmation() {

  previousState =
    currentState;

  cancelSelection =
    false;

  playButtonSound();

  currentState =
    State::CANCEL_CONFIRM;
}


void handleCancelConfirm() {

  // Left half = NO
  // Right half = YES

  cancelSelection =
    potiPercent >= 50;


  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(22, 0);

  display.print("END SESSION?");


  display.setTextSize(2);


  if (cancelSelection) {

    display.setCursor(5, 20);
    display.print("NO");

    display.setCursor(73, 20);
    display.print("YES");

    display.drawRect(
      68,
      17,
      55,
      25,
      SSD1306_WHITE
    );

  } else {

    display.setCursor(5, 20);
    display.print("NO");

    display.drawRect(
      0,
      17,
      50,
      25,
      SSD1306_WHITE
    );

    display.setCursor(73, 20);
    display.print("YES");
  }


  display.setTextSize(1);

  display.setCursor(18, 51);

  display.print("Turn + press");

  display.display();


  if (buttonPressed()) {

    if (cancelSelection) {

      playCancelSound();

      currentMenuPage =
        MenuPage::WORK_TIME;

      currentState =
        State::MENU;

    } else {

      playButtonSound();

      currentState =
        previousState;
    }
  }
}


// ============================================================
// SESSION COMPLETE
// ============================================================

void completeSession() {

  playSessionCompleteSound();

  specialScreenStartTime =
    millis();

  currentState =
    State::SESSION_COMPLETE;
}


void handleSessionComplete() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(30, 0);

  display.print("SESSION COMPLETE");


  display.setTextSize(2);

  display.setCursor(25, 18);

  display.print("WELL DONE");


  display.setTextSize(1);

  display.setCursor(18, 43);

  display.print(
    totalWorkTime / MINUTE
  );

  display.print(" min focused");


  display.display();


  if (
    millis() -
    specialScreenStartTime
    >= COMPLETE_SCREEN_DURATION
  ) {

    currentMenuPage =
      MenuPage::WORK_TIME;

    currentState =
      State::MENU;
  }
}


// ============================================================
// TIMER HELPERS
// ============================================================

unsigned long getRemainingTime(
  unsigned long duration,
  unsigned long elapsed
) {

  if (elapsed >= duration) {
    return 0;
  }

  return duration - elapsed;
}


// ============================================================
// WORK SCREEN
// ============================================================

void drawWorkScreen(
  unsigned long remaining
) {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(0, 0);

  display.print("WORK ");

  display.print(currentWorkSession);

  display.print("/");

  display.print(numberOfBreaks);


  display.setTextSize(3);

  display.setCursor(20, 15);

  printMinSec(
    remaining / SECOND
  );


  drawSessionProgressBar();

  display.display();
}


// ============================================================
// BREAK SCREEN
// ============================================================

void drawBreakScreen(
  unsigned long remaining
) {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(0, 0);

  display.print("BREAK AFTER ");

  display.print(currentWorkSession);


  display.setTextSize(3);

  display.setCursor(20, 15);

  printMinSec(
    remaining / SECOND
  );


  drawSessionProgressBar();

  display.display();
}


// ============================================================
// SESSION PROGRESS
// ============================================================

void drawSessionProgressBar() {

  int totalUnits =
    numberOfBreaks;

  int completedUnits =
    currentWorkSession - 1;


  int currentProgress =
    map(
      completedUnits,
      0,
      totalUnits,
      0,
      120
    );


  unsigned long elapsed =
    millis() - phaseStartTime;


  int currentPhaseProgress = 0;


  if (phaseDuration > 0) {

    currentPhaseProgress =
      map(
        constrain(
          elapsed,
          0,
          phaseDuration
        ),
        0,
        phaseDuration,
        0,
        120 / totalUnits
      );
  }


  int barWidth =
    currentProgress +
    currentPhaseProgress;


  barWidth =
    constrain(
      barWidth,
      0,
      120
    );


  display.drawRect(
    4,
    50,
    120,
    10,
    SSD1306_WHITE
  );

  display.fillRect(
    4,
    50,
    barWidth,
    10,
    SSD1306_WHITE
  );
}


// ============================================================
// HALF-WAY FEEDBACK
// ============================================================

void handleHalfwayFeedback(
  unsigned long duration,
  unsigned long elapsed
) {

  // Very short sessions do not receive
  // halfway feedback.

  if (
    duration <
    10 * MINUTE
  ) {
    return;
  }


  if (
    !halfwayFeedbackTriggered &&
    elapsed >= duration / 2
  ) {

    triggerVibration();

    playHalfwaySound();

    halfwayFeedbackTriggered =
      true;
  }
}


// ============================================================
// VIBRATION
// ============================================================

bool vibrationEnabled() {

  return
    digitalRead(
      VIBRATION_SWITCH_PIN
    ) == LOW;
}


void triggerVibration() {

  if (
    vibrationEnabled() &&
    !vibrationActive
  ) {

    digitalWrite(
      VIBRATION_PIN,
      HIGH
    );

    vibrationActive = true;

    vibrationStartTime =
      millis();
  }
}


void updateVibration() {

  if (
    vibrationActive &&
    millis() -
      vibrationStartTime
      >= VIBRATION_DURATION
  ) {

    digitalWrite(
      VIBRATION_PIN,
      LOW
    );

    vibrationActive = false;
  }
}


// ============================================================
// SOUND SYSTEM
// ============================================================

bool soundEnabled() {

  return
    digitalRead(
      SOUND_SWITCH_PIN
    ) == LOW;
}


void startSoundPattern(
  int frequency1,
  unsigned long duration1
) {

  soundNoteCount = 1;

  soundPattern[0].frequency =
    frequency1;

  soundPattern[0].duration =
    duration1;

  soundPattern[0].pause =
    0;

  currentSoundNote = 0;

  soundPlaying = false;

  soundNoteStartTime =
    millis();
}


void startTwoNotePattern(
  int frequency1,
  unsigned long duration1,
  unsigned long pause1,
  int frequency2,
  unsigned long duration2
) {

  soundNoteCount = 2;

  soundPattern[0] = {
    frequency1,
    duration1,
    pause1
  };

  soundPattern[1] = {
    frequency2,
    duration2,
    0
  };

  currentSoundNote = 0;

  soundPlaying = false;

  soundNoteStartTime =
    millis();
}


void startThreeNotePattern(
  int frequency1,
  unsigned long duration1,
  unsigned long pause1,
  int frequency2,
  unsigned long duration2,
  unsigned long pause2,
  int frequency3,
  unsigned long duration3
) {

  soundNoteCount = 3;

  soundPattern[0] = {
    frequency1,
    duration1,
    pause1
  };

  soundPattern[1] = {
    frequency2,
    duration2,
    pause2
  };

  soundPattern[2] = {
    frequency3,
    duration3,
    0
  };

  currentSoundNote = 0;

  soundPlaying = false;

  soundNoteStartTime =
    millis();
}


void updateSound() {

  if (!soundEnabled()) {

    noTone(BUZZER_PIN);

    soundPlaying = false;

    return;
  }


  if (
    currentSoundNote >=
    soundNoteCount
  ) {

    soundPlaying = false;

    return;
  }


  SoundNote &note =
    soundPattern[currentSoundNote];


  if (!soundPlaying) {

    unsigned long requiredPause = 0;

    if (currentSoundNote > 0) {

      requiredPause =
        soundPattern[
          currentSoundNote - 1
        ].pause;
    }


    if (
      millis() -
      soundNoteStartTime
      >= requiredPause
    ) {

      tone(
        BUZZER_PIN,
        note.frequency,
        note.duration
      );

      soundPlaying = true;

      soundNoteStartTime =
        millis();
    }

    return;
  }


  if (
    millis() -
    soundNoteStartTime
    >= note.duration
  ) {

    noTone(BUZZER_PIN);

    soundPlaying = false;

    soundNoteStartTime =
      millis();

    currentSoundNote++;

    if (
      currentSoundNote >=
      soundNoteCount
    ) {

      soundPlaying = false;
    }
  }
}


// ============================================================
// SOUND PATTERNS
// ============================================================

void playButtonSound() {

  startSoundPattern(
    600,
    70
  );
}


void playCountdownSound(
  int number
) {

  if (number == 1) {

    startSoundPattern(
      1000,
      150
    );

  } else {

    startSoundPattern(
      700,
      80
    );
  }
}


void playGoSound() {

  startSoundPattern(
    1200,
    250
  );
}


void playHalfwaySound() {

  startSoundPattern(
    800,
    100
  );
}


void playWorkCompleteSound() {

  startTwoNotePattern(
    1000,
    120,
    80,
    1300,
    180
  );
}


void playBreakCompleteSound() {

  startThreeNotePattern(
    700,
    100,
    70,
    900,
    100,
    70,
    1200,
    180
  );
}


void playSessionCompleteSound() {

  startThreeNotePattern(
    800,
    100,
    80,
    1000,
    100,
    80,
    1300,
    250
  );
}


void playCancelSound() {

  startSoundPattern(
    400,
    180
  );
}


// ============================================================
// TIME DISPLAY
// ============================================================

void printMinSec(
  unsigned long totalSeconds
) {

  int minutes =
    totalSeconds / 60;

  int seconds =
    totalSeconds % 60;


  if (minutes < 10) {
    display.print("0");
  }

  display.print(minutes);

  display.print(":");


  if (seconds < 10) {
    display.print("0");
  }

  display.print(seconds);
}
