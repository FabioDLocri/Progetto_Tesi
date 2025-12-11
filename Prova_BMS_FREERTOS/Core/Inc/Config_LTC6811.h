/*
 * Config_LTC6811.h
 *
 *  Created on: Nov 16, 2025
 *      Author: faxio
 */

#ifndef INC_CONFIG_LTC6811_H_
#define INC_CONFIG_LTC6811_H_

//Definiamo i comandi da inviare a LTC6811
#define WRCFGA 	0x0001 	// Write Configuration Register Group A
#define RDCFGA 	0x0002 	// Read Configuration Register Group A
#define RDCVA 	0x0004 	// Read Cell Voltage Register Group A
#define RDCVB 	0x0006 	// Read Cell Voltage Register Group B
#define RDCVC 	0x0008 	// Read Cell Voltage Register Group C
#define RDCVD 	0x000A 	// Read Cell Voltage Register Group D
#define RDAUXA	0x000C	// Read Auxiliary Register Group A
#define RDAUXB	0x000E	// Read Auxiliary Register Group B
#define RDSTATA 0X0010 	// Read Status Register Group A
#define RDSTATB 0X0012	// Read Status Register Group A

//Definiamo varie variabili che possono essere utili nel comporre alcuni comandi.

#define MD_422HZ_1KHZ 0
#define MD_27KHZ_14KHZ 1
#define MD_7KHZ_3KHZ 2
#define MD_26HZ_2KHZ 3

#define ADC_OPT_ENABLED 1
#define ADC_OPT_DISABLED 0

#define CELL_CH_ALL 0
#define CELL_CH_1and7 1
#define CELL_CH_2and8 2
#define CELL_CH_3and9 3
#define CELL_CH_4and10 4
#define CELL_CH_5and11 5
#define CELL_CH_6and12 6

#define SELFTEST_1 1
#define SELFTEST_2 2

#define AUX_CH_ALL 0
#define AUX_CH_GPIO1 1
#define AUX_CH_GPIO2 2
#define AUX_CH_GPIO3 3
#define AUX_CH_GPIO4 4
#define AUX_CH_GPIO5 5
#define AUX_CH_VREF2 6

#define STAT_CH_ALL 0
#define STAT_CH_SOC 1
#define STAT_CH_ITEMP 2
#define STAT_CH_VREGA 3
#define STAT_CH_VREGD 4

#define DCP_DISABLED 0
#define DCP_ENABLED 1

#define PULL_UP_CURRENT 1
#define PULL_DOWN_CURRENT 0

#define NUM_RX_BYT 8
#define CELL 1
#define AUX 2
#define STAT 3

#define CFGRA 0
#define CFGRB 4

#define NUM_CELLS 12
#define NUM_IC 1

//Tensioni di UnderVoltage e OverVoltage
#define UV_THRESHOLD 33000
#define OV_THRESHOLD 42000

#endif /* INC_CONFIG_LTC6811_H_ */
