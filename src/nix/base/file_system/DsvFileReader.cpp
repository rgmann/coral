#include "DsvFileReader.h"
#include "StringHelper.h"
#include <algorithm>
//#include <sstream>

//------------------------------------------------------------------------------
DsvFileReader::DsvFileReader()
{
}

//------------------------------------------------------------------------------
bool DsvFileReader::open(const char* pFullFileName, 
                         char delimeter, 
                         bool bHasHeader)
{
   std::vector<std::string> header;
   
   if (isOpen())
   {
      return false;
   }
   
   m_File.open (pFullFileName, std::ifstream::in);
   if (m_File.fail())
   {
      return false;
   }
   
   // Store the delimeter character.
   m_DelimeterChar = delimeter;
   
   // If the file has a header, read it.
   if (bHasHeader)
   {
      if (!readRow(header))
      {
         this->close();
         return false;
      }
      
      setColumnHeaders(header);
   }
   
   return true;
}

//------------------------------------------------------------------------------
void DsvFileReader::close()
{
   if (isOpen())
   {
      m_File.close();
   }
}

//------------------------------------------------------------------------------
bool DsvFileReader::isOpen() const
{
   return m_File.is_open();
}

//------------------------------------------------------------------------------
bool DsvFileReader::foundHeader() const
{
   return (m_ColIndexMap.empty() == false);
}

//------------------------------------------------------------------------------
void DsvFileReader::setColumnHeaders(const std::vector<std::string> &headers)
{
   std::vector<std::string>::const_iterator it = headers.begin();
   unsigned int locColIndex = 0;
   
   for (; it < headers.end(); ++it)
   {
      if (m_ColIndexMap.count(*it) > 0)
      {
         m_ColIndexMap[*it] = locColIndex++;
      }
   }
}

//------------------------------------------------------------------------------
bool DsvFileReader::getColumnHeaders(std::vector<std::string> &headers)
{
   // Use a list to sort the indices.
   std::vector<ColHdrIdx> sortedHeaders;
   std::map<std::string, unsigned int>::iterator mapIt;
   std::vector<ColHdrIdx>::iterator vecIt;
   
   if (m_ColIndexMap.empty())
   {
      return false;
   }
   
   mapIt = m_ColIndexMap.begin();
   for (; mapIt != m_ColIndexMap.end(); ++mapIt)
   {
      ColHdrIdx headerInd;
      
      headerInd.name = mapIt->first;
      headerInd.col  = mapIt->second;
      
      sortedHeaders.push_back(headerInd);
   }
   
   std::sort(sortedHeaders.begin(), sortedHeaders.end(), columnHdrComparator);
   vecIt = sortedHeaders.begin();
   for (; vecIt < sortedHeaders.end(); ++vecIt)
   {
      headers.push_back(vecIt->name);
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool DsvFileReader::columnHdrComparator(ColHdrIdx a, ColHdrIdx b)
{
   return (a.col < b.col);
}

//------------------------------------------------------------------------------
bool DsvFileReader::readRow(std::vector<std::string> &row)
{
   std::string locLineStr;
   char locTempStr[256];
   
   if (m_File.eof() || m_File.fail())
   {
      return false;
   }
   m_File.getline(locTempStr, 256);
   
   locLineStr = std::string(locTempStr);
   
   row.clear();
   StringHelper::Split(locLineStr, m_DelimeterChar, row);
   
   return true;
}

//------------------------------------------------------------------------------
bool DsvFileReader::readRow(std::map<std::string, std::string> &row)
{
   std::vector<std::string> l_vRow;
   std::map<std::string, unsigned int>::iterator it;
   bool  l_bSuccess = true;
   
   if (!readRow(l_vRow))
   {
      return false;
   }
   
   it = m_ColIndexMap.begin();
   for (; it != m_ColIndexMap.end(); ++it)
   {
      if (it->second >= l_vRow.size())
      {
         l_bSuccess = false;
      }
      else
      {
         row[it->first] = l_vRow.at(it->second);
      }
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
//std::vector<std::string> &DsvFileReader::split(const std::string &s, 
//                                               char delim, 
//                                               std::vector<std::string> &elems) 
//{
//   std::stringstream ss(s);
//   std::string item;
//   
//   while(std::getline(ss, item, delim))
//   {
//      elems.push_back(item);
//   }
//   
//   return elems;
//}

//------------------------------------------------------------------------------
//std::vector<std::string> DsvFileReader::split(const std::string &s, char delim)
//{
//   std::vector<std::string> elems;
//   return split(s, delim, elems);
//}
