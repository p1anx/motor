
#ifndef ENCODER_H_
#define ENCODER_H_

#include "encoder_as5600.h"
#ifdef __cplusplus
extern "C"
{
#endif

#include "sensor.h"
#include "mt6835.h"

    typedef enum
    {
        EncoderType_AS5600,
        EncoderType_MT6835,
    } EncoderType_enum;
    typedef enum
    {
        Encoder_OK,
        Encoder_ERROR,

    }EncoderStatus_t;
    /**
     * Quadrature mode configuration enumeration
     */
    typedef enum
    {
        Quadrature_ON, //!< Enable quadrature mode CPR = 4xPPR
        Quadrature_OFF //!< Disable quadrature mode / CPR = PPR
    } Quadrature_t;

    /**
     * Encoder structure
     */

    typedef struct
    {
        Sensor_t sensor; //!< Base sensor structure (must be first member)
        AS5600_t *as5600;
        AS5600_t as5600_Instance;
        AS5600_t AS5600;
        MT6835_t mt6835;
        EncoderType_enum EncoderType;

        // Hardware pins
        int pinA;      //!< encoder hardware pin A
        int pinB;      //!< encoder hardware pin B
        int index_pin; //!< index pin

        // Encoder configuration
        Pullup_t pullup;         //!< Configuration parameter internal or external pullups
        Quadrature_t quadrature; //!< Configuration parameter enable or disable quadrature mode
        float cpr;               //!< encoder cpr number

        // Internal state variables
        volatile long pulse_counter;       //!< current pulse counter
        volatile long pulse_timestamp;     //!< last impulse timestamp in us
        volatile int A_active;             //!< current active states of A channel
        volatile int B_active;             //!< current active states of B channel
        volatile int I_active;             //!< current active states of Index channel
        volatile long index_pulse_counter; //!< impulse counter number upon first
                                           //!< index interrupt

        // velocity calculation variables
        float prev_Th, pulse_per_second;
        volatile long prev_pulse_counter, prev_timestamp_us;

        float angle;
        float initial_angle_offset;
        uint32_t delta_t_us;
    } Encoder_t;
    int Encoder_init(Encoder_t *encoder);
    void Encoder_linkAS5600(Encoder_t *encoder, AS5600_t *as5600);
    float Encoder_getAngle(Encoder_t *encoder);
    float Encoder_getVelocity(Encoder_t *encoder);
    float Encoder_getVelocityRPM(Encoder_t *enc);
    EncoderStatus_t Encoder_init0(Encoder_t *encoder);
    float Encoder_getAngleDegrees(Encoder_t *encoder);

#ifdef __cplusplus
}
#endif

#endif /* ENCODER_H_ */
