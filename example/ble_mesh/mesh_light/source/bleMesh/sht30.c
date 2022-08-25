/**************************************************************************************************
    Filename:       sht30.c
    Revised:
    Revision:

    Description:    This file contains the SHT30 temperature and humidity sensor read application.


**************************************************************************************************/

/*********************************************************************
    INCLUDES
*/

#include "sht30.h"
#include "i2c.h"
#include "log.h"
#include <stdio.h>

#include "OSAL.h"
#include "OSAL_Timers.h"

#include "access_extern.h"
#include "MS_access_api.h"
#include "MS_common.h"

#include "vendormodel_common.h"

/*********************************************************************
    MACROS
*/

#define SHT31_DEFAULT_ADDR 0x0044
#define SHT31_MEAS_HIGHREP_STRETCH 0x2C06
#define SHT31_MEAS_MEDREP_STRETCH 0x2C0D
#define SHT31_MEAS_LOWREP_STRETCH 0x2C10
#define SHT31_MEAS_HIGHREP 0x2400
#define SHT31_MEAS_MEDREP 0x240B
#define SHT31_MEAS_LOWREP 0x2416
#define SHT31_READSTATUS 0xF32D
#define SHT31_CLEARSTATUS 0x3041
#define SHT31_SOFTRESET 0x30A2
#define SHT31_HEATEREN 0x306D
#define SHT31_HEATERDIS 0x3066

/*********************************************************************
    CONSTANTS
*/

/*********************************************************************
    TYPEDEFS
*/

/*********************************************************************
    GLOBAL VARIABLES
*/

uint8 sht30_sensor_data[4];

/*********************************************************************
    EXTERNAL VARIABLES
*/

extern MS_ACCESS_MODEL_HANDLE UI_vendor_defined_server_model_handle;

/*********************************************************************
    EXTERNAL FUNCTIONS
*/

/*********************************************************************
    LOCAL VARIABLES
*/

uint8 sht30_TaskID;      // Task ID for internal task/event processing
AP_I2C_TypeDef *pi2cdev; // I2C handle

/*********************************************************************
    LOCAL FUNCTIONS
*/

static int write_command(uint16_t cmd)
{
    hal_i2c_addr_update(pi2cdev, SHT31_DEFAULT_ADDR);

    uint8 data[2] = {cmd >> 8, cmd & 0xff};

    HAL_ENTER_CRITICAL_SECTION();
    hal_i2c_tx_start(pi2cdev);
    hal_i2c_send(pi2cdev, data, sizeof data);
    HAL_EXIT_CRITICAL_SECTION();

    return hal_i2c_wait_tx_completed(pi2cdev);
}

static uint8 crc8(uint8 *data, int len)
{
    const uint8 POLYNOMIAL = 0x31;
    uint8 crc = 0xFF;
    for (int j = len; j; --j)
    {
        crc ^= *data++;
        for (int i = 8; i; --i)
        {
            crc = (crc & 0x80)
                      ? (crc << 1) ^ POLYNOMIAL
                      : (crc << 1);
        }
    }
    return crc;
}

/*********************************************************************
    PROFILE CALLBACKS
*/

/*********************************************************************
    PUBLIC FUNCTIONS
*/

void sht30_Init(uint8 task_id)
{
    sht30_TaskID = task_id;

    hal_gpio_pull_set(P16, GPIO_PULL_DOWN);
    hal_gpio_write(P16, 1);

    osal_set_event(sht30_TaskID, SHT30_SEND_READ_EVT);
}

uint16 sht30_ProcessEvent(uint8 task_id, uint16 events)
{
    VOID task_id; // OSAL required parameter that isn't used in this function

    if (events & SHT30_RST_PULL_DOWN_EVT)
    {

        hal_gpio_fast_write(P16, 0);
        osal_start_timerEx(sht30_TaskID, SHT30_RST_PULL_UP_EVT, 300);
    }

    if (events & SHT30_RST_PULL_UP_EVT)
    {
        hal_gpio_fast_write(P16, 1);
        osal_start_timerEx(sht30_TaskID, SHT30_SEND_READ_EVT, 300);
    }

    if (events & SHT30_SEND_READ_EVT)
    {
        hal_i2c_pin_init(I2C_0, P2, P3);
        pi2cdev = hal_i2c_init(I2C_0, I2C_CLOCK_100K);

        write_command(SHT31_MEAS_HIGHREP);
        osal_start_timerEx(sht30_TaskID, SHT30_DATA_RECEIVE_EVT, 3000);
    }

    if (events & SHT30_DATA_RECEIVE_EVT)
    {
        uint8 buffer[6];
        osal_memset(buffer, 0, sizeof buffer);

        int ret = hal_i2c_read(pi2cdev, SHT31_DEFAULT_ADDR, 0, buffer, sizeof buffer);
        if (ret == PPlus_ERR_TIMEOUT)
        {
            LOG("i2c read timeout, reset sensor\r\n");
            osal_set_event(sht30_TaskID, SHT30_RST_PULL_DOWN_EVT);
        }
        else
        {
            // LOG("received data:");
            // LOG_DUMP_BYTE(buffer, sizeof buffer);
            
            do
            {
                uint16 ST, SRH;
                ST = buffer[0];
                ST <<= 8;
                ST |= buffer[1];

                if (buffer[2] != crc8(buffer, 2))
                {
                    break;
                }

                SRH = buffer[3];
                SRH <<= 8;
                SRH |= buffer[4];

                if (buffer[5] != crc8(buffer + 3, 2))
                {
                    break;
                }

                int temp = ST;
                temp *= 17500;
                temp /= 0xffff;
                temp = -4500 + temp;

                uint8 signature = temp < 0;
                if (temp < 0) {
                    temp = -temp;
                }
                uint8 temperature_integer = temp / 100;
                uint8 temperature_decimal = temp % 100;

                int humidity = SRH;
                humidity *= 10000;
                humidity /= 0xFFFF;

                uint8 humidity_integer = humidity / 100;

                uint8 sensor_data[4] = {
                    signature,
                    temperature_integer,
                    temperature_decimal,
                    humidity_integer,
                };

                if (EM_mem_cmp(sensor_data, sht30_sensor_data, sizeof sensor_data))
                {
                    UCHAR buffer[20];
                    UINT16 marker = 0;
                    buffer[marker] = ++vendor_tid;
                    marker++;
					MS_PACK_LE_2_BYTE_VAL(&buffer[marker], MS_STATE_VENDORMODEL_SENSOR_T);
                    marker += 2;
                    EM_mem_copy(&buffer[marker], sensor_data, sizeof sensor_data);
                    marker += sizeof sensor_data;
                    MS_access_raw_data(
                        &UI_vendor_defined_server_model_handle,
                        MS_ACCESS_VENDORMODEL_STATUS_OPCODE,
                        0x0001,
                        0x0000,
                        buffer,
                        marker,
                        MS_FALSE);

                    //LOG("publish sensor data ");
                    //LOG_DUMP_BYTE(sensor_data, sizeof sensor_data);
                }

                EM_mem_copy(sht30_sensor_data, sensor_data, sizeof sensor_data);

            } while (0);

            osal_start_timerEx(sht30_TaskID, SHT30_SEND_READ_EVT, 500);
        }
    }

    // Discard unknown events
    return 0;
}

/*********************************************************************
*********************************************************************/
