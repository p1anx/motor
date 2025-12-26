//
// Created by xwj on 12/4/25.
//

#include "vofa.h"
#include <string.h>

extern char txDMA_buffer[1024];
volatile int Vofa_TxCpltFlag = 0;
uint8_t UART_DMABuffer[1024];

void UART_SendJustFloat(float *pData, const int n) {
  uint8_t justFloat_tail[4] = {0x00, 0x00, 0x80, 0x7f};
  uint8_t txBuffer[1024];
  int data_bytes = sizeof(float) * n;
  memcpy(txBuffer, pData, data_bytes);
  HAL_UART_Transmit(&vofa_huart, txBuffer, data_bytes, 1000);
  HAL_UART_Transmit(&vofa_huart, justFloat_tail, 4, 1000);

}


void UART_SendJustFloat_DMA(float *pData, const int n) {
  uint8_t justFloat_tail[4] = {0x00, 0x00, 0x80, 0x7f};
  const int data_bytes = sizeof(float) * n;
  // memcpy(UART_DMABuffer, pData, data_bytes);
  // memcpy(UART_DMABuffer + data_bytes, justFloat_tail, 4);
  memcpy(txDMA_buffer, pData, data_bytes);
  memcpy(txDMA_buffer + data_bytes, justFloat_tail, 4);
  if (Vofa_TxCpltFlag == 1) {
    Vofa_TxCpltFlag = 0;
    HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, data_bytes + 4);
  }

}
void VOFA_SendJustFloat(float *pData, const int n) {
  return UART_SendJustFloat(pData, n);
}
void VOFA_SendJustFloat_DMA(float *pData, const int n) {
  return UART_SendJustFloat_DMA(pData, n);
}
void VOFA_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart == &vofa_huart) {
    Vofa_TxCpltFlag = 1;
  }
}


void VOFA_SendjustFloat4_DMA(float dataFloat1, float dataFloat2, float dataFloat3, float dataFloat4) {

  uint8_t justFloat_tail[4] = {0, 0, 0x80, 0x7f};
  uint8_t buffer[256];
  const int dataFloat_num = 4;
  float dataFloat_list[4] = {dataFloat1, dataFloat2, dataFloat3, dataFloat4};
  int data_n = sizeof(dataFloat1) * dataFloat_num + sizeof(justFloat_tail);
  for (int i = 0; i < dataFloat_num; i++) {
    memcpy(buffer + i*sizeof(dataFloat1), &dataFloat_list[i], sizeof(dataFloat1));
  }
  memcpy(buffer + dataFloat_num*sizeof(dataFloat1) , &justFloat_tail, sizeof(justFloat_tail));
  memcpy(txDMA_buffer, buffer, sizeof(buffer));
  if (Vofa_TxCpltFlag== 1) {
    Vofa_TxCpltFlag= 0;
    HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, data_n);
  }

}





void vofa_test_uart(void) {
  HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
  // strcpy(txDMA_buffer, "hello world, uart1\n");
  float data_float[2];
  data_float[0] = 10.0f;
  data_float[1] = 10.0f;
  while (1) {
    // if (TxCompleteFlag == 1) {
    data_float[0] += 1.0f;
    if (data_float[0] > 10000.0f) {
      data_float[0] = 0.0f;
    }
    UART_SendJustFloat(data_float, 1);

    // printf("%f\n", data_float[0]);
    // HAL_UART_Transmit(&print_uart, tx_uint8, NUM, 10000);// 86.85us for 115200, ideal t = 86.81us per byte
    // HAL_UART_Transmit(&huart3, tx_uint8, NUM, 10000);// 86.85us for 115200, ideal t = 86.81us per byte
    // UART_WriteBytes(&print_uart, tx_uint8, NUM); // 86.83us

  }
}

void vofa_test_uart_dma(void) {
  HAL_UART_Transmit_DMA(&vofa_huart, (uint8_t*)txDMA_buffer, sizeof(txDMA_buffer));
  // strcpy(txDMA_buffer, "hello world, uart1\n");
  float data_float[2];
  data_float[0] = 10.0f;
  data_float[1] = 10.0f;
  while (1) {
    // if (TxCompleteFlag == 1) {
    data_float[0] += 1.0f;
    if (data_float[0] > 10000.0f) {
      data_float[0] = 0.0f;
    }
    if (getUs() % 100 ==  0) {
      VOFA_SendJustFloat_DMA(data_float, 2);
    }

    // printf("%f\n", data_float[0]);
    // HAL_UART_Transmit(&print_uart, tx_uint8, NUM, 10000);// 86.85us for 115200, ideal t = 86.81us per byte
    // HAL_UART_Transmit(&huart3, tx_uint8, NUM, 10000);// 86.85us for 115200, ideal t = 86.81us per byte
    // UART_WriteBytes(&print_uart, tx_uint8, NUM); // 86.83us

  }
}
