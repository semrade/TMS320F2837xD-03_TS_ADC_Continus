/***********************************************************************************
 * File                     :main.c
 *
 * Title                    :
 *
 * Author                   :Tarik SEMRADE
 *
 * Description              :ADC SOC0 to SOC15 Continues tutorial project
 *
 * Version                  :
 *
 * Copyright (c) 2020 Tarik SEMRADE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************************/

/**********************************************************************************
 *  Included Files
 *
 *********************************************************************************/
#include "F28x_Project.h"
#include "device.h"
#include "main.h"
#include "TS_SysMng_Adc.h"
#include "adc.h"
/**********************************************************************************
 *  Defines
 *
 *********************************************************************************/
#define ADC_RESOLUTION_SIGNALMODE 4095

/**********************************************************************************
 *  Global Variables
 *
 *********************************************************************************/
/* Store hexadecimal values to u16 buffer */
float32 t_rAdcResults[ADC_BUFFER_LENGTH];
Uint16 s_u16ResultsIndex = 0;
/**********************************************************************************
 * \function:       main
 * \brief           main `0` numbers
 * \param[in]       void
 * \return          void
 **********************************************************************************/
void
main (void)
{
    /***************************CPU Initialization*********************************/
    /* Set up system flash and turn peripheral clocks */
    InitSysCtrl();

    /* GPIO Init */
    InitGpio();

    /* Globally disable maskable CPU interrupts */
    DINT;

    /* Clear and disable all PIE interrupts */
    InitPieCtrl();

    /* Individually disable maskable CPU interrupts */
    IER = 0x0000;

    /* Clear all CPU interrupt flags */
    IFR = 0x0000;

    /* Populate the PIE interrupt vector table with */
    InitPieVectTable();

    /***********************Interrupt linking functions*****************************/
    /* Timer0 ISR function linking */
    EALLOW;
    PieVectTable.TIMER0_INT = &Timer0_ISR;
    EDIS;
    /************************Peripheral Initialization*****************************/
    /* Initialize all timer with default values */
    InitCpuTimers();

    /* 200MHz CPU Freq, 50 uSeconds Period (in uSeconds) */
    ConfigCpuTimer(&CpuTimer0, 200, 100);

    /* Enable TINT0 in the PIE: Group 1 interrupt 7 */
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    /* Enable group 1 interrupts */
    IER |= M_INT1;

    /* Enable Global interrupt INTM */
    EINT;

    /* Start Timer0 */
    StartCpuTimer0();

    /* ADC Reset */
    TS_SysMng_AdcConfig();

    /* ADC channel selection */
    TS_SysMng_AdcContConfig(ADC_CHANNEL_0,19);

    /* Main loop */
    while(1)
    {
        asm(" NOP");
    }

}
/**********************************************************************************
 * \function:       Timer0_ISR
 * \brief           `0` Param
 * \param[in]       void
 * \return          void
 **********************************************************************************/
interrupt void
Timer0_ISR(void)
{

    /* Clear Timer interrupt flag */
    CpuTimer0Regs.TCR.bit.TIF = 0;

    /* Increment timer0 interrupt counter */
    CpuTimer0.InterruptCount++;

    /* Wait Acquisition ending for SOC0 to SOC3 */
    while(0 == AdcaRegs.ADCINTFLG.bit.ADCINT1);

    /* Clear interrupt flag */
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

    /* Store data and compute the first acquisition */
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT0 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT1 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT2 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT3 * (3.3f / ADC_RESOLUTION_SIGNALMODE);

    /* Wait Acquisition ending for SOC4 to SOC7*/
    while(0 == AdcaRegs.ADCINTFLG.bit.ADCINT2);

    /* Clear interrupt flag */
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;

    /* Store data and compute the second acquisition */
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT4 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT5 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT6 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT7 * (3.3f / ADC_RESOLUTION_SIGNALMODE);

    /* Wait Acquisition ending for SOC8 to SOC11*/
    while(0 == AdcaRegs.ADCINTFLG.bit.ADCINT3);

    /* Clear interrupt flag */
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT3 = 1;

    /* Force SOC12 to SOC15 By software */
    AdcaRegs.ADCSOCFRC1.all = ADC_FORCE_SOC12|ADC_FORCE_SOC13|ADC_FORCE_SOC14|ADC_FORCE_SOC15;

    /* Store data and compute the tired acquisition */
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT8 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT9 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT10 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT11 * (3.3f / ADC_RESOLUTION_SIGNALMODE);

    /* Wait Acquisition ending for SOC12 to SOC15*/
    while(0 == AdcaRegs.ADCINTFLG.bit.ADCINT4);

    /* Clear interrupt flag */
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT4 = 1;

    /* Store data and compute the fourth acquisition */
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT12 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT13 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT14 * (3.3f / ADC_RESOLUTION_SIGNALMODE);
    t_rAdcResults[s_u16ResultsIndex++] = AdcaResultRegs.ADCRESULT15 * (3.3f / ADC_RESOLUTION_SIGNALMODE);

    /* Rewind indexer to the first element */
    s_u16ResultsIndex = 0;

    /* Acknowledge this interrupt to receive more interrupts from group 1 */
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}
