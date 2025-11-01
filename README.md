# 正点原子F407ZGT6

# cmake

## for stm32 cmake project

1. in `CMakePresets.json`
rewrite '"hidden": true' to `"hidden":false`
2. generate cmake project

```bash
cmake --preset=default
```

3. build

```bash
cmake -B build/default
```

4. flash

```bash
cmake -B build/default flash
```

# as5600

## 实现不显式初始化AS5600就能使用

```c
// encoder.h
typedef struct {
    AS5600_t as5600_instance;  // 内嵌AS5600实例
    AS5600_t *as5600;           // 指向内嵌实例的指针
    float angle;
    // ...
} Encoder_t;

void Encoder_Init(Encoder_t *enc, I2C_HandleTypeDef *hi2c);

// encoder.c
void Encoder_Init(Encoder_t *enc, I2C_HandleTypeDef *hi2c)
{
    // 初始化内嵌的AS5600实例
    Encoder_AS5600_Init(&enc->as5600_instance, hi2c);
    
    // 指针指向内嵌实例
    enc->as5600 = &enc->as5600_instance;
    enc->angle = 0;
}

// 使用 - 不需要外部as5600变量！
Encoder_Init(&encoder, &hi2c1);
FOCMotor_linkEncoder(&FOCMotor, &encoder);
```
