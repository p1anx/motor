//
// Created by xwj on 11/22/25.
//

#include "uart.h"

int TxCompleteFlag = 0;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    // 设置标志位，通知主循环发送已完成
    TxCompleteFlag = 1;
  }
}
// for `vofa justFloat` plot data, example of usage

// extern int TxCompleteFlag;
// char buffer[3];
// extern char txDMA_buffer[256];
// extern uint8_t tempData[8];
// float data_float[1];
// uint8_t tail[4] = {0, 0, 0x80, 0x7f};
// uint8_t head[4] = {0, 0};
// DMA_BUFFER uint8_t Data[8] = {0, 0, 0, 0, 0, 0,0x80, 0x7F};
// DMA_BUFFER float DataFloat[1];
// void test_uart(void) {
//     printf("start vofa test\n");
//     // HAL_UART_Transmit_DMA(&huart1, tempData, 8);
//     HAL_UART_Transmit_DMA(&huart1, Data, 8);
//     // strcpy(txDMA_buffer, "hello world\n");
//     // txDMA_buffer[0] = 0x11;
//     // txDMA_buffer[1] = 0x12;
//     // txDMA_buffer[2] = 0x13;
//     // txDMA_buffer[3] = 0x14;
//     // txDMA_buffer[4] = 0x0;
//     // txDMA_buffer[5] = 0x0;
//     // txDMA_buffer[6] = 0x80;
//     // txDMA_buffer[7] = 0x7f;
//     static uint8_t vofa_justFloat[8] = {0, 0, 0, 0, 0, 0, 0x80, 0x7F};
//     memcpy(txDMA_buffer, (char*)vofa_justFloat, 8);
//     // txDMA_buffer[5] = '\n';
//
//     HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
//     data_float[0] = 10.0f;
//     while (1) {
//         data_float[0] += 1.0f;
//         if (data_float[0] > 100.0f) {
//             data_float[0] = 10.0f;
//         }
//         DataFloat[0] = 2.0f;
//         // 1.
//          // memcpy(head, (uint8_t*)&data_float, 4);
//          // HAL_UART_Transmit(&print_uart, head, 4, 1000);
//          // HAL_UART_Transmit(&print_uart, tail, 4, 1000);
//          // HAL_Delay(1);
//
//         //2.
//          memcpy(txDMA_buffer, (uint8_t*)&data_float, sizeof(data_float));
//         if (TxCompleteFlag == 1) {
//             TxCompleteFlag = 0;
//             HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, 8);
//         }
//         HAL_Delay(1);
//         //3.not ok
//         // memcpy(vofa_justFloat, (uint8_t*)&data_float, sizeof(data_float));
//         // HAL_UART_Transmit(&print_uart,vofa_justFloat, 8, 1000);
//
//         // // memcpy(txDMA_buffer, (uint8_t*)&DataFloat, sizeof(DataFloat));
//         // memcpy(Data, (uint8_t*)&DataFloat, sizeof(DataFloat));
//         //  // memcpy(Data, (uint8_t*)&data_float, 4);
//         // if (TxCompleteFlag == 1) {
//         //     TxCompleteFlag = 0;
//         //     // HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, strlen(txDMA_buffer));
//         //     // HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txDMA_buffer, 8);
//         //     HAL_UART_Transmit_DMA(&print_uart, Data, 8);
//         // }
//         HAL_Delay(1);
//
//
//     }
//
// }
