/*
 * Sensor.h
 * Ported from: https://github.com/simplefoc/Arduino-FOC/blob/master/src/common/base_classes/Sensor.h
 * Converted from C++ to C
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Direction enumeration
 */
typedef enum {
    Direction_CW      = 1,  //clockwise
    Direction_CCW     = -1, // counter clockwise
    Direction_UNKNOWN = 0   //not yet known or invalid state
} Direction_t;

/**
 * Pullup configuration enumeration  
 */
typedef enum {
    Pullup_INTERN, //!< Use internal pullups
    Pullup_EXTERN  //!< Use external pullups
} Pullup_t;

/**
 * Forward declaration for sensor function pointers
 */
typedef struct Sensor_t Sensor_t;

/**
 * Sensor function pointers structure
 */
typedef struct {
    /** get current angle (rad) */
    float (*getAngle)(Sensor_t* sensor);
    /** get current angular velocity (rad/s)*/
    float (*getVelocity)(Sensor_t* sensor);
    /**
     * set current angle as zero angle
     * return the angle [rad] difference
     */
    float (*initRelativeZero)(Sensor_t* sensor);
    /**
     * set absolute zero angle as zero angle
     * return the angle [rad] difference
     */
    float (*initAbsoluteZero)(Sensor_t* sensor);
    /**
     * returns 0 if it has no absolute 0 measurement
     * 0 - incremental encoder without index
     * 1 - encoder with index & magnetic sensors
     */
    int (*hasAbsoluteZero)(Sensor_t* sensor);
    /**
     * returns 0 if it does need search for absolute zero
     * 0 - magnetic sensor (& encoder with index which is found)
     * 1 - encoder with index (with index not found yet)
     */
    int (*needsAbsoluteZeroSearch)(Sensor_t* sensor);
} SensorFunctions_t;

/**
 * Sensor base structure
 * Each sensor implementation should include this as the first member
 */
struct Sensor_t {
    SensorFunctions_t* functions; //!< Function pointer table
    // if natural_direction == Direction_CCW then direction will be flipped to CW
    int natural_direction; //!< Natural direction (Direction_CW by default)
};

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_H_ */
