#ifndef _LIB_DRIVER_H
#define _LIB_DRIVER_H

#include "default.h"

/******************************
 * Definitions
 ******************************/

#define COMMANDS_USART_FLAG       0xB3

/******************************
 * Datatypes
 ******************************/

typedef enum __attribute__ (( packed )) {
  COMMAND_MOTOR_MX = 0,
  COMMAND_MOTOR_MR
} command_motor_t;

typedef enum __attribute__ (( packed )) {
  COMMAND_TYPE_ENABLE = 0,    /* Disables specified motor */
  COMMAND_TYPE_DISABLE,       /* Enables specified motor */
  COMMAND_TYPE_MOVE,          /* Move to POS */
  COMMAND_TYPE_EAHOME         /* Enter Auto-Homing Mode */
} command_type_t;

typedef struct __attribute__ (( packed )) {
  unsigned reserved : 8;
} command_packet_flags_t;

typedef struct __attribute__ (( packed )) {
  command_type_t type;
  command_packet_flags_t flags;
} command_packet_header_t;

typedef struct __attribute__ (( packed )) {
  uint8_t size;
  char payload[0];
} command_packet_body_t;

typedef struct __attribute__ (( packed )) {
  command_packet_header_t hdr;
  command_packet_body_t body;
} command_packet_t;

/******************************
 * Class
 ******************************/

class Driver
{
public:
  Driver(const char *path, speed_t speed) noexcept;

  void connect(void);
  void write_command(command_packet_t *cmd);
  void write_encoded_byte(uint8_t b);
  void write_byte(uint8_t b);

  ~Driver();
private:
  const char *m_DevicePath;
  int32_t m_Port;
  bool m_Open, m_Transmitting;
  speed_t m_Speed;
};

#endif
