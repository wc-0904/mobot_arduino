// variables
int distVal = 8.5;
int distVal2 = 19;
int distVal3 = 12.7;
int turnSpeed = 90;
int forwardSpeed = turnSpeed - 15;
int backwardSpeed = 65;
int desiredLine = 2;
const int noWall = 0;
const int goodDist = 1;
const int tooFar = 2;
const int tooClose = 3;
const int rev = 4;
bool lineFound = 0;
bool doorFound = 0;
bool endGame = 0;
int count = 0;
int doorNum = 0;
int countNum = 150;

#include <NewPing.h>
#include <Servo.h>

// Define all of the pins used.  These are NOT up to us, but rather what Elegoo decided.  Don't change.
// If your car doesn't function properly  with these pins, please double check your pin connections.
const int LEFT_MOTORS = 5; // pin that controls the speed of the 2 left side motors - PWM pin
const int RIGHT_MOTORS = 6; // pin that controls the speed of the 2 right side motors - PWM pin
const int LEFT_MOTOR_DIR = 8; // pin that controls the polarity, or direction, of the 2 left side motors (HIGH = forward, LOW = backward)
const int RIGHT_MOTOR_DIR = 7; // pin that controls the polarity, or direction, of the 2 right side motors (HIGH = forward, LOW = backward)
const int MOTOR_OUTPUT = 3; // controls whether the motors are allowed to run or not

// IR Detector Pins
const int RIGHT = A0;
const int MIDDLE = A1;
const int LEFT = A2;

// Servo & Sonar Pins, Servo naming
const int ServoPin = 10;
const int TriggerPin = 13; //digi pin
const int EchoPin = 12; //digi pin with PWM
const int Servo2Pin = 11;

// assign servos
NewPing sonar (TriggerPin, EchoPin, 30);
Servo servo; // Ultrasonic servo
Servo servo2; // boat-dropping servo

// Threshold values for IR detection
// Feel free to change these values if you find a better range to work with
const int THRESHOLD_WHITE_BLACK = 450;
const int THRESHOLD_OUTSIDE = 850;

// TODO: Make a global counter to represent the number of counted lines
int lineCount = 0;

// helper functions
void forward(int speed) {
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  digitalWrite(RIGHT_MOTOR_DIR, HIGH);
  analogWrite(RIGHT_MOTORS, speed);
  analogWrite(LEFT_MOTORS, speed);
}

void backward(int speed) {
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(RIGHT_MOTORS, speed);
  analogWrite(LEFT_MOTORS, speed);
}

void turnRight(int speed) {
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  digitalWrite(RIGHT_MOTOR_DIR, HIGH);
  analogWrite(LEFT_MOTORS, 35);
  analogWrite(RIGHT_MOTORS, speed+50);
}

void standingRight(int speed) {
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTORS, 100);
  analogWrite(RIGHT_MOTORS, 100);
}

void noWallTurn(int speed) {
  forward(forwardSpeed);
  delay(750);
  standingRight(turnSpeed);
  delay(555);
  forward(forwardSpeed);
  delay(900);
  standingRight(turnSpeed);
  delay(560);
  forward(forwardSpeed);
  delay(265);
}

void backRight(int speed) {
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTORS, 30);
  analogWrite(RIGHT_MOTORS, speed+40);
}

void turnLeft(int speed) {
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  digitalWrite(RIGHT_MOTOR_DIR, HIGH);
  analogWrite(LEFT_MOTORS, speed+55);
  analogWrite(RIGHT_MOTORS, 35);
}

void backLeft(int speed) {
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTORS, speed+55);
  analogWrite(RIGHT_MOTORS, 35);
}

void enterDoor() {
  turnLeft(turnSpeed-10);
  delay(1000);
  servo.write(0);
  forward(forwardSpeed);
  delay(845);
  turnLeft(turnSpeed);
  delay(750);
  motorOff();
  delay(1000);
}

bool lineDetect() {
  bool rightBlack = 0;
  bool leftBlack = 0;
  bool midBlack = 0;
  int leftRead = analogRead(LEFT);
  int middleRead = analogRead(MIDDLE);
  int rightRead = analogRead(RIGHT);

  if (rightRead > THRESHOLD_WHITE_BLACK && rightRead < THRESHOLD_OUTSIDE) {
      rightBlack = 1;
  } else {
    rightBlack = 0;
  }

  if (middleRead > THRESHOLD_WHITE_BLACK && middleRead < THRESHOLD_OUTSIDE) {
    midBlack = 1;
  } else {
    midBlack = 0;
  }

  if (leftRead > THRESHOLD_WHITE_BLACK && leftRead < THRESHOLD_OUTSIDE) {
    leftBlack = 1;
  } else {
    leftBlack = 0;
  }

  return ((rightBlack && leftBlack && midBlack) || (rightBlack && midBlack) || (midBlack && leftBlack));
}

int wallDetect(int wallDist, int distVal) {
  if ((0+1.5 < wallDist) & (wallDist < distVal-1.5)) {
    return 3; // too close, turn left
  } 
  else if (wallDist > distVal+1.5) {
    return 2; // too far, turn right
  } 
  else if (wallDist == 0) {
    return 0; // wall not detected, turn right 
  }
  else {
    return 1; // appropriate distace, go straight
  }
}

int newWallDetect(int wallDist) {
  if ((0+3.5 < wallDist) & (wallDist < distVal2-3.5)) {
    return 3; // too close, turn left
  } 
  else if (wallDist > distVal2+3.5) {
    return 2; // too far, turn right
  } 
  else if (wallDist == 0) {
    return 0; // wall not detected, turn right 
  }
  else {
    return 1; // appropriate distace, go straight
  }
}


// initial setup on start
void initial(){
  motorOff();
  servo.write(0);
  servo2.write(180);
}

// TODO: off function -> sets MOTOR_OUTPUT to LOW
// Inputs: None
// Outputs: None
void motorOff() {
    digitalWrite(MOTOR_OUTPUT, LOW);
}

void checkDoor(int lineCount, bool lineFound, bool doorFound) {
  motorOff();
  servo.write(0);
  delay(1500);
  while (doorFound == 0)
  {
    if (lineCount%2 == 1) {
      delay(250);
      int wallDist = sonar.ping_cm();
      int mode = wallDetect(wallDist, distVal2);
      wallFollow(wallDist, distVal2);
      while (lineFound == 0) {
        lineFound = lineDetect();
      }
      
      if (lineFound) {
        lineFound = 0;
        lineCount++;
        servo.write(180);
        motorOff();
        delay(1500);
        int wallDist = sonar.ping_cm();
        int wall = wallDetect(wallDist, distVal);
        if (wall == 0) {
          doorFound = 1;
          doorNum = 1;
          break;
        } else {
          servo.write(0);
          delay(1500);
          break;
        }
      }
    } else {
      delay(350);
      int wallDist = sonar.ping_cm();
      int mode = wallDetect(wallDist, distVal2);
      backFollow(mode);
      while (lineFound == 0) {
        lineFound = lineDetect();
      }
      
      if (lineFound) {
        lineFound = 0;
        lineCount++;
        servo.write(180);
        motorOff();
        delay(1500);
        int wallDist = sonar.ping_cm();
        int wall = wallDetect(wallDist, distVal);
        if (wall == 0) {
          doorFound = 1;
          doorNum = 2;
          break;
        } else {
          servo.write(0);
          delay(1500);
          break;
        }
      }
    }
  }
}

void stage2() {
  turnSpeed = 75;
  int wallDist = sonar.ping_cm();
  int mode = newWallDetect(wallDist);
  bool line = lineDetect();

  servo.write(0);
  if (doorNum == 1) {
    countNum = 250;
  } else if (doorNum == 2) {
    countNum = 150;
  }

  count++;
  if (count < countNum) {
    wallFollow(mode, 2);

    } else {
      mode = newWallDetect(wallDist); 
      backFollow(mode);
  }  
}

void wallFollow(int mode, int state) {
  switch (mode) {
    
    case noWall:
      if (state == 1) {
        noWallTurn(turnSpeed);
      } else {
        turnRight(turnSpeed);
      }
      break;
    
    case goodDist: // appropriate distance from wall
      forward(forwardSpeed);
      break;
    
    case tooFar: // too far from wall
      turnRight(turnSpeed);
      break;
    
    case tooClose: // too close from wall
      turnLeft(turnSpeed);
      break;
  }
}

void backFollow(int mode) {
  switch (mode) {
    case noWall: // no wall
      backward(backwardSpeed);
      break;
    case goodDist: // appropriate distance from wall
      backward(backwardSpeed);
      break;
    case tooFar: // too far from wall
      backRight(backwardSpeed);
      break;
    case tooClose: // too close from wall
      backLeft(backwardSpeed);
      break;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servo.attach(ServoPin);
  servo2.attach(Servo2Pin);
  
  // motor pins:
  pinMode(MOTOR_OUTPUT, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  pinMode(RIGHT_MOTORS, OUTPUT);
  pinMode(LEFT_MOTORS, OUTPUT);

  //IR pins:
  pinMode(LEFT, INPUT);
  pinMode(MIDDLE, INPUT);
  pinMode(RIGHT, INPUT);

  // initial state
  initial();
}

void loop() {

  int wallDist = sonar.ping_cm();
  int mode = wallDetect(wallDist, distVal);
  bool line = lineDetect();
  
  if (line) {
    lineCount++;
    Serial.println("ooga booga"); // debugging print statement
    delay(200);
  }

  if ((line == 0) && (doorFound == 0)) {
    wallFollow(mode, 1);

  } else {
    if (doorFound == 0){
      checkDoor(lineCount, lineFound, doorFound);
    }

    doorFound = 1;
    enterDoor();
    bool buckFound = 0;
    backward(backwardSpeed);
    delay(1200);

    while (buckFound == 0) 
    {
      stage2();
      line = lineDetect();
      if (line) {
        buckFound = 1;
      }
    }
    endGame = 1;
    while (endGame) 
    {
      motorOff();
      delay(255);
      servo2.write(90);
    }
    }
}
