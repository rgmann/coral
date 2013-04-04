#ifndef DSV_FILE_READER_H
#define DSV_FILE_READER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

class DsvFileReader
{
public:
   
   DsvFileReader();
   
   bool open(const char* pFullFileName, 
             char delimeter = ',', 
             bool bHasHeader = false);
   
   void close();
   
   enum MissingPolicy {
      FilterMissing,
      ReplaceMissing
   };
   //void setReplacementPolicy(MissingPolicy policy, );
   
   void  setColumnHeaders(const std::vector<std::string> &headers);
   bool  getColumnHeaders(std::vector<std::string> &headers);
   
   bool  readRow(std::vector<std::string> &row);
   bool  readRow(std::map<std::string, std::string> &row);
   
   bool  isOpen() const;
   bool  foundHeader() const;
   
private:
   
   //std::vector<std::string> &split(const std::string &s, 
//                                   char delim, 
//                                   std::vector<std::string> &elems);
//   
//   std::vector<std::string> split(const std::string &s, char delim);
   
   struct ColHdrIdx
   {
      std::string name;
      unsigned int col;
   };
   static bool columnHdrComparator(ColHdrIdx a, ColHdrIdx b);
   
private:
   
   std::ifstream m_File;
   
   std::vector<std::string> m_Columns;
   
   std::map<std::string, unsigned int> m_ColIndexMap;
   
   char m_DelimeterChar;
};

#endif // DSV_FILE_READER_H