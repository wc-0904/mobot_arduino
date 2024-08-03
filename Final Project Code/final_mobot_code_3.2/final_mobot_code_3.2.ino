int distVal = 10.5;
int distVal2 = 16.5;
int turnSpeed = 90;
int forwardSpeed = turnSpeed - 15;
int backwardSpeed = forwardSpeed - 15;
int desiredLine = 2;
const int noWall = 0;
const int goodDist = 1;
const int tooFar = 2;
const int tooClose = 3;
const int rev = 4;
bool lineFound = 0;
bool doorFound = 0;
bool buckFound = 0;
bool endGame = 0;
int count = 0;

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

NewPing sonar (TriggerPin, EchoPin, 30);
Servo servo;

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
  analogWrite(LEFT_MOTORS, 40);
  analogWrite(RIGHT_MOTORS, speed+50);
}

void backRight(int speed) {
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTORS, 40);
  analogWrite(RIGHT_MOTORS, speed+50);
}

void turnLeft(int speed) {
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  digitalWrite(RIGHT_MOTOR_DIR, HIGH);
  analogWrite(LEFT_MOTORS, speed+50);
  analogWrite(RIGHT_MOTORS, 30);
}

void backLeft(int speed) {
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTORS, speed+50);
  analogWrite(RIGHT_MOTORS, 30);
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

int wallDetect(int wallDist, int dir) {
  if (dir>0) {
    if ((0+2.5 < wallDist) && (wallDist < distVal-2.5)) {
      return 3; // too close, turn left
    } 
    else if (wallDist > distVal+2.5) {
      return 2; // too far, turn right
    } 
    else if (wallDist == 0) {
      return 0; // wall not detected, turn right 
    }
    else {
      return 1; // appropriate distace, go straight
    }
} else {
  if ((0+2.5 < wallDist) && (wallDist < distVal2-2.5)) {
    return 3; // too close, turn left
  } 
  else if (wallDist > distVal2+2.5) {
    return 2; // too far, turn right
  } 
  else if (wallDist == 0) {
    return 0; // wall not detected, turn right 
  }
  else {
    return 1; // appropriate distace, go straight
  }
}
}

// initial setup on start
void initial(){
  motorOff();
  servo.write(0);
  // Serial.println("here");
}

// TODO: off function -> sets MOTOR_OUTPUT to LOW
// Inputs: None
// Outputs: None
void motorOff() {
    digitalWrite(MOTOR_OUTPUT, LOW);
}

void checkDoor(int lineCount, bool lineFound, bool doorFound) {
  motorOff();
  servo.write(180);
  delay(1500);
  while (doorFound == 0)
  {
    if (lineCount%2 == 1) {
      forward(forwardSpeed);
      delay(250);
      // forward(forwardSpeed);
      while (lineFound == 0) {
        lineFound = lineDetect();
      }
      
      if (lineFound) {
        lineFound = 0;
        lineCount++;
        motorOff();
        delay(1500);
        int wallDist = sonar.ping_cm();
        int wall = wallDetect(wallDist, 1);
        if (wall == 0) {
          doorFound = 1;
          break;
        }
      }
    } else {
      backward(backwardSpeed);
      delay(250);
      // backward(backwardSpeed);
      while (lineFound == 0) {
        lineFound = lineDetect();
      }
      
      // bool lineFound = lineDetect();
      if (lineFound) {
        lineFound = 0;
        lineCount++;
        motorOff();
        delay(1500);
        int wallDist = sonar.ping_cm();
        int wall = wallDetect(wallDist, 1);
        if (wall == 0) {
          doorFound = 1;
          break;
        }
      }
    }
  }
}

void stage3() {
  Serial.println("entered stage 3");
  servo.write(0);
  delay(500);
  // bool line = lineDetect();
  // motorOff();
  int wallDist = sonar.ping_cm();
  int mode = wallDetect(wallDist, -1);
  while (!(mode == 0)) 
  {
    wallDist = sonar.ping_cm();
    mode = wallDetect(wallDist, -1); // use distVal2
    wallFollow(mode, doorFound, count, 1); // forwards following
  }
  // int wallDist = sonar.ping_cm();
  // int mode = wallDetect(wallDist, -1);
  motorOff();
  delay(1000);
  bool line = lineDetect();
  while (!(line)) 
  {
    int wallDist = sonar.ping_cm();
    bool mode = wallDetect(wallDist, -1); // use distVal2 
    line = lineDetect();
    wallFollow(mode, doorFound, count, -1); // backwards following
  }
}

void wallFollow(int mode, bool doorFound, int count, int dir) {
  switch (mode) {

    case noWall: // no wall
      if (dir>0) {
        turnRight(turnSpeed);
      } else {
        count = 249;
      }
      break;

    case goodDist: // appropriate distance from wall
      if (dir>0) {
        forward(forwardSpeed);
      } else {
        backward(backwardSpeed);
      }
      break;

    case tooFar: // too far from wall
      if (dir>0) {
        turnRight(turnSpeed);
      } else {
        backRight(turnSpeed);
      }
      break;

    case tooClose: // too close from wall
      if (dir>0) {
        turnLeft(turnSpeed);
      } else {
        backLeft(turnSpeed);
      }
      break;
  }
}

void stage2() {
  turnLeft(turnSpeed-10);
  delay(1000);
  motorOff();
  servo.write(90);
  delay(1200);
  forward(forwardSpeed);
  delay(1000);
  turnLeft(turnSpeed);
  delay(150);
  int wallDist = sonar.ping_cm();
  while (wallDist > 10.15) {
    forward(forwardSpeed);
    wallDist = sonar.ping_cm();
  }
  turnLeft(turnSpeed);
  delay(1000);
  motorOff();
  servo.write(0);
  delay(500);
  // motorOff();
  // servo.write(0);
  // delay(1000);
}

void setup() {
  Serial.begin(9600);
  servo.attach(ServoPin);

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

  initial();
  delay(500);
  // Uncomment the below to begin debugging
}

void loop() {
  // put your main code here, to run repeatedly:
 
  int wallDist = sonar.ping_cm();
  int mode = wallDetect(wallDist, 1);
  bool line = lineDetect();


// *** ENTER STAGE 1 ***

// counts lines, used later to alternate between forward and backward to check door
  if (line) {
    lineCount++;
    // Serial.println("ooga booga");
    delay(200);
  }


  // if it detects no line and the door isn't found, just follow the wall
  if ((line == 0) && (doorFound == 0)) {
    // Serial.println("following wall stage 1");
    wallFollow(mode, doorFound, 0, 1);

  } else {
    // checks and updates door status if line detected and door is still not found
    if (doorFound == 0){
      checkDoor(lineCount, lineFound, doorFound); // this function has a loop, won't exit until door is found
    }
    // global variable adjustment, only runs if code exits checkDoor() 
    doorFound = 1;


    // ENTER STAGE 2
    // contains the turn left and door entering mechanism
    stage2();

    // ENTER STAGE 3
    // initial boolean for bucket found or not
    // loops stage3() until bucket is found
    while (buckFound == 0) 
    {
      stage3(); // should only exit when line is detected
      // line = lineDetect();
      buckFound = 1;
      // Serial.println("line detected");
    }

    }
    if (buckFound) {
      endGame = 1;
    }
    // endGame = 1;
    while (endGame == 1) 
    {
      motorOff();
      Serial.println("endgame here");
    }
    // while (endGame) 
    // {
    //   motorOff();
    // }
    }
