//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2012 Mateusz Pusz
//
// Condor2Nav is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Condor2Nav is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Condor2Nav. If not, see <http://www.gnu.org/licenses/>.
//
// Visit the project webpage (http://sf.net/projects/condor2nav) for more info.
//

/**
 * @file istream.cpp
 *
 * @brief Implements the IStream wrapper class. 
 */

#include "istream.h"
#include <algorithm>
#include <boost/asio/ip/tcp.hpp>
#include "activeSync.h"   // has to be included after boost/asio
#include <boost/filesystem/fstream.hpp>


/**
 * @brief Class constructor.
 *
 * condor2nav::CIStream class constructor.
 *
 * @param fileName The name of the file to read.
 */
condor2nav::CIStream::CIStream(const boost::filesystem::path &fileName)
{
  switch(Type(fileName)) {
  case TType::LOCAL:
    {
      boost::filesystem::fstream stream{fileName, std::ios_base::in};
      if(!stream)
        throw EOperationFailed{"ERROR: Couldn't open file '" + fileName.string() + "' for reading!!!"};
      Buffer() << stream.rdbuf();
    }
    break;

  case TType::ACTIVE_SYNC:
    Buffer().str(CActiveSync::Instance().Read(fileName));
    break;
  }
}


condor2nav::CIStream::CIStream(const std::string &server, const boost::filesystem::path &url, unsigned timeout /* = 30 */)
{
  boost::asio::ip::tcp::iostream http;
  http.expires_from_now(boost::posix_time::seconds(timeout));

  // establish a connection to the server.
  http.connect(server, "http");
  if(!http)
    throw EOperationFailed{"ERROR: Unable to connect to: '" + server + url.generic_string() + "', error: " + http.error().message()};

  // Send the request. We specify the "Connection: close" header so that the
  // server will close the socket after transmitting the response. This will
  // allow us to treat all data up until the EOF as the content.
  http << "GET " << url.generic_string() << " HTTP/1.0\r\n";
  http << "Host: " << server << "\r\n";
  http << "Accept: */*\r\n";
  http << "Connection: close\r\n\r\n";

  // Check that response is OK.
  std::string http_version;
  http >> http_version;
  unsigned int status_code;
  http >> status_code;
  std::string status_message;
  std::getline(http, status_message);
  if(!http || http_version.substr(0, 5) != "HTTP/")
    throw EOperationFailed{"ERROR: Invalid response from: '" + server + url.generic_string() + "'"};
  if(status_code != 200)
    throw EOperationFailed{"ERROR: '" + server + url.generic_string() + "' returned a response with status code: " + Convert(status_code)};

  // Process the response headers, which are terminated by a blank line.
  std::string header;
  while(std::getline(http, header) && header != "\r")
    ;

  // Write the remaining data to internal buffer
  Buffer() << http.rdbuf();

  if(http.error() == boost::asio::error::operation_aborted)
    throw EOperationFailed{"ERROR: Download timeout (" + Convert(timeout) + " seconds) exceeded!"};
}
