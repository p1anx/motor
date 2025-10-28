#include "bit_stream.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* uart_bitstream(const char* input) {
    if (input == NULL) return NULL;
    
    size_t len = strlen(input);
    char* result = malloc(len * 10 + 1); // 每个字符10位 + 终止符
    if (result == NULL) return NULL;
    
    size_t output_index = 0;
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = input[i];
        
        // 起始位（0）
        result[output_index++] = '0';
        
        // 数据位（高位到低位，即bit7到bit0）
        for (int j = 7; j >= 0; --j) {
            int bit = (c >> j) & 1;
            result[output_index++] = bit ? '1' : '0';
        }
        
        // 停止位（1）
        result[output_index++] = '1';
    }
    
    result[output_index] = '\0'; // 终止字符串
    return result;
}

/**
 * 将UTF-8编码的文本转换为比特流
 * 参数:
 *   text: 输入文本字符串
 * 返回:
 *   动态分配的比特流字符串(调用者需要负责释放内存)
 */
char* utf8_to_bits(const char* text) {
    // 计算需要的总空间: 每个字节8位 + 结束符
    size_t len = strlen(text);
    char* bits = malloc(len * 8 + 1);
    if (bits == NULL) {
        return NULL;
    }
    
    char* pos = bits;
    for (size_t i = 0; i < len; i++) {
        unsigned char byte = text[i];
        // 处理每个bit，从最高位开始
        for (int j = 7; j >= 0; j--) {
            *pos++ = (byte & (1 << j)) ? '1' : '0';
        }
    }
    *pos = '\0'; // 添加字符串结束符
    
    return bits;
}
extern float g_target_position;
int char_to_bit_stream(void){
    static int tim_count = 0;
    static int tim_flag = 0;
    const float BIT_1 = POSITION_INCREMENT;
    const float BIT_0 = 0;
    // printf("tim is starting\n");
    if(tim_count > 0.5/PID_UPDATE_TIME){
            tim_count = 0;
            tim_flag = 1;
            // printf("tim is starting in time\n");
    }
    tim_count++;
    printf("tim count = %d\n", tim_count);

    const char* tx_text = "He"; // he ---> 01001000 01100101
    static int bits_cnt = 0;
    char* tx_bit_stream = utf8_to_bits(tx_text);
    // printf("timflag = %d\n", tim_flag);
    // printf("it will go into tim flag\n");
    if(tim_flag == 1){
        // printf("go into tim flag\n");
        printf("utf8 bit stream: '%s'--->%s\n", tx_text, tx_bit_stream);
        printf("str length = %d\n", strlen(tx_bit_stream));
        tim_flag = 0;
        if(bits_cnt < strlen(tx_bit_stream))
        {
            if(*(tx_bit_stream + bits_cnt) == '1'){
                g_target_position = BIT_1;
                printf("bit_cnt = %d, char 1 = %c\n", bits_cnt, *(tx_bit_stream + bits_cnt));
            }
            else if(*(tx_bit_stream + bits_cnt) == '0'){
                printf("bit_cnt = %d, char 0 = %c\n", bits_cnt, *(tx_bit_stream + bits_cnt));
                // printf("char 0 = %c\n", *(tx_bit_stream + bits_cnt));
                g_target_position = BIT_0; 
            }
            bits_cnt++;
        }
        else
        {
            bits_cnt = 0;
        }

    }
    free(tx_bit_stream);
    return 0;
}
int char_to_bit_stream_uart(Motor_InitStruct* motor){
    const char* tx_text = "Hello"; // he ---> 0100100001100101

    static int tim_count = 0;
    static int tim_flag = 0;
    const float BIT_1 = POSITION_INCREMENT;
    const float BIT_0 = 0;
    // float g_target_position = motor->pid_pos_init->target_position;
    // printf("tim is starting\n");
    if(tim_count > 1/PID_UPDATE_TIME){
            tim_count = 0;
            tim_flag = 1;
            // printf("tim is starting in time\n");
    }
    tim_count++;
    printf("tim count = %d\n", tim_count);

    static int bits_cnt = 0;
    char* tx_bit_stream = uart_bitstream(tx_text);
    // printf("timflag = %d\n", tim_flag);
    // printf("it will go into tim flag\n");
    if(tim_flag == 1){
        // printf("go into tim flag\n");
        printf("utf8 bit stream: '%s'--->%s\n", tx_text, tx_bit_stream);
        printf("str length = %d\n", strlen(tx_bit_stream));
        tim_flag = 0;
        if(bits_cnt < strlen(tx_bit_stream))
        {
            if(*(tx_bit_stream + bits_cnt) == '1'){
                motor->pid_pos_init->target_position = BIT_1;
                printf("bit_cnt = %d, char 1 = %c\n", bits_cnt, *(tx_bit_stream + bits_cnt));
            }
            else if(*(tx_bit_stream + bits_cnt) == '0'){
                printf("bit_cnt = %d, char 0 = %c\n", bits_cnt, *(tx_bit_stream + bits_cnt));
                // printf("char 0 = %c\n", *(tx_bit_stream + bits_cnt));
                motor->pid_pos_init->target_position = BIT_0; 
            }
            bits_cnt++;
        }
        else
        {
            bits_cnt = 0;
        }

    }
    free(tx_bit_stream);
    return 0;
}

int bit_stream_test(void) {
    // const char* text = "Hello world";
    const char* text = "H";
    char* bit_stream = utf8_to_bits(text);
    
    if (bit_stream != NULL) {
        printf("utf8 bit stream: '%s'--->%s\n", text, bit_stream);
        for(int i = 0; i < strlen(bit_stream); i++){
            printf("single char = %c\n", *(bit_stream+i));

        }
        printf("string length = %d\n", strlen(bit_stream));
        free(bit_stream);
    } else {
        printf("Memory allocation failed.\n");
    }
    
    return 0;
}
