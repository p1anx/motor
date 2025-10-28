#include "magneticSensor.h"
#include "common.h"

extern GlobalVar_t g_var;

float MagneticSenor_getValue(void)
{
    return g_var.B_avg;
}
