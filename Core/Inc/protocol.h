#ifndef VERSION_H
#define VERSION_H

// Protocol version - increment when changing I2C command format
#define HP_LED_PROTOCOL_VERSION 1

// Command types
#define CMD_GET_VERSION     0x00  // Get protocol version
#define CMD_SET_COLOR       0x01  // Set LED color
// Add more commands as needed:
// #define CMD_SET_BRIGHTNESS  0x02
// #define CMD_GET_TEMP        0x03
// etc.

#endif // VERSION_H