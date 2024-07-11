#include <Wire.h>
#include <NewPing.h>
#include <MsTimer2.h> // MsTimer2 라이브러리 포함

#define SONAR_NUM 3      
#define MAX_DISTANCE 100

#define SIZE 5

#define sensorPin A0

#define Front 0
#define Left  1 
#define Right 2

#define TRIG1 31 // 초음파 센서 1번 Trig 핀 번호
#define ECHO1 30 // 초음파 센서 1번 Echo 핀 번호

#define TRIG2 27 // 초음파 센서 2번 Trig 핀 번호
#define ECHO2 26 // 초음파 센서 2번 Echo 핀 번호

#define TRIG3 34 // 초음파 센서 3번 Trig 핀 번호
#define ECHO3 35 // 초음파 센서 3번 Echo 핀 번호

NewPing sonar[SONAR_NUM] = {   
  NewPing(TRIG1, ECHO1, MAX_DISTANCE), 
  NewPing(TRIG2, ECHO2, MAX_DISTANCE),
  NewPing(TRIG3, ECHO3, MAX_DISTANCE)
};

float front_sensorData[SIZE] = {0.0};
float left_sensorData[SIZE] = {0.0};
float right_sensorData[SIZE] = {0.0};
float resistance_sensorData[SIZE] = {0.0};

float recursive_moving_average_front(float ad_value) {
  static float avg_front = 0.0;  

  for (int i = 0; i <= SIZE - 2; i++) {
    front_sensorData[i] = front_sensorData[i + 1]; 
  }
    
  front_sensorData[SIZE - 1] = ad_value;
     
  avg_front = avg_front + (front_sensorData[SIZE - 1] - front_sensorData[0]) / (float)SIZE;

  return avg_front;
}

float recursive_moving_average_left(float ad_value) {
  static float avg_left = 0.0;  

  for (int i = 0; i <= SIZE - 2; i++) {
    left_sensorData[i] = left_sensorData[i + 1]; 
  }
    
  left_sensorData[SIZE - 1] = ad_value;
     
  avg_left = avg_left + (left_sensorData[SIZE - 1] - left_sensorData[0]) / (float)SIZE;

  return avg_left;
}

float recursive_moving_average_right(float ad_value) {
  static float avg_right = 0.0;  

  for (int i = 0; i <= SIZE - 2; i++) {
    right_sensorData[i] = right_sensorData[i + 1]; 
  }
    
  right_sensorData[SIZE - 1] = ad_value;
     
  avg_right = avg_right + (right_sensorData[SIZE - 1] - right_sensorData[0]) / (float)SIZE;

  return avg_right;
}

float recursive_moving_average_resistance(float add_value) {
  static float avg_resistance = 0.0;  

  for (int i = 0; i <= SIZE - 2; i++) {
    resistance_sensorData[i] = resistance_sensorData[i + 1]; 
  }
    
  resistance_sensorData[SIZE - 1] = add_value;
     
  avg_resistance = avg_resistance + (resistance_sensorData[SIZE - 1] - resistance_sensorData[0]) / (float)SIZE;

  return avg_resistance;
}

float result = 0.0; // 전역 변수로 선언

void Read_sensor(void) {
   float add_value = 0.0;
   add_value = analogRead(A0);
   result = recursive_moving_average_resistance(add_value);
   Serial.print("가변저항 : "); Serial.println(result);
   Serial.println(" ");
}

void setup() {
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);
  
  Serial.begin(115200);

  MsTimer2::set(100, Read_sensor); // 100ms마다 Read_sensor 함수 호출
  MsTimer2::start(); // MsTimer2 시작

  Wire.begin();
}

void loop() {
  float front_sonar = sonar[Front].ping_cm();
  float left_sonar  = sonar[Left].ping_cm();  
  float right_sonar = sonar[Right].ping_cm();
  
  if (front_sonar == 0.0 ) front_sonar = MAX_DISTANCE; 
  if (left_sonar == 0.0) left_sonar = MAX_DISTANCE;
  if (right_sonar == 0.0) right_sonar = MAX_DISTANCE;
  
  float front_avg = recursive_moving_average_front(front_sonar);
  float left_avg = recursive_moving_average_left(left_sonar);
  float right_avg = recursive_moving_average_right(right_sonar);

  Serial.print("정면: "); Serial.print(front_avg); Serial.print("cm ");
  Serial.print("왼쪽: "); Serial.print(left_avg); Serial.print("cm ");
  Serial.print("오른쪽: "); Serial.print(right_avg); Serial.println("cm ");

  union {
    float value;
    byte bytes[4];
  } frontUnion, leftUnion, rightUnion, resultUnion;

  frontUnion.value = front_avg;
  leftUnion.value = left_avg;
  rightUnion.value = right_avg;
  resultUnion.value = result;

  Wire.beginTransmission(8); // 슬레이브 주소 8번
  Wire.write(frontUnion.bytes, 4); // 정면 초음파 센서 값
  Wire.write(leftUnion.bytes, 4); // 왼쪽 초음파 센서 값
  Wire.write(rightUnion.bytes, 4); // 오른쪽 초음파 센서 값
  Wire.write(resultUnion.bytes, 4); // 가변 저항 값
  Wire.endTransmission(); // 데이터 송신 종료

  delay(100);
}
