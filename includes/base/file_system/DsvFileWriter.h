#ifndef DSV_FILE_WRITER_H
#define DSV_FILE_WRITER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

class DsvFileWriter
{
public:
   
   DsvFileWriter();
   
   bool open(const char* pFullFileName, 
             char delimeter = ',');
   
   bool open(const char* pFullFileName,
             const std::vector<std::string> &header,
             char delimeter = ',');
   
   bool open(const char* pFullFileName,
             const std::string &header,
             char delimeter = ',');
   
   void close();
   
   bool  getColumnHeaders(std::vector<std::string> &headers);
   
   bool  writeRow(const std::vector<std::string> &row, bool bOmitWrongWidth = true);
   bool  writeRow(const std::string &row, bool bOmitWrongWidth = true);
   
   bool  isOpen() const;
   
private:
   
   //std::vector<std::string> &split(const std::string &s, 
//                                   char delim, 
//                                   std::vector<std::string> &elems);
//   
//   std::vector<std::string> split(const std::string &s, char delim);
   
private:
   
   std::ofstream m_File;
   
   std::vector<std::string> m_ColHdrs;
   
   char m_DelimeterChar;
};

#endif // DSV_FILE_WRITER_H