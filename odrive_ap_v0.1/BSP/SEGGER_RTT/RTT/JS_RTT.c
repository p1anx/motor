//
// Created by xwj on 12/30/25.
//

#include "JS_RTT.h"

#include "SEGGER_RTT_Conf.h"
#include "SEGGER_RTT.h"

#include <stdio.h>
char JS_RTT_UpBuffer[4096];    // J-Scope RTT Buffer
int  JS_RTT_Channel = 1;       // J-Scope RTT Channel


static int _timestamp = 0;

#define JS_TIMESTAMP() _timestamp++

void JS_RTT_InitFloat(const int float_num) {
  const int SEGGER_RTT_MODE  = SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL;
  switch (float_num) {
  case 1:
    SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_T4F4", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer),SEGGER_RTT_MODE);
    break;
  case 2:
    SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_T4F4F4", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer),SEGGER_RTT_MODE);
    break;
  case 3:
    SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_T4F4F4F4", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer),SEGGER_RTT_MODE);
    break;
  case 4:
    SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_T4F4F4F4F4", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer),SEGGER_RTT_MODE);
    break;
  case 5:
    SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_T4F4F4F4F4F4", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer),SEGGER_RTT_MODE);
    break;
  case 6:
    SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_T4F4F4F4F4F4F4", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer),SEGGER_RTT_MODE);
    break;
  default:
    // SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_T4F4F4F4", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer),SEGGER_RTT_MODE);
    printf("JS_RTT Num from 1 to 6\n");
    break;
  }

}
void JS_RTT_Write1Float(float data_float) {
  #pragma pack(push, 1)
    struct {
      unsigned int Timestamp;
      float data0;
    } acValBuffer;
  #pragma pack(pop)
  JS_TIMESTAMP();
  acValBuffer.Timestamp = _timestamp;
  acValBuffer.data0 = data_float;
  SEGGER_RTT_Write(JS_RTT_Channel, &acValBuffer, sizeof(acValBuffer));
}
void JS_RTT_Write2Float(float data_float0, float data_float1) {
#pragma pack(push, 1)
  struct {
    unsigned int Timestamp;
    float data0;
    float data1;
  } acValBuffer;
#pragma pack(pop)
  JS_TIMESTAMP();
  acValBuffer.Timestamp = _timestamp;
  acValBuffer.data0 = data_float0;
  acValBuffer.data1 = data_float1;
  SEGGER_RTT_Write(JS_RTT_Channel, &acValBuffer, sizeof(acValBuffer));
}
void JS_RTT_Write3Float(float data_float0, float data_float1, float data_float2) {
#pragma pack(push, 1)
  struct {
    unsigned int Timestamp;
    float data0;
    float data1;
    float data2;
  } acValBuffer;
#pragma pack(pop)
  JS_TIMESTAMP();
  acValBuffer.Timestamp = _timestamp;
  acValBuffer.data0 = data_float0;
  acValBuffer.data1 = data_float1;
  acValBuffer.data2 = data_float2;
  SEGGER_RTT_Write(JS_RTT_Channel, &acValBuffer, sizeof(acValBuffer));
}
void JS_RTT_Write4Float(float data_float0, float data_float1, float data_float2, float data_float3) {
  #pragma pack(push, 1)
    struct {
      unsigned int Timestamp;
      float data0;
      float data1;
      float data2;
      float data3;
    } acValBuffer;
  #pragma pack(pop)
  JS_TIMESTAMP();
  acValBuffer.Timestamp = _timestamp;
  acValBuffer.data0 = data_float0;
  acValBuffer.data1 = data_float1;
  acValBuffer.data2 = data_float2;
  acValBuffer.data3 = data_float3;
  SEGGER_RTT_Write(JS_RTT_Channel, &acValBuffer, sizeof(acValBuffer));
}
void JS_RTT_Write5Float(float data_float0, float data_float1, float data_float2, float data_float3, float data_float4) {
#pragma pack(push, 1)
  struct {
    unsigned int Timestamp;
    float data0;
    float data1;
    float data2;
    float data3;
    float data4;
  } acValBuffer;
#pragma pack(pop)
  JS_TIMESTAMP();
  acValBuffer.Timestamp = _timestamp;
  acValBuffer.data0 = data_float0;
  acValBuffer.data1 = data_float1;
  acValBuffer.data2 = data_float2;
  acValBuffer.data3 = data_float3;
  acValBuffer.data4 = data_float4;
  SEGGER_RTT_Write(JS_RTT_Channel, &acValBuffer, sizeof(acValBuffer));
}
void JS_RTT_Write6Float(float data_float0, float data_float1, float data_float2, float data_float3, float data_float4, float data_float5) {
#pragma pack(push, 1)
  struct {
    unsigned int Timestamp;
    float data0;
    float data1;
    float data2;
    float data3;
    float data4;
    float data5;
  } acValBuffer;
#pragma pack(pop)
  JS_TIMESTAMP();
  acValBuffer.Timestamp = _timestamp;
  acValBuffer.data0 = data_float0;
  acValBuffer.data1 = data_float1;
  acValBuffer.data2 = data_float2;
  acValBuffer.data3 = data_float3;
  acValBuffer.data4 = data_float4;
  acValBuffer.data5 = data_float5;
  SEGGER_RTT_Write(JS_RTT_Channel, &acValBuffer, sizeof(acValBuffer));
}
