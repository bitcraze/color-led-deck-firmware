#ifndef VERSION_H
#define VERSION_H

// Protocol version - increment when changing I2C command format
#define HP_LED_PROTOCOL_VERSION 1

#define RXBUFFERSIZE  5
#define TXBUFFERSIZE  3

// Command types
#define CMD_GET_VERSION         0x00  // Get protocol version
#define CMD_SET_COLOR           0x01  // Set LED color
#define CMD_GET_THERMAL_STATUS  0x02  // Get thermal status

#endif // VERSION_H