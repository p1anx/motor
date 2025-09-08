// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//     UNUSED(huart);
//     if(huart->Instance == USART1){
//         my_printf("%s", rxBuffer);
//         // memset(rxBuffer, 0, BUFFER_SIZE);
//         HAL_UART_Receive_IT(&huart1, (uint8_t *)rxBuffer, BUFFER_SIZE);
//     }
//
// } 
