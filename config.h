#include <sdkconfig.h>

//// PZEM config

#ifndef CONFIG_PZEM_NUM
#define CONFIG_PZEM_NUM     (1)
#endif /*CONFIG_PZEM_NUM*/

#ifndef CONFIG_PZEM_SERIAL
#define CONFIG_PZEM_SERIAL   Serial2
#endif /*CONFIG_PZEM_SERIAL*/

#ifndef CONFIG_PZEM_RX_PIN
#define CONFIG_PZEM_RX_PIN      (16)
#endif /*CONFIG_PZEM_RX_PIN*/

#ifndef CONFIG_PZEM_TX_PIN
#define CONFIG_PZEM_TX_PIN      (17)
#endif /*CONFIG_PZEM_TX_PIN*/


///// PLC Config

#ifndef CONFIG_PLC_SLAVEID
#define CONFIG_PLC_SLAVEID      (0x01)
#endif /*CONFIG_PLC_SLAVEID*/

#ifndef CONFIG_PLC_BAUD
#define CONFIG_PLC_BAUD         (57600)
#endif /*CONFIG_PLC_BAUD*/

#ifndef CONFIG_PLC_SERIAL
#define CONFIG_PLC_SERIAL       (Serial)
#endif /*CONFIG_PLC_SERIAL*/

// #ifndef CONFIG_PLC_TX_ENABLE_PIN
// #define CONFIG_PLC_TX_ENABLE_PIN   (2)
// #endif /*CONFIG_PLC_TX_ENABLE_PIN*/

// #ifndef CONFIG_PLC_TIMEOUT
// #define CONFIG_PLC_TIMEOUT      (1000)
// #endif /*CONFIG_PLC_TIMEOUT*/

// #ifndef CONFIG_PLC_POLLING
// #define CONFIG_PLC_POLLING      (500)
// #endif /*CONFIG_PLC_POLLING*/

// #ifndef CONFIG_PLC_RETRY
// #define CONFIG_PLC_RETRY        (10)
// #endif /*CONFIG_PLC_RETRY*/

// #ifndef CONFIG_PLC_TOTAL_REG
// #define CONFIG_PLC_TOTAL_REG    (7)
// #endif /*CONFIG_PLC_TOTAL_REG*/