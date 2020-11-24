#include "driver.h"

Driver::Driver(const char *path, speed_t speed) noexcept:
  m_DevicePath(path), m_Speed(speed), m_Transmitting(false)
{}

void Driver::connect(void)
{
  // Creates an socket to the specified device path, if this fails throw
  //  an exception, else set open to true, so that the destructor know we 
  //  should close the socket
  this->m_Port = open(this->m_DevicePath, O_RDWR);
  if (this->m_Port < 0)
    throw std::runtime_error(std::string("open() failed: ") + strerror(errno));
  this->m_Open = true;

  // Read existing settings from the serial port, and throws an exception
  //  if it fails
  struct termios tty;
  if (tcgetattr(this->m_Port, &tty) != 0)
    throw std::runtime_error(std::string("tcgetattr() failed: ") + strerror(errno));


  tty.c_cflag &= ~PARENB;         /* No Parity */
  tty.c_cflag &= ~CSTOPB;         /* One Stop Bit */
  
  tty.c_cflag &= ~CSIZE;          /* Clear previous bit-size bits */
  tty.c_cflag |= CS8;             /* 8-Bit Bytes */

  tty.c_cflag &= ~CRTSCTS;        /* No RTS/CTS Hardware control flow */

  tty.c_cflag |= CREAD;           /* Turn on read */
  tty.c_cflag |= CLOCAL;          /* No modem specific signal lines */

  tty.c_lflag &= ~ICANON;         /* Disable canonical mode */
  tty.c_lflag &= ~ECHO;           /* Disable Echo */
  tty.c_lflag &= ~ECHOE;          /* Disable Erasure */
  tty.c_lflag &= ~ECHONL;         /* Disable newline echo */
  tty.c_lflag &= ~ISIG;           /* Disable interpretation of INTR, QUIT and SUSP */

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable flow control */
  tty.c_iflag &= ~(
    IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL
  ); /* No Special byte handling */

  tty.c_oflag &= ~OPOST;          /* Prevent special interpretation of output bytes */
  tty.c_oflag &= ~ONLCR;          /* Prevent newline conversion */

  tty.c_cc[VTIME] = 0;            /* Block until bytes received */
  tty.c_cc[VMIN] = 1;             /* One byte per read */

  cfsetispeed(&tty, this->m_Speed);   /* Set input speed */
  cfsetospeed(&tty, this->m_Speed);   /* Set output speed */

  // Saves the new configuration, and throws an error if anything
  //  went wrong
  if (tcsetattr(this->m_Port, TCSANOW, &tty) != 0)
    throw std::runtime_error(std::string("tcsetattr() failed: ") + strerror(errno));
}

void Driver::write_command(command_packet_t *cmd)
{
  uint8_t i;

  // Waits until ready for transmission, and starts transmitting
  while (m_Transmitting);
  m_Transmitting = true;

  // Sends the start condition
  this->write_byte(COMMANDS_USART_FLAG);

  // Starts writing the static parts of the command
  for (i = 0; i < sizeof (command_packet_t); ++i)
    write_encoded_byte(reinterpret_cast<uint8_t *>(cmd)[i]);

  // Starts writing the payload of the packet
  for (i = 0; i < cmd->body.size; ++i)
    write_encoded_byte(cmd->body.payload[i]);

  // Sends the stop condition
  this->write_byte(COMMANDS_USART_FLAG);

  // Sets transmitting to false
  m_Transmitting = false;
}

void Driver::write_encoded_byte(uint8_t b)
{
  // Writes the lower four bits
  uint8_t t = b & 0x0F;
  this->write_byte(t);

  // Writes the higher four bits
  t = (b & 0xF0) >> 4;
  this->write_byte(t);
}

void Driver::write_byte(uint8_t b)
{
  if (write(this->m_Port, &b, 1) < 0)
    throw std::runtime_error(std::string("write() failed: ") + strerror(errno));
}

Driver::~Driver()
{
  if (this->m_Open) close(this->m_Port);
}