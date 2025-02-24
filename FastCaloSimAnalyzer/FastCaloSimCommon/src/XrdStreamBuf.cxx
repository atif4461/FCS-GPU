/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "XrdStreamBuf.h"

/**
 * Constructs an XrdStreamBuf object with the specified file URL and buffer size.
 * @param fileUrl The URL of the file to be opened.
 * @param bufferSize The size of the internal buffer in bytes.
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
 * Destructor for XrdStreamBuf class 
 * Releases system resources associated with the object
 * Closes the file and prints error message if close operation fails
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
 * Checks if the buffer is underflow and reads more data from the file if necessary.
 *
 * @return The next character in the buffer or eof if the end of the file is reached.
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
