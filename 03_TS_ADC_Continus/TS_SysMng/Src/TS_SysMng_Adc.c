/***********************************************************************************
 * File              :TS_SysMng_ADC.c
 *
 * Title             :
 *
 * Author            :Tarik SEMRADE
 *
 * Created on        :Mar 19, 2020
 *
 * Version           :
 *
 * Description       : ADC Reset and management
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
#include "TS_SysMng_Adc.h"
#include "F28x_Project.h"
#include "F2837xD_Adc_defines.h"
#include "TS_SysMng/TS_SysMng_X.h"
#include "adc.h"
/**********************************************************************************
 * \function:       TS_SysMng_AdcConfig
 * \brief           main `0` numbers
 * \param[in]       void
 * \return          void
 **********************************************************************************/
void
TS_SysMng_AdcConfig(void)
{
    /* Enable EALLOW protected register access */
    EALLOW;

    /* ADC reset*/
    DevCfgRegs.SOFTPRES13.bit.ADC_A = 1;

    /* ADC is released from reset */
    DevCfgRegs.SOFTPRES13.bit.ADC_A = 0;

    /* Interrupt pulse position when end of conversion occurs, later mode */
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 0x1;

    /* Adc clock configuration is 50Mhz */
    AdcaRegs.ADCCTL2.bit.PRESCALE = ADC_CLK_DIV_4_0;

    /* Call AdcSetMode() to configure the resolution and signal mode. */
    /* This also performs the correct ADC calibration for the configured mode. */
    AdcSetMode(ADC_ADCA, ADC_BITRESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    /*
     * SOC0 to SOC3 is triggered by Timer0 interrupt
     * */
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = ADC_TRIGGER_CPU1_TINT0;
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = ADC_TRIGGER_CPU1_TINT0;
    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = ADC_TRIGGER_CPU1_TINT0;
    AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = ADC_TRIGGER_CPU1_TINT0;

    /* No ADC interrupt triggers SOC0 to SOC3 */
    AdcaRegs.ADCINTSOCSEL1.bit.SOC0 = 0;
    AdcaRegs.ADCINTSOCSEL1.bit.SOC1 = 0;
    AdcaRegs.ADCINTSOCSEL1.bit.SOC2 = 0;
    AdcaRegs.ADCINTSOCSEL1.bit.SOC3 = 0;


    AdcaRegs.ADCSOC12CTL.bit.TRIGSEL = ADC_TRIGGER_SW_ONLY;
    AdcaRegs.ADCSOC13CTL.bit.TRIGSEL = ADC_TRIGGER_SW_ONLY;
    AdcaRegs.ADCSOC14CTL.bit.TRIGSEL = ADC_TRIGGER_SW_ONLY;
    AdcaRegs.ADCSOC15CTL.bit.TRIGSEL = ADC_TRIGGER_SW_ONLY;

    /* Enable Adc-a interrupts 1..4 */
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;
    AdcaRegs.ADCINTSEL1N2.bit.INT2E = 1;
    AdcaRegs.ADCINTSEL3N4.bit.INT3E = 1;
    AdcaRegs.ADCINTSEL3N4.bit.INT4E = 1;

    /* No interrupt event synchronization for all Adc-a Interrupts */
    AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 1;
    AdcaRegs.ADCINTSEL1N2.bit.INT2CONT = 1;
    AdcaRegs.ADCINTSEL3N4.bit.INT3CONT = 1;
    AdcaRegs.ADCINTSEL3N4.bit.INT4CONT = 1;

    /* End of conversion will trigger interrupts 1..4 */
    /* End of SOC3 will set INT1 flag */
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 3;

    /* End of SOC7 will set INT2 flag */
    AdcaRegs.ADCINTSEL1N2.bit.INT2SEL = 7;

    /* End of SOC11 will set INT3 flag */
    AdcaRegs.ADCINTSEL3N4.bit.INT3SEL = 11;

    /* End of SOC15 will set INT4 flag */
    AdcaRegs.ADCINTSEL3N4.bit.INT4SEL = 15;


    /* ADCINT1 will trigger second 4 SOCs */
    AdcaRegs.ADCINTSOCSEL1.bit.SOC4 = 1;
    AdcaRegs.ADCINTSOCSEL1.bit.SOC5 = 1;
    AdcaRegs.ADCINTSOCSEL1.bit.SOC6 = 1;
    AdcaRegs.ADCINTSOCSEL1.bit.SOC7 = 1;

    /* ADCINT2 will trigger  tired 4 SOCs */
    AdcaRegs.ADCINTSOCSEL2.bit.SOC8 = 2;
    AdcaRegs.ADCINTSOCSEL2.bit.SOC9 = 2;
    AdcaRegs.ADCINTSOCSEL2.bit.SOC10 = 2;
    AdcaRegs.ADCINTSOCSEL2.bit.SOC11 = 2;


    /*
     * => Power up ADC-A
     * => Wait 1 ms after power-up before using the ADC
     * => Disable EALLOW protected register access
     * */
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    DELAY_US(1000);
    EDIS;

}
/**********************************************************************************
 * \function:       TS_SysMng_AdcContConfig
 * \brief           Channel selection and S/H time definition.
 * \param[in]       Channel, Acdcs S/H time
 * \return          void
 **********************************************************************************/
void
TS_SysMng_AdcContConfig(Uint16 p_u16Channel, Uint16 p_u16Acdps)
{
    EALLOW;

    /* Channel selection for all SOC 0..15 */
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC1CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC2CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC3CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC4CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC5CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC6CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC7CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC8CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC9CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC10CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC11CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC12CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC13CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC14CTL.bit.CHSEL = p_u16Channel;
    AdcaRegs.ADCSOC15CTL.bit.CHSEL = p_u16Channel;

    /* The same acquisition ending for all SOC 0..15 */
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC3CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC4CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC5CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC6CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC7CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC8CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC9CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC10CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC11CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC12CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC13CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC14CTL.bit.ACQPS = p_u16Acdps;
    AdcaRegs.ADCSOC15CTL.bit.ACQPS = p_u16Acdps;

    EDIS;
}
