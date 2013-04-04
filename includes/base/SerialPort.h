#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <string>

class SerialPort
{
public:
   
   enum PortBaud {
      Baud9600,
      Baud19200,
      Baud57600,
      Baud115200
   };
   
   enum PortMode {
      Mode8N1,
      Mode7E1,
      Mode7O1,
      Mode7S1
   };
   
   struct PortSettings {
      PortBaud    baud;
      PortMode    mode;
      bool        verbose_on;
   };
   
   enum PortTimeout {
      NoWait = -2,
      WaitForever = -1
   };
   
   SerialPort();
   
   ~SerialPort();
   
   bool  openPort(std::string name, PortSettings settings);
   
   void  closePort();
   
   unsigned int   readPort(void* pDst, unsigned int nMaxBytes, int nTimeoutMs = WaitForever);
   
   unsigned int   writePort(const void* pSrc, unsigned int nNumBytes);
   
   bool           getchar(char* c, int nTimeoutMs = WaitForever);
   
   int   getline(char* pLine, int nMaxBytes, int nTimeoutMs = WaitForever);
   int   getline(std::string &line, int nMaxBytes, int nTimeoutMs = WaitForever);
   
private:
   
   static void  Configure(struct SerialPortDesc *pPortDesc,
                          PortSettings settings);
   
private:
   
   struct SerialPortDesc* m_pSpd;
};

#endif // SERIAL_PORT_H