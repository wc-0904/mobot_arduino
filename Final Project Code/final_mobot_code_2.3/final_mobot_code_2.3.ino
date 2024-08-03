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

NewPing sonar (TriggerPin, EchoPin, 30);
Servo servo;
Servo servo2;

// Threshold values for IR detection
// Feel free to change these values if you find a better range to work with
const int THRESHOLD_WHITE_BLACK = 450;
const int THRESHOLD_OUTSIDE = 850;


// The number of lines your car should detect. This value should not be changed.


// TODO: Make a global counter to represent the number of counted lines
int lineCount = 0;
// TODO: forward function -> turns on motors and sets the appropriate speed
// Inputs: An integer representing the speed of the motors (0-255)
// Outputs: None
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

int wallDetect(int wallDist) {
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
  Serial.println("here");
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
      forward(forwardSpeed);
      // int wallDist = sonar.ping_cm;
      // int mode = wallDetect(wallDist, distVal2);
      delay(250);
      // forward(forwardSpeed);
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
      backward(backwardSpeed);
      // int wallDist = sonar.ping_cm();
      // int mode = wallDetect(wallDist, distVal2);
      delay(250);
      while (lineFound == 0) {
        lineFound = lineDetect();
      }
      
      // bool lineFound = lineDetect();
      if (lineFound) {
        lineFound = 0;
        lineCount++;
        servo.write(180);
        motorOff();
        delay(1500);
        int wallDist = sonar.ping_cm();
        int wall = wallDetect(wallDist);
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
  // Serial.println(wallDist);
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
    // switch (mode) {
    //   case noWall: // no wall
    //     // motorOff();
    //     // maybe another bool to discern if we're past a wall?
    //     // delay(50);
    //     Serial.println("here 0");
    //     turnRight(turnSpeed-15);
    //     break;
    //   case goodDist: // appropriate distance from wall
    //     forward(forwardSpeed);
    //     Serial.println("here 1");
    //     break;
    //   case tooFar: // too far from wall
    //     turnRight(turnSpeed-15);
    //     Serial.println("here 2");
    //     break;
    //   case tooClose: // too close from wall
    //     turnLeft(turnSpeed-15);
    //     Serial.println("here 3");
    //     break;
    //   }  
    } else {
      mode = newWallDetect(wallDist); 
      backFollow(mode);
  }  
}

void wallFollow(int mode, int state) {
  switch (mode) {
    case noWall: // no wall
      if (state == 1) {
        noWallTurn(turnSpeed);
      } else {
        turnRight(turnSpeed);
      }
      // Serial.println("here 0");
      // noWallTurn(turnSpeed);
      // delay(800);
      // turnRight(turnSpeed);
      // delay(500);
      break;
    case goodDist: // appropriate distance from wall
      forward(forwardSpeed);
      Serial.println("here 1");
      break;
    case tooFar: // too far from wall
      turnRight(turnSpeed);
      Serial.println("here 2");
      break;
    case tooClose: // too close from wall
      turnLeft(turnSpeed);
      Serial.println("here 3");
      break;
  }
}

void backFollow(int mode) {
  switch (mode) {
    case noWall: // no wall
      // motorOff();
      // maybe another bool to discern if we're past a wall?
      // delay(50);
      Serial.println("here 0");
      backward(backwardSpeed);
      break;
    case goodDist: // appropriate distance from wall
      backward(backwardSpeed);
      Serial.println("here 1");
      break;
    case tooFar: // too far from wall
      // backLeft(backwardSpeed);
      backRight(backwardSpeed);
      // delay(200);
      // backward(200);
      // backRight(backwardSpeed);
      // delay()
      Serial.println("here 2");
      break;
    case tooClose: // too close from wall
      // backRight(backwardSpeed);
      backLeft(backwardSpeed);
      Serial.println("here 3");
      break;
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servo.attach(ServoPin);
  servo2.attach(Servo2Pin);
  // servo.write(180);
  
  // TODO: At least 6 calls to pinMode to set up the appropriate INPUTS and OUTPUTS
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

  Serial.begin(9600);
  // TODO: function call to forward with an appropriate speed value
  initial();
  // Uncomment the below to begin debugging
}

void loop() {
  // put your main code here, to run repeatedly:
 
  // servo.write(180);
  int wallDist = sonar.ping_cm();
  int mode = wallDetect(wallDist, distVal1);
  Serial.println(wallDist);

  bool line = lineDetect();
  if (line) {
    lineCount++;
    Serial.println("ooga booga");
    delay(200);
  }

  // mode switch between left,right,forward
  // try to create an array like [0, "left", "right", "forward"] then index into it
  if ((line == 0) && (doorFound == 0)) {
    wallFollow(mode, 1);

    // switch (mode) {
    //   case noWall: // no wall
    //     // motorOff();
    //     // maybe another bool to discern if we're past a wall?
    //     // delay(50);
    //     // delay(400);
    //     Serial.println("here 0");
    //     turnRight(turnSpeed);
    //     break;
    //   case goodDist: // appropriate distance from wall
    //     forward(forwardSpeed);
    //     Serial.println("here 1");
    //     break;
    //   case tooFar: // too far from wall
    //     turnRight(turnSpeed);
    //     Serial.println("here 2");
    //     break;
    //   case tooClose: // too close from wall
    //     turnLeft(turnSpeed);
    //     Serial.println("here 3");
    //     break;

  } else {
    if (doorFound == 0){

      checkDoor(lineCount, lineFound, doorFound);
    }
    doorFound = 1;
    // servo.write(180);
    // turnLeft(turnSpeed-10);
    // delay(1000);
    // servo.write(0);
    // forward(forwardSpeed);
    // delay(870);
    // turnLeft(turnSpeed);
    // delay(750);
    // motorOff();
    // delay(1000);
    enterDoor();
    bool buckFound = 0;
    backward(backwardSpeed);
    delay(1200);
    while (buckFound == 0) 
    {
      // stuff
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
