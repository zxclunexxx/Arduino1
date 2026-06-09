#include <Servo.h>

#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

#define SERVO_PIN 6
#define BUTTON_PIN 2

Servo myServo;

int stepDelay = 5;
int currentStep = 0;

bool stepDirection = true;
bool servoPosition = false;

const unsigned long debounceDelay = 50;
const unsigned long doubleClickDelay = 350;

bool lastReading = HIGH;
bool buttonState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long firstClickTime = 0;

int clickCount = 0;

int steps[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  stopStepper();
}

void stepMotor(int stepNumber) {
  digitalWrite(IN1, steps[stepNumber][0]);
  digitalWrite(IN2, steps[stepNumber][1]);
  digitalWrite(IN3, steps[stepNumber][2]);
  digitalWrite(IN4, steps[stepNumber][3]);
}

void moveMotor(int direction, int stepCount) {
  for (int i = 0; i < stepCount; i++) {
    currentStep = currentStep + direction;

    if (currentStep > 7) {
      currentStep = 0;
    }

    if (currentStep < 0) {
      currentStep = 7;
    }

    stepMotor(currentStep);
    delay(stepDelay);
  }
}

void stopStepper() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void runStepper() {
  if (stepDirection == true) {
    moveMotor(1, 4096);   // вправо 360 градусов
  } else {
    moveMotor(-1, 4096);  // влево 360 градусов
  }

  stepDirection = !stepDirection;
  stopStepper();
}

void runServo() {
  if (servoPosition == false) {
    myServo.write(90);
    servoPosition = true;
  } else {
    myServo.write(0);
    servoPosition = false;
  }

  delay(500);
}

void registerClick() {
  clickCount++;

  if (clickCount == 1) {
    firstClickTime = millis();
  }

  if (clickCount == 2) {
    clickCount = 0;
    runStepper();   // двойное нажатие
  }
}

void loop() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        registerClick();
      }
    }
  }

  lastReading = reading;

  // Если было одно нажатие и второго не дождались — запускаем серво
  if (clickCount == 1 && (millis() - firstClickTime > doubleClickDelay)) {
    clickCount = 0;
    runServo();     // одиночное нажатие
  }
}
