//
// Created by xwj on 12/29/25.
//

/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2021 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* * This software may in its unmodified form be freely redistributed *
*   in source form.                                                  *
* * The source code may be modified, provided the source code        *
*   retains the above copyright notice, this list of conditions and  *
*   the following disclaimer.                                        *
* * Modified versions of this software in source or linkable form    *
*   may not be distributed without prior consent of SEGGER.          *
* * This software may only be used for communication with SEGGER     *
*   J-Link debug probes.                                             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
---------------------------END-OF-HEADER------------------------------
  Sample implementation for using J-Scope with RTT

  Generates 3 sine waveforms and sends data to J-Scope application
  using RTT.
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "SEGGER_RTT.h"

#define PI 3.14159265358979323846f

#define SINUS_SAMPLE_DATA  500 // Number of Sinus samples per 90
#define SINUS_SCALE      12000 // SIN_MAX = sin(90) = 1 * SINUS_SCALE (PI/2*SINUS_SCALE)

extern char JS_RTT_UpBuffer[4096];    // J-Scope RTT Buffer
extern int  JS_RTT_Channel;       // J-Scope RTT Channel

/* Static sinus lookup table */
static unsigned _aSin[SINUS_SAMPLE_DATA];
static unsigned _SinAShift;
static unsigned _SinBShift;
static unsigned _SinCShift;
static int _Timestamp;

int Sine1;                     // Sine wave 1
int Sine2;                     // Sine wave 2
int Sine3;                     // Sine wave 3

/*********************************************************************
*
*       _CalcSinusData()
*
* Function description
*   Fills _aSin[SINUS_SAMPLE_DATA] with data values
*
*/
static void _CalcSinusData(void) {
  unsigned i;
  for (i = 0; i < SINUS_SAMPLE_DATA; i++) {
    _aSin[i] = (sin((float)i * PI / (2. * SINUS_SAMPLE_DATA)) * SINUS_SCALE); // Calcs sin data for 0-90? (0-PI/2)
  }
  _SinAShift = SINUS_SAMPLE_DATA * 0.00000f;
  _SinBShift = SINUS_SAMPLE_DATA * 1.33333f;
  _SinCShift = SINUS_SAMPLE_DATA * 2.66667f;
}

/*********************************************************************
*
*       _GetSinVal()
*
* Function description
*   Returns the corresponding y-value for the x-Value passed
*
*  Parameter
*    XVal - 0 <= XVal < (4 * SINUS_SAMPLE_DATA)
*/
int _GetSinVal(unsigned XVal) {
  switch (XVal / SINUS_SAMPLE_DATA) {
  case 0:
    return _aSin[XVal];                                          // Values 0?-90?   = "normal"
  case 1:
    return _aSin[(((2 * SINUS_SAMPLE_DATA) - 1) - XVal)];        // Values 90?-180? = 0?-90? in reverse order (=> 89? <=> 91?; 30? <=> 150? etc)
  case 2:
    return _aSin[XVal - (2 * SINUS_SAMPLE_DATA)] * (-1);         //Values 180?-270? = 0-90? inverted
  case 3:
    return _aSin[(((4 * SINUS_SAMPLE_DATA) - 1) - XVal)] * (-1); //Values 270?-360? = 90?-180? inverted
  default:
    return 0;
  }
}

void SineLUT(void) {
  //
  // RTT block structure
  //
  #pragma pack(push, 1)
  struct {
    unsigned int Timestamp;
    signed int Sine1;
    signed int Sine2;
    signed int Sine3;
  } acValBuffer;
  #pragma pack(pop)

  static unsigned XCnt;

  _Timestamp++;
  Sine1 = _GetSinVal((XCnt + _SinAShift) % (4 * SINUS_SAMPLE_DATA));
  Sine2 = _GetSinVal((XCnt + _SinBShift) % (4 * SINUS_SAMPLE_DATA));
  Sine3 = _GetSinVal((XCnt + _SinCShift) % (4 * SINUS_SAMPLE_DATA));
  //
  // Send over RTT channel
  // For the example we use a "fake" timestamp which is simply a counting variable.
  // This timestamp will be interpreted as [us] by J-Scope.
  //
  acValBuffer.Timestamp = _Timestamp;
  acValBuffer.Sine1 = Sine1;
  acValBuffer.Sine2 = Sine2;
  acValBuffer.Sine3 = Sine3;
  SEGGER_RTT_Write(JS_RTT_Channel, &acValBuffer, sizeof(acValBuffer));
  XCnt++;
}

int main_js_rtt(void) {
  SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_T4I4I4I4", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
  _CalcSinusData(); // Init sinus lookup table
  _Timestamp = 0;
  while (1) {
    SineLUT();
  }
}
