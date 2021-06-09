#include <Arduino.h>
#include "encryption.h"
void setup() {
  Serial.begin(921600);
  while(!Serial){}
  Security sec;
  sec.encryption_key();
  char str[] = {0xAA,1,2,3,4,5,6,7,8,9,255};
  
  for(uint32_t i = 0; i < 11; i++) {
    Serial.print(str[i],DEC);Serial.print(" ");
  }
  Serial.println();

  sec.encrypt(str,11);

  for(uint32_t i = 0; i < 11; i++) {
    Serial.print(str[i],DEC);Serial.print(" ");
  }
  Serial.println();

  sec.decrypt(str,11);

  for(uint32_t i = 0; i < 11; i++) {
    Serial.print(str[i],DEC);Serial.print(" ");
  }
  Serial.println();



  for(char k = 170; k < 0xFF; k++){
    char c = k;
    sec.encrypt(&c,1);
    sec.decrypt(&c,1);
    if(c != k){
      Serial.print("Error at k =\t");Serial.print(k,DEC);Serial.print("\twith error\t");Serial.println(k-c,DEC);
    }
  }
  Serial.println("---------------------------------------------");
  for(char k = 170; k > 0; k--){
    char c = k;
    sec.encrypt(&c,1);
    sec.decrypt(&c,1);
    if(c != k){
      Serial.print("Error at k =\t");Serial.print(k,DEC);Serial.print("\twith error\t");Serial.println(k-c,DEC);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}