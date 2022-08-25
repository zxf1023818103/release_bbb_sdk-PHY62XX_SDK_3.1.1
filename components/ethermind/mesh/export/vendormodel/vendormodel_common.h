/**
    \file appl_config_client.h

    \brief This file defines the Mesh Configuration Model Application Interface
    - includes Data Structures and Methods for both Server and Client.
*/

/*
    Copyright (C) 2018. Mindtree Ltd.
    All rights reserved.
*/

#ifndef _H_VENDORMODEL_COMMON_
#define _H_VENDORMODEL_COMMON_


/* --------------------------------------------- Header File Inclusion */
#include "MS_common.h"



// vendor model att
// #define MS_STATE_VENDORMODEL_HSL_T                                     0x0123
// #define MS_STATE_VENDORMODEL_ONOFF_T                                   0x0100
// #define MS_STATE_VENDORMODEL_LIGHTNESS_T                               0x0121
// #define MS_STATE_VENDORMODEL_CTL_T                                     0x0122
// #define MS_STATE_VENDORMODEL_MAINLIGHTONOFF_T                          0x0534
// #define MS_STATE_VENDORMODEL_BACKLIGHTONOFF_T                          0x0533
// #define MS_STATE_VENDORMODEL_HB_CALLBACK_T                             0x0801
// #define MS_STATE_VENDORMODEL_RESET_T                                   0x0802
// #define MS_STATE_VENDORMODEL_MODENUMBER_T                              0xF004
// #define MS_STATE_VENDORMODEL_EVENT_INDICATE_T                          0xF009
#define MS_STATE_VENDORMODEL_NOTIFY_T                                  0xFFFE
#define MS_STATE_VENDORMODEL_HEARTBEAT_T                               0x0000
#define MS_STATE_VENDORMODEL_ONOFF_T                                   0x0100
#define MS_STATE_VENDORMODEL_LIGHTNESS_T                               0x0200
#define MS_STATE_VENDORMODEL_RGB_T                                     0x0300
#define MS_STATE_VENDORMODEL_SENSOR_T                                  0x0400
#define MS_STATE_VENDORMODEL_IRBODY_T                                  0x0500
#define MS_STATE_VENDORMODEL_RESET_T                                   0x0900



//phy model opcode
#define MS_ACCESS_VENDORMODEL_GET_OPCODE                               0x00D08888//0405
#define MS_ACCESS_VENDORMODEL_SET_OPCODE                               0x00D18888//0405
#define MS_ACCESS_VENDORMODEL_SET_UNACKNOWLEDGED_OPCODE                0x00D28888//0405
#define MS_ACCESS_VENDORMODEL_STATUS_OPCODE                            0x00D38888//0405
#define MS_ACCESS_VENDORMODEL_INDICATION_OPCODE                        0x00D48888//0405
#define MS_ACCESS_VENDORMODEL_CONFIRMATION_OPCODE                      0x00D58888//0405
#define MS_ACCESS_VENDORMODEL_WRITECMD_OPCODE                          0x00E08888//0405
#define MS_ACCESS_VENDORMODEL_NOTIFY_OPCODE                            0x00E18888//0405

#define MS_MODEL_ID_VENDORMODEL_SERVER                                 0x00008888//0504
#define MS_MODEL_ID_VENDORMODEL_CLIENT                                 0x00018888//0504





/* --------------------------------------------- Global Definitions */


/* --------------------------------------------- Data Types/ Structures */
typedef struct _MS_access_vendormodel_state_params
{
    /** State Type */
    UINT16 vendormodel_type;

    /** State pointer */
    UCHAR* vendormodel_param;

} MS_ACCESS_VENDORMODEL_STATE_PARAMS;





/* --------------------------------------------- Function */

#endif /*_H_APPL_CONFIG_CLIENT_ */
