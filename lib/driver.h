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
  COMMAND_ERROR_NONE,
  COMMAND_ERROR_ENABLE_NOT_ALLOWED,     /* Auto mode enabled, no manual enable allowed */
  COMMAND_ERROR_DISABLE_NOT_ALLOWED,    /* Auto mode enabled, no manual disable allowed */
  COMMAND_ERROR_INVALID_COMMAND
} command_error_t;

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
  unsigned err : 1;           /* If an error has occured */
  unsigned reserved : 7;
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

const char *pktErrorString(command_error_t err);

/******************************
 * Class
 ******************************/

class Driver
{
public:
  Driver(const char *path, speed_t speed) noexcept;

  void connect(void);
  void write_command(command_packet_t *cmd);
  void read_response(void);
  void write_encoded_byte(uint8_t b);
  uint8_t read_enccoded_byte(void);
  void write_byte(uint8_t b);
  uint8_t read_byte(void);

  inline command_packet_t *getPktFromBuffer(void) noexcept
  { return reinterpret_cast<command_packet_t *>(this->m_Buffer); }

  ~Driver();
private:
  const char *m_DevicePath;
  int32_t m_Port;
  bool m_Open, m_Transmitting;
  speed_t m_Speed;
  uint8_t m_Buffer[255];
};

#endif
