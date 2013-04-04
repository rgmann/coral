#include "DsvFileWriter.h"
#include "StringHelper.h"
#include <algorithm>
//#include <sstream>

//------------------------------------------------------------------------------
DsvFileWriter::DsvFileWriter()
{
}

//------------------------------------------------------------------------------
bool DsvFileWriter::open(const char* pFullFileName, 
                         char delimeter)
{
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
   
   return true;
}

//------------------------------------------------------------------------------
bool DsvFileWriter::open(const char* pFullFileName,
                         const std::vector<std::string> &header,
                         char delimeter)
{
   if (!open(pFullFileName, delimeter))
   {
      return false;
   }
   
   // If the file has a header, read it.
   if (!header.empty())
   {
      if (!writeRow(header))
      {
         close();
         return false;
      }
      
      m_ColHdrs = header;
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool DsvFileWriter::open(const char* pFullFileName,
                         const std::string &header,
                         char delimeter)
{
   return open(pFullFileName, StringHelper::Split(header, delimeter));
}

//------------------------------------------------------------------------------
void DsvFileWriter::close()
{
   if (isOpen())
   {
      m_File.close();
   }
}

//------------------------------------------------------------------------------
bool DsvFileWriter::isOpen() const
{
   return m_File.is_open();
}

//------------------------------------------------------------------------------
bool DsvFileWriter::getColumnHeaders(std::vector<std::string> &headers)
{
   headers = m_ColHdrs;
   return (m_ColHdrs.empty() == false);
}

//------------------------------------------------------------------------------
bool DsvFileWriter::writeRow(const std::vector<std::string> &row, 
                             bool bOmitWrongWidth)
{
   std::string l_sLineString;
   std::vector<std::string>::const_iterator it;
   
   if (!isOpen())
   {
      return false;
   }
   
   // If a header was specified, verify that the number of elements in the row
   // is equal to the number of columns.
   if (bOmitWrongWidth && 
       (m_ColHdrs.size() > 0) && 
       (row.size() != m_ColHdrs.size()))
   {
      return false;
   }
   
   // Create the delimited string from the vector
   it = row.begin();
   for (; it < row.end(); ++it)
   {
      std::vector<std::string>::const_iterator peek_it = it;
      
      l_sLineString += *it;
      
      // If this is not the last row element, then we need to add the delimeter.
      if (++peek_it != row.end())
      {
         l_sLineString.push_back(m_DelimeterChar);
      }
   }
   
   // Add the newline character
   l_sLineString.push_back('\n');
   
   m_File.write(l_sLineString.c_str(), l_sLineString.size());
   
   return (m_File.fail() == false);
}

//------------------------------------------------------------------------------
bool DsvFileWriter::writeRow(const std::string &row, 
                             bool bOmitWrongWidth)
{
   std::vector<std::string> l_vRow = StringHelper::Split(row, m_DelimeterChar);
   
   return writeRow(l_vRow, bOmitWrongWidth);
}

//------------------------------------------------------------------------------
//std::vector<std::string> &DsvFileWriter::split(const std::string &s, 
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
//std::vector<std::string> DsvFileWriter::split(const std::string &s, char delim)
//{
//   std::vector<std::string> elems;
//   return split(s, delim, elems);
//}

