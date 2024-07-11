#include <Wire.h> // I2C 통신을 위한 Wire 라이브러리 포함

union FloatUnion 
{
  float value;
  byte bytes[4];
};

void setup() 
{
  Serial.begin(115200); // 시리얼 통신 시작
  Wire.begin(8); // I2C 슬레이브 주소 8번
  Wire.onReceive(receiveEvent); // 데이터 수신 시 이벤트 핸들러 등록
}

void loop() // 메인 loop에서는 특별한 작업을 하지 않습니다.
{
  delay(100);
}

void receiveEvent(int howMany) 
{
  if (Wire.available() >= 16) // 모든 데이터(16개의 바이트)가 수신될 때까지 대기
  { 
    FloatUnion frontUnion, leftUnion, rightUnion, resultUnion;
    
    for (int i = 0; i < 4; i++) 
    {
      frontUnion.bytes[i] = Wire.read();
    }
    for (int i = 0; i < 4; i++) 
    {
      leftUnion.bytes[i] = Wire.read();
    }
    for (int i = 0; i < 4; i++) 
    {
      rightUnion.bytes[i] = Wire.read();
    }
    for (int i = 0; i < 4; i++) 
    {
      resultUnion.bytes[i] = Wire.read();
    }
    
    Serial.print("정면: "); Serial.print(frontUnion.value); Serial.print(" cm, ");
    Serial.print("왼쪽: "); Serial.print(leftUnion.value); Serial.print(" cm, ");
    Serial.print("오른쪽: "); Serial.print(rightUnion.value); Serial.print(" cm, ");
    Serial.print("가변저항 값: "); Serial.print(resultUnion.value); Serial.println(" ");
  }
}
