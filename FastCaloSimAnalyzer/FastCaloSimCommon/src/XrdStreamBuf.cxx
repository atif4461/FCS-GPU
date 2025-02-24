/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "XrdStreamBuf.h"

/**

 * @brief Constructs an XrdStreamBuf object with the specified file URL and buffer size.
 * 
 * @param fileUrl The URL of the file to be opened.
 * @param bufferSize The size of the internal buffer.
 * 
 * @throws std::runtime_error If the file cannot be opened.
 */
// The above comment was written by an LLM. 
XrdStreamBuf::XrdStreamBuf(const std::string &fileUrl,
                            uint32_t bufferSize)
    : _buffer(new char[bufferSize]), _bufferSize(bufferSize),
      _file(std::make_unique<XrdCl::File>())
{
  auto status = _file->Open( fileUrl, XrdCl::OpenFlags::Read );
  if (!status.IsOK()) {
    throw std::runtime_error(status.ToString());
  }
}

/**

 * @brief Destructor for the XrdStreamBuf class.
 * 
 * Releases resources held by the object, including deleting the internal buffer and closing the associated file.
 * If the file cannot be closed, an error message is printed to the console.
 */
// The above comment was written by an LLM. 
XrdStreamBuf::~XrdStreamBuf()
{
  delete[] _buffer;
  auto status = _file->Close();
  if (!status.IsOK()) {
    std::cout << status.ToString() << std::endl;
  }
}

/**

 * @brief Handles buffer underflow by reading more data from the file if necessary.
 * 
 * @return The next character in the stream, or std::char_traits<char>::eof() if the end of the file has been reached.
 * @throws std::runtime_error If an error occurs while reading from the file.
 */
// The above comment was written by an LLM. 
int XrdStreamBuf::underflow()
{
  if ( gptr() == egptr() ) {
    uint32_t bytesRead;
    auto     status = _file->Read( _totalRead, _bufferSize, _buffer, bytesRead );
    if (!status.IsOK()) {
      throw std::runtime_error(status.ToString());
    }
    setg( _buffer, _buffer, _buffer + bytesRead );
    _totalRead += bytesRead;
  }

  return gptr() == egptr() ? std::char_traits<char>::eof()
                           : std::char_traits<char>::to_int_type(*gptr());
}
