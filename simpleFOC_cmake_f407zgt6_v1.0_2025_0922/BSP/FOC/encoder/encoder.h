
#ifndef ENCODER_H_
#define ENCODER_H_

#include "encoder_as5600.h"
#ifdef __cplusplus
extern "C"
{
#endif

#include "sensor.h"

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
        AS5600_t as5600;

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
    } Encoder_t;

    /**
     * Initialize Encoder
     * @param encoder - Pointer to Encoder structure
     * @param encA - encoder A pin
     * @param encB - encoder B pin
     * @param ppr - impulses per rotation (cpr=ppr*4)
     * @param index - index pin number (0 if not used)
     */
    void Encoder_init(Encoder_t *encoder, int encA, int encB, float ppr, int index);

    /**
     * Initialize encoder pins
     * @param encoder - Pointer to Encoder structure
     */
    void Encoder_init_hardware(Encoder_t *encoder);

    /**
     * Encoder interrupt callback functions
     */
    void Encoder_handleA(Encoder_t *encoder);
    void Encoder_handleB(Encoder_t *encoder);
    void Encoder_handleIndex(Encoder_t *encoder);

    /**
     * Get current angle (rad)
     * @param encoder - Pointer to Encoder structure
     * @return Current angle in radians
     */
    float Encoder_getAngle(Encoder_t *encoder);

    /**
     * Get current angular velocity (rad/s)
     * @param encoder - Pointer to Encoder structure
     * @return Current angular velocity in rad/s
     */
    float Encoder_getVelocity(Encoder_t *encoder);

    /**
     * Set current angle as zero angle
     * @param encoder - Pointer to Encoder structure
     * @return The angle [rad] difference
     */
    float Encoder_initRelativeZero(Encoder_t *encoder);

    /**
     * Set index angle as zero angle
     * @param encoder - Pointer to Encoder structure
     * @return The angle [rad] difference
     */
    float Encoder_initAbsoluteZero(Encoder_t *encoder);

    /**
     * Check if encoder has absolute zero capability
     * @param encoder - Pointer to Encoder structure
     * @return 0 - encoder without index, 1 - encoder with index
     */
    int Encoder_hasAbsoluteZero(Encoder_t *encoder);

    /**
     * Check if needs search for absolute zero
     * @param encoder - Pointer to Encoder structure
     * @return 0 - encoder without index, 1 - encoder with index
     */
    int Encoder_needsAbsoluteZeroSearch(Encoder_t *encoder);

    /**
     * Check if encoder has index pin
     * @param encoder - Pointer to Encoder structure
     * @return 1 if encoder has index pin, 0 if not
     */
    int Encoder_hasIndex(Encoder_t *encoder);

#ifdef __cplusplus
}
#endif

#endif /* ENCODER_H_ */
