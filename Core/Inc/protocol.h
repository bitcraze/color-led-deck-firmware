/**
 * ,---------,       ____  _ __
 * |  ,-^-,  |      / __ )(_) /_______________ _____  ___
 * | (  O  ) |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * | / ,--´  |    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *    +------`   /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2025 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * protocol.h - I2C protocol version and command definitions
 */

#ifndef VERSION_H
#define VERSION_H

// Protocol version - increment when changing I2C command format
#define COLOR_LED_PROTOCOL_VERSION 2

#define RXBUFFERSIZE  5
#define TXBUFFERSIZE  9  // CMD (1) + 4 × uint16_t current values (8)

// Command types
#define CMD_GET_VERSION         0x00  // Get protocol version
#define CMD_SET_COLOR           0x01  // Set LED color
#define CMD_GET_THERMAL_STATUS  0x02  // Get thermal status
#define CMD_GET_LED_POSITION    0x03  // Get LED mounting position
#define CMD_GET_LED_CURRENT     0x04  // Get LED current readings

// LED position values
#define LED_POS_NONE            0x00  // No LED / floating
#define LED_POS_BOTTOM          0x01  // Bottom-mounted
#define LED_POS_TOP             0x02  // Top-mounted

#endif // VERSION_H