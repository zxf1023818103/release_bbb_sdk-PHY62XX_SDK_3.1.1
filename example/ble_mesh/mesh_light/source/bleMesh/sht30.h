/**************************************************************************************************
    Filename:       sht30.h
    Revised:
    Revision:

    Description:    This file contains the SHT30 temperature and humidity sensor read application
                  definitions and prototypes.


**************************************************************************************************/

#ifndef SHT30_H
#define SHT30_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
    INCLUDES
*/

#include "OSAL.h"

/*********************************************************************
    CONSTANTS
*/

extern uint8 sht30_TaskID;   // Task ID for internal task/event processing

/*********************************************************************
    MACROS
*/

#define SHT30_SEND_READ_EVT                             0x0001
#define SHT30_DATA_RECEIVE_EVT                          0x0002
#define SHT30_RST_PULL_DOWN_EVT                         0x0004
#define SHT30_RST_PULL_UP_EVT                           0x0008

/*********************************************************************
    FUNCTIONS
*/

/*
    Task Initialization for the BLE Application
*/
extern void sht30_Init( uint8 task_id );

/*
    Task Event Processor for the BLE Application
*/
extern uint16_t sht30_ProcessEvent( uint8 task_id, uint16 events );

extern uint8 sht30_sensor_data[4];

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SHT30_H */
