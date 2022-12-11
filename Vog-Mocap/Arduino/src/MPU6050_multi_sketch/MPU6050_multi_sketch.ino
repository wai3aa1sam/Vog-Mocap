#include "I2Cdev.h"
#include <Wire.h>
#include "MPU6050_6Axis_MotionApps20.h"
#define VECTOR_STRUCT_H
struct myVector
{
    float x = 0;
    float y = 0;
    float z = 0;
};

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)


// ================================================================
// ===               SETTING PARAMETERS                         ===
// ================================================================
MPU6050 IMU(0x69); //Setting AD0 as 1 0x69

int g_active_imu_count = 0;

const int IMU_count = 7;
int pins[IMU_count] = {22,24,26,28,30,32,34};
bool int_IMU;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus[IMU_count] = {0};      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
 


// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

// ================================================================
// ===                       Calculation                        ===
// ================================================================
myVector accel_raw, ax_1, gyro_1;
float quternion_1[4];
float rotMatrix_1[3][3];
myVector abs_Acc_raw_1, abs_Acc_1, abs_gravity;
myVector spd_raw_1, spd_1, spd_change_1;
myVector dist_raw_1, dist_1;

Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector

unsigned long deltaMicros = 50000;
unsigned long timeLastSend = 0;
unsigned long timeLastCal = 0;
float fps = 100.0f;
float dt = deltaMicros / 1000000.0f;
bool readyToSend;
 
float lowPassFilter(float data, float newData, float filter_koef, float filter_trigger);
float quternionToRotationMatrix(float quaternion[4], float rotMatrix_2[3][3]);
void showmyVector(myVector data);
void serialData();  
void pins_init();

void setup() {
  // serial to display data
  Wire.begin();
  Wire.setClock(400000);
  pins_init();
  pinMode(INTERRUPT_PIN, INPUT);
  Serial.begin(115200);

  // start communication with IMU
  Serial.print("Begin Init");
  for(int i = 0; i < IMU_count; i++){
      //Serial.print("IMU ");
      //Serial.print(pins[i]);
      digitalWrite(pins[i], HIGH);
      IMU.initialize();
      if(IMU.testConnection()){
        //Serial.print("Connected");
      }
      devStatus[i] = IMU.dmpInitialize();
      Serial.print(devStatus[i]);
      if (devStatus[i] == 0) {
            // Calibration Time: generate offsets and calibrate our MPU6050
            initOffset(IMU);
            //IMU.PrintActiveOffsets();
            // turn on the DMP, now that it's ready
            
            Serial.println(F("Enabling DMP..."));
            IMU.setDMPEnabled(true);
    
            // enable Arduino interrupt detection
            //Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
            //Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
            //Serial.println(F(")..."));
            attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
            mpuIntStatus = IMU.getIntStatus();
    
            // set our DMP Ready flag so the main loop() function knows it's okay to use it
            //Serial.println(F("DMP ready! Waiting for first interrupt..."));
            dmpReady = true;
            int_IMU = true;
            // get expected DMP packet size for later comparison
            packetSize = IMU.dmpGetFIFOPacketSize();
            g_active_imu_count++;
            
        } else {
            // ERROR!
            // 1 = initial memory load failed
            // 2 = DMP configuration updates failed
            // (if it's going to break, usually the code will be 1)
            Serial.print("IMU Error Pin ");
            Serial.print(pins[i]);
            Serial.print(F("DMP Initialization failed (code "));
            Serial.print(devStatus[i]);
            Serial.println(F(")"));
        }
    digitalWrite(pins[i], LOW);
    delay(1000);
  }
     
  Serial.print("Active IMU count:");
  Serial.print(g_active_imu_count);
  Serial.println("End Init");
  readyToSend = true;
}
int pin_num = 0;
void loop() {
  // start timer to madgwick filter
  
  unsigned long start = micros() - timeLastCal;
  //get data from IMU
  digitalWrite(pins[pin_num], HIGH);
    if(start >= deltaMicros){  
      start = 0;
      if (!dmpReady) return;
      if (IMU.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        IMU.dmpGetQuaternion(&q, fifoBuffer);
        IMU.dmpGetAccel(&aa, fifoBuffer);
        IMU.dmpGetGravity(&gravity, &q);
        IMU.dmpGetLinearAccel(&aaReal, &aa, &gravity);
        IMU.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
      
        quternion_1[0] = q.w;
        quternion_1[1] = q.x;
        quternion_1[2] = q.y;
        quternion_1[3] = q.z;
        accel_raw.x = (float)aaWorld.x / 8192* 9.81;
        accel_raw.y = (float)aaWorld.y / 8192* 9.81;
        accel_raw.z = (float)aaWorld.z / 8192* 9.81;
      timeLastCal = micros();
    }
  
    // time cut-off for sending data with frequency 40 HZ
    unsigned long timeToSend = micros() - timeLastSend;
    if((timeToSend >= 2000) && (readyToSend)){
      //Serial.print(timeToSend);
      timeToSend = 0;
      // sending data
      //showData();
      Serial.print("==");
      Serial.print(pins[pin_num]);
      Serial.print("=:");
      //Serial.print(",");
      serialData();  
      timeLastSend = micros();
    }
    digitalWrite(pins[pin_num], LOW);
    pin_num = (pin_num +1)%g_active_imu_count;
  }
}
void pins_init(){
  for(int i = 0; i < IMU_count; i++){
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
}
    
float lowPassFilter(float data, float newData, float filter_koef, float filter_trigger)
{
  if ((newData - data < filter_trigger) || (data - newData < filter_trigger)){
    data = data + filter_koef * (newData - data);
  }
  else{
    data = newData;
  }
  return data;
}

float complementaryFilter(float data, float change, float filter_koef, float filter_trigger)
{
  if (abs(change) < filter_trigger){
    data = filter_koef * data;
  }
  else{
    data = data + change;
  }
  return data;
}

float cutoffFilter(float data, float cutoff_trigger)
{
  if (abs(data) < cutoff_trigger){
    //Serial.println("Cutoff");
    return 0;
  }
  else{
    return data;
  }
}

float quternionToRotationMatrix(float quaternion[4], float rotMatrix_2[3][3])
{
  // getting rotation matrix from quaternion
  rotMatrix_2[0][0] = 1 - 2 * (quaternion[2] * quaternion[2]) - 2 * (quaternion[3] * quaternion[3]);
  rotMatrix_2[0][1] = 2 * quaternion[1] * quaternion[2] - 2 * quaternion[3] * quaternion[0];
  rotMatrix_2[0][2] = 2 * quaternion[1] * quaternion[3] + 2 * quaternion[2] * quaternion[0];
  rotMatrix_2[1][0] = 2 * quaternion[1] * quaternion[2] + 2 * quaternion[3] * quaternion[0];
  rotMatrix_2[1][1] = 1 - 2 * (quaternion[1] * quaternion[1]) - 2 * (quaternion[3] * quaternion[3]);
  rotMatrix_2[1][2] = 2 * quaternion[2] * quaternion[3] - 2 * quaternion[1] * quaternion[0];
  rotMatrix_2[2][0] = 2 * quaternion[1] * quaternion[3] - 2 * quaternion[2] * quaternion[0];
  rotMatrix_2[2][1] = 2 * quaternion[2] * quaternion[3] + 2 * quaternion[1] * quaternion[0];
  rotMatrix_2[2][2] = 1 - 2 * (quaternion[1] * quaternion[1]) - 2 * (quaternion[2] * quaternion[2]);

  return 0;
}

void serialData() {
   Serial.print(quternion_1[0]);
   Serial.print(",");
   Serial.print(quternion_1[1]);
   Serial.print(",");
   Serial.print(quternion_1[2]);
   Serial.print(",");
   Serial.print(quternion_1[3]);
   Serial.print(",");
   showmyVector(accel_raw);
   Serial.flush();
   Serial.println();
}   

void showmyVector(myVector data){
   Serial.print(data.x, 3);
   Serial.print(", ");
   Serial.print(data.y, 3);
   Serial.print(", ");
   Serial.println(data.z, 3);   
}
void initOffset(MPU6050 IMU) { 
    IMU.setXAccelOffset(0);
    IMU.setYAccelOffset(0);
    IMU.setZAccelOffset(0);
    IMU.setXGyroOffset(0);
    IMU.setYGyroOffset(0);
    IMU.setZGyroOffset(0);
    IMU.CalibrateGyro(6);
    IMU.CalibrateAccel(6);
} // SetOffsets
