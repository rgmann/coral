#include "SerialPort.h"
#include "Timestamp.h"
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/types.h>
#include <sys/stat.h>

// Specify the SerialPortDesc for Nix platforms
struct SerialPortDesc
{
   int   fd;
};

//------------------------------------------------------------------------------
SerialPort::SerialPort()
{
   // Instantiate a serial port file descriptor
   m_pSpd = NULL;
   m_pSpd = new SerialPortDesc();
   
   if (m_pSpd)
   {
      // Initialize the serial port file descriptor to -1
      m_pSpd->fd = -1;
   }
}

//------------------------------------------------------------------------------
SerialPort::~SerialPort()
{
   // Close port if it's open
   closePort();
   
   // If it exists, destroy the serial port file descriptor.
   if (m_pSpd)
   {
      delete m_pSpd;
      m_pSpd = NULL;
   }
}

//------------------------------------------------------------------------------
bool SerialPort::openPort(std::string name, PortSettings settings)
{
   if (m_pSpd == NULL)
   {
      return false;
   }
   
   m_pSpd->fd = open(name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
   if (m_pSpd->fd == -1)
   {
      /*
       * Could not open the port.
       */
      
      perror("open_port: Unable to open /dev/ttyS0 - ");
      return false;
   }
   
   fcntl(m_pSpd->fd, F_SETFL, 0);
   
   Configure(m_pSpd, settings);
   
   return true;
}

//------------------------------------------------------------------------------
void SerialPort::closePort()
{
   if (m_pSpd == NULL)
   {
      return;
   }
   
   if (m_pSpd->fd == -1)
   {
      return;
   }
   
   close(m_pSpd->fd);
   m_pSpd->fd = -1;
}

//------------------------------------------------------------------------------
unsigned int SerialPort::readPort(void* pDst, unsigned int nMaxBytes, int nTimeoutMs)
{
   unsigned int l_nBytesRead = 0;
   
   if (m_pSpd == NULL)
   {
      return 0;
   }
   
   if (m_pSpd->fd == -1)
   {
      return 0;
   }
   
   if (nTimeoutMs == NoWait)
   {
      // Don't wait; just read however many bytes are available.
      fcntl(m_pSpd->fd, F_SETFL, FNDELAY);
      l_nBytesRead = read(m_pSpd->fd, (void*)pDst, nMaxBytes);
      fcntl(m_pSpd->fd, F_SETFL, 0);
   }
   if (nTimeoutMs == WaitForever)
   {
      // Block until the specified number of bytes are available.
      // This will block forever.
      l_nBytesRead = read(m_pSpd->fd, pDst, nMaxBytes);
   }
   else
   {
      int      l_nSlctRet = 0;
      fd_set   l_ReadFdSet;
      struct timeval l_ReadTo;
      
      // If the caller specifed some negative value,
      // pretend like they specified a timeout of zero milliseconds.
      nTimeoutMs = (nTimeoutMs < 0) ? 0 : nTimeoutMs;
      
      // Add the file
      FD_ZERO(&l_ReadFdSet);
      FD_SET(m_pSpd->fd, &l_ReadFdSet);
      
      // Setup the read timeout
      l_ReadTo.tv_sec  = nTimeoutMs / 1000;
      l_ReadTo.tv_usec = (nTimeoutMs - l_ReadTo.tv_sec * 1000) * 1000;
      
      // Wait for data to be available for reading.
      l_nSlctRet = select(m_pSpd->fd + 1, &l_ReadFdSet, NULL, NULL, &l_ReadTo);
      if (l_nSlctRet != 1)
      {
         fcntl(m_pSpd->fd, F_SETFL, FNDELAY);
         l_nBytesRead = read(m_pSpd->fd, pDst, nMaxBytes);
         fcntl(m_pSpd->fd, F_SETFL, 0);
      }
      else
      {
         l_nBytesRead = 0;
      }

   }
   
   return l_nBytesRead;
}

//------------------------------------------------------------------------------
unsigned int SerialPort::writePort(const void* pSrc, unsigned int nNumBytes)
{
   if (m_pSpd == NULL)
   {
      return 0;
   }
   
   if (m_pSpd->fd == -1)
   {
      return 0;
   }
   
   return write(m_pSpd->fd, pSrc, nNumBytes);
}

//------------------------------------------------------------------------------
bool SerialPort::getchar(char *pChar, int nTimeoutMs)
{
   return (readPort(pChar, sizeof(*pChar), nTimeoutMs) == sizeof(*pChar));
}

//------------------------------------------------------------------------------
int SerialPort::getline(char *pChar, int nMaxBytes, int nTimeoutMs)
{
   char  l_cCurrentChar;
   int   l_nBytesRead = 0;
   
   int       l_nElapsedMs = 0;
   Timestamp l_Start, l_Stop;
   
   if (m_pSpd == NULL)
   {
      return -1;
   }
   
   if (m_pSpd->fd == -1)
   {
      return -1;
   }
   
   // Loop conditions:
   // 1. Elapsed time is <= the timeout specified by the caller
   // 2. Number of bytes read is <= the max specified by the caller
   // 3. The newline character was found. 
   //    (Note: the returned string does not contains the newline character)
   while ((l_nElapsedMs < nTimeoutMs) && (l_nBytesRead < nMaxBytes - 1))
   {
      // Start the iteration timer
      l_Start.sample();
      
      // Try to get a character.
      if (!getchar(&l_cCurrentChar, nTimeoutMs))
      {
         // If we fail, we need to return because we've
         // used up the entire timeout.
         break;
      }
      else
      {
         // If we got a newline character we're done.
         if (l_cCurrentChar == '\n')
         {
            break;
         }
         
         // Add the character at the current position in the string.
         pChar[l_nBytesRead++] = l_cCurrentChar;
      }
      
      // Start the iteration timer and compute the elapsed
      // time for the iteration.
      l_Stop.sample();
      l_nElapsedMs += l_Stop.diffInMs(l_Start);
   }
   
   // Last character is the NULL character
   pChar[l_nBytesRead] = '\0';
   
   // Return the number of bytes that were written into the destination buffer.
   return l_nBytesRead;
}

//------------------------------------------------------------------------------
int SerialPort::getline(std::string &line, int nMaxBytes, int nTimeoutMs)
{
   bool  l_nBytesRead   = 0;
   char* l_pLine        = NULL;
   
   l_pLine = new char[nMaxBytes];
   if (l_pLine == NULL)
   {
      return 0;
   }
   
   l_nBytesRead = getline(l_pLine, nMaxBytes, nTimeoutMs);
   
   line = "";
   if (l_nBytesRead > 0)
   {
      line = l_pLine;
   }
   
   if (l_pLine)
   {
      delete[] l_pLine;
      l_pLine = NULL;
   }

   return l_nBytesRead;
}

//------------------------------------------------------------------------------
void SerialPort::Configure(struct SerialPortDesc *pPortDesc, 
                           PortSettings settings)
{
   struct termios options;
   speed_t l_sBaud;
   
   /*
    * Get the current options for the port...
    */
   tcgetattr(pPortDesc->fd, &options);
   
   /*
    * Set the baud rates to 19200...
    */
   switch (settings.baud)
   {
      case Baud9600:  l_sBaud = B9600;   break;
      case Baud19200: l_sBaud = B19200;  break;
      case Baud57600: l_sBaud = B57600;  break;
      case Baud115200:l_sBaud = B115200; break;
      default: l_sBaud = B9600;break;
   }
   cfsetispeed(&options, l_sBaud);
   cfsetospeed(&options, l_sBaud);
   
   /*
    * Enable the receiver and set local mode...
    */
   options.c_cflag |= (CLOCAL | CREAD);
   
   /**
    * Set the character size.
    */
   options.c_cflag &= ~CSIZE; /* Mask the character size bits */
   options.c_cflag |= CS8;    /* Select 8 data bits */
   
   /**
    * Configure the port parity.
    */
   if (settings.mode == Mode8N1)
   {
      options.c_cflag &= ~PARENB;
      options.c_cflag &= ~CSTOPB;
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS8;
   }
   else if (settings.mode == Mode7E1)
   {
      options.c_cflag |= PARENB;
      options.c_cflag &= ~PARODD;
      options.c_cflag &= ~CSTOPB;
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS7;
   }
   else if (settings.mode == Mode7O1)
   {
      options.c_cflag |= PARENB;
      options.c_cflag |= PARODD;
      options.c_cflag &= ~CSTOPB;
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS7;
   }
   else if (settings.mode == Mode7S1)
   {
      options.c_cflag &= ~PARENB;
      options.c_cflag &= ~CSTOPB;
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS8;
   }
   
   /*
    * Set the new options for the port...
    */
   tcsetattr(pPortDesc->fd, TCSANOW, &options);
}

