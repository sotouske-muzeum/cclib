
#ifndef CCLIB_CCLCURL_H
#define CCLIB_CCLCURL_H

#include "cclbase.h"

#include <string>

#include <curl/curl.h>

namespace cclcurl {

class result_c;

// - exceptions list -
const std::string error_CURL_CANNOT_CREATE_SESSION = "cannot create curl session";
const std::string error_CURL_ERROR_WHILE_PERFORMING_GET_REQUEST = "error while performing http get request";
const std::string error_CURL_ERROR_WHILE_PERFORMING_PUT_REQUEST = "error while performing http put request";
const std::string error_CURL_ERROR_WHILE_PERFORMING_DELETE_REQUEST = "error while performing http delete request";
const std::string error_CURL_ERROR_WHILE_PERFORMING_HEAD_REQUEST = "error while performing http head request";
const std::string error_CURL_RESULT_INVALID_INFO_VALUE_TYPE = "invalid type of curl result information value";
const std::string error_CURL_RESULT_ERROR_WHILE_GET_INFO = "error while reading curl result information value";

size_t cb_write_buffer(void *ptr,size_t size,size_t nmemb,void *stream);
size_t cb_read_buffer(void *ptr,size_t size,size_t nmemb,void *stream);

result_c GET(const std::string &a_url);
result_c PUT(const std::string &a_url,const std::string &a_data);
result_c DELETE(const std::string &a_url);
result_c HEAD(const std::string &a_url);

class result_c
{/*{{{*/
  friend result_c GET(const std::string &a_url); 
  friend result_c PUT(const std::string &a_url,const std::string &a_data);
  friend result_c DELETE(const std::string &a_url);
  friend result_c HEAD(const std::string &a_url);

  private:
  CURL *m_curl_ptr;
  std::string m_data;

  result_c(CURL *a_curl_ptr,std::string a_data) :
    m_curl_ptr{a_curl_ptr}, m_data(std::move(a_data)) {}
  public:
  result_c(const result_c &) = delete;
  void operator = (const result_c &) = delete;
  result_c(result_c &&a_result) throw();
  ~result_c();

  std::string &data() { return m_data; }
  std::string info_string(CURLINFO a_info);
  int64_t info_long(CURLINFO a_info);
  double info_double(CURLINFO a_info);
};/*}}}*/

class cclcurl_c
{/*{{{*/
  public:
  cclcurl_c() throw();
  ~cclcurl_c();
};/*}}}*/

} // namespace cclcurl

#endif

