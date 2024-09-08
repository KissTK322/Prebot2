#define PWM1 9
#define INA1 2
#define INB1 3
#define PWM2 10
#define INA2 4
#define INB2 5

int sensorValue[2] = {0, 0};
int maxRead[2] = {500,667};
int minRead[2] = {58,99};

void setup() {
  initial();
  MotorControl(70 , 70);
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

void MotorControl(int speedl , int speedr) {
  if (speedl < 0) {
    analogWrite(PWM1 , abs(speedl));
    digitalWrite(INA1, HIGH);
    digitalWrite(INB1, LOW);
  }
  else if (speedl > 0) {
    analogWrite(PWM1 , abs(speedl));
    digitalWrite(INA1, LOW);
    digitalWrite(INB1, HIGH);
  }
  else {
    analogWrite(PWM1 , 0);
    digitalWrite(INA1, LOW);
    digitalWrite(INB1, LOW);
  }
  if (speedr < 0) {
    analogWrite(PWM2 , abs(speedr));
    digitalWrite(INA2, HIGH);
    digitalWrite(INB2, LOW);
  }
  else if (speedr > 0) {
    analogWrite(PWM2 , abs(speedr));
    digitalWrite(INA2, LOW);
    digitalWrite(INB2, HIGH);
  }
  else {
    analogWrite(PWM2 , 0);
    digitalWrite(INA2, LOW);
    digitalWrite(INB2, LOW);
  }
}

void STOP() {
  //Motor1
  analogWrite(PWM1, 0);
  digitalWrite(INA1, HIGH);
  digitalWrite(INB1, HIGH);
  //Motor2
  analogWrite(PWM2, 0);
  digitalWrite(INA2, HIGH);
  digitalWrite(INB2, HIGH);
}

void initial() {
  //Motor1
  pinMode(PWM1, OUTPUT);
  pinMode(INA1, OUTPUT);
  pinMode(INB1, OUTPUT);
  analogWrite(PWM1, 0);
  digitalWrite(INA1, LOW);
  digitalWrite(INB1, LOW);
  //Motor2
  pinMode(PWM2, OUTPUT);
  pinMode(INA2, OUTPUT);
  pinMode(INB2, OUTPUT);
  analogWrite(PWM2, 0);
  digitalWrite(INA2, LOW);
  digitalWrite(INB2, LOW);
}

void Read(){
  for(int pins = 0 ; pins < 2 ; pins++){
    sensorValue[pins] = analogRead(pins);
  }
}

void readCarlib() {
  Read();
  for (int j = 0; j < 2; j++) {
    sensorValue[j] = map(sensorValue[j], minRead[j], maxRead[j], 0, 1000);
    sensorValue[j] = (sensorValue[j] < 0 ? 0 : sensorValue[j]);
    sensorValue[j] = (sensorValue[j] > 1000 ? 1000 : sensorValue[j]);
  }
}

int ReadLine() {
  readCarlib();
  char on_line = 0;
  unsigned long avg;
  unsigned int sum;
  avg = 0;
  sum = 0;
  for (int k = 0; k < 2; k++) {
    int Value;
    Value = sensorValue[k];
    Value = 1000 - Value;
//    if (Value > 150) on_line = 1;
//    if (Value > 50) {
//      avg += (long)(Value) * (k * 1000);
//      sum += Value;
//    }
    if (Value > 220) on_line = 1;
    if (Value > 50) {
      avg += (long)(Value) * (k * 1000);
      sum += Value;
    }
  }
  if (on_line == 0) {
    if (last_value < 500) last_value = 0;
    else last_value = 1000;
  } else {
    last_value = avg / sum;
  }
  return last_value;
}

void stdpid(int BaseSpeed, float Kp, float Kd) {
  int Position  = ReadLine();
  int error = Position - 500;
  int PowerMotor = (Kp * error) + (Kd * (error - lastError));
  lastError = error;
  if (PowerMotor > BaseSpeed) {
    PowerMotor = BaseSpeed;
  }
  if (PowerMotor < 0 - BaseSpeed) {
    PowerMotor = 0 - BaseSpeed;
  }
  int LeftMotor = BaseSpeed + PowerMotor;
  int RightMotor = BaseSpeed - PowerMotor;
  if (LeftMotor >= 255) LeftMotor = 255;
  if (LeftMotor <= 0) LeftMotor = -190;
  if (RightMotor >= 255) RightMotor = 255;
  if (RightMotor <= 0) RightMotor = -190;
  MotorControl(LeftMotor, RightMotor);
}
