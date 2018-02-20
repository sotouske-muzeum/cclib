
#include "cclcurl.h"

#include <sstream>

namespace cclcurl {

size_t cb_write_buffer(void *ptr,size_t size,size_t nmemb,void *stream)
{/*{{{*/
  (void)size;

  auto buff_ptr = static_cast<std::stringbuf *>(stream);
  buff_ptr->sputn(static_cast<const char *>(ptr),nmemb);

  return nmemb;
}/*}}}*/

size_t cb_read_buffer(void *ptr,size_t size,size_t nmemb,void *stream)
{/*{{{*/
  (void)size;

  auto buff_ptr = static_cast<std::stringbuf *>(stream);

  // - retrieve count of available characters -
  auto avail = buff_ptr->in_avail();

  // - if there are not enough bytes in buffer -
  if (static_cast<ssize_t>(nmemb) > avail)
  {
    // - write rest of bytes -
    if (avail > 0)
    {
      buff_ptr->sgetn(static_cast<char *>(ptr),avail);
    }

    return avail;
  }

  // - write requested count of bytes -
  buff_ptr->sgetn(static_cast<char *>(ptr),nmemb);

  return nmemb;
}/*}}}*/

result_c GET(const std::string &a_url)
{/*{{{*/

  // - create curl session -
  CURL *curl_ptr = curl_easy_init();

  // - ERROR -
  if (curl_ptr == nullptr)
  {
    cclthrow(error_CURL_CANNOT_CREATE_SESSION);
  }

  // - create write buffer -
  std::stringbuf write_buffer{std::ios_base::out};

  curl_easy_setopt(curl_ptr,CURLOPT_URL,a_url.data());
  curl_easy_setopt(curl_ptr,CURLOPT_WRITEFUNCTION,cb_write_buffer);
  curl_easy_setopt(curl_ptr,CURLOPT_WRITEDATA,&write_buffer);

  // - ERROR -
  if (curl_easy_perform(curl_ptr) != CURLE_OK)
  {
    curl_easy_cleanup(curl_ptr);

    cclthrow(error_CURL_ERROR_WHILE_PERFORMING_GET_REQUEST);
  }

  return {curl_ptr,write_buffer.str()};
}/*}}}*/

result_c PUT(const std::string &a_url,const std::string &a_data)
{/*{{{*/

  // - create curl session -
  CURL *curl_ptr = curl_easy_init();

  // - ERROR -
  if (curl_ptr == nullptr)
  {
    cclthrow(error_CURL_CANNOT_CREATE_SESSION);
  }

  // - create read buffer -
  std::stringbuf read_buffer(a_data,std::ios_base::in);

  // - create write buffer -
  std::stringbuf write_buffer{std::ios_base::out};

  curl_easy_setopt(curl_ptr,CURLOPT_URL,a_url.data());
  curl_easy_setopt(curl_ptr,CURLOPT_READFUNCTION,cb_read_buffer);
  curl_easy_setopt(curl_ptr,CURLOPT_READDATA,&read_buffer);
  curl_easy_setopt(curl_ptr,CURLOPT_WRITEFUNCTION,cb_write_buffer);
  curl_easy_setopt(curl_ptr,CURLOPT_WRITEDATA,&write_buffer);
  curl_easy_setopt(curl_ptr,CURLOPT_UPLOAD,1L);
  curl_easy_setopt(curl_ptr,CURLOPT_PUT,1L);
  curl_easy_setopt(curl_ptr,CURLOPT_INFILESIZE_LARGE,static_cast<curl_off_t>(a_data.length()));

  // - ERROR -
  if (curl_easy_perform(curl_ptr) != CURLE_OK)
  {
    curl_easy_cleanup(curl_ptr);

    cclthrow(error_CURL_ERROR_WHILE_PERFORMING_PUT_REQUEST);
  }

  return {curl_ptr,write_buffer.str()};
}/*}}}*/

result_c DELETE(const std::string &a_url)
{/*{{{*/

  // - create curl session -
  CURL *curl_ptr = curl_easy_init();

  // - ERROR -
  if (curl_ptr == nullptr)
  {
    cclthrow(error_CURL_CANNOT_CREATE_SESSION);
  }

  // - create write buffer -
  std::stringbuf write_buffer{std::ios_base::out};

  curl_easy_setopt(curl_ptr,CURLOPT_URL,a_url.data());
  curl_easy_setopt(curl_ptr,CURLOPT_WRITEFUNCTION,cb_write_buffer);
  curl_easy_setopt(curl_ptr,CURLOPT_WRITEDATA,&write_buffer);
  curl_easy_setopt(curl_ptr,CURLOPT_CUSTOMREQUEST,"DELETE");

  // - ERROR -
  if (curl_easy_perform(curl_ptr) != CURLE_OK)
  {
    curl_easy_cleanup(curl_ptr);

    cclthrow(error_CURL_ERROR_WHILE_PERFORMING_DELETE_REQUEST);
  }

  return {curl_ptr,write_buffer.str()};
}/*}}}*/

result_c HEAD(const std::string &a_url)
{/*{{{*/

  // - create curl session -
  CURL *curl_ptr = curl_easy_init();

  // - ERROR -
  if (curl_ptr == nullptr)
  {
    cclthrow(error_CURL_CANNOT_CREATE_SESSION);
  }

  // - create write buffer -
  std::stringbuf write_buffer{std::ios_base::out};

  curl_easy_setopt(curl_ptr,CURLOPT_URL,a_url.data());
  curl_easy_setopt(curl_ptr,CURLOPT_WRITEFUNCTION,cb_write_buffer);
  curl_easy_setopt(curl_ptr,CURLOPT_WRITEDATA,&write_buffer);
  curl_easy_setopt(curl_ptr,CURLOPT_HEADER,1);
  curl_easy_setopt(curl_ptr,CURLOPT_NOBODY,1);

  // - ERROR -
  if (curl_easy_perform(curl_ptr) != CURLE_OK)
  {
    curl_easy_cleanup(curl_ptr);

    cclthrow(error_CURL_ERROR_WHILE_PERFORMING_HEAD_REQUEST);
  }

  return {curl_ptr,write_buffer.str()};
}/*}}}*/

result_c::result_c(result_c &&a_result) throw() :
  m_curl_ptr{a_result.m_curl_ptr}, m_data(std::move(a_result.m_data))
{/*{{{*/
  a_result.m_curl_ptr = nullptr;
}/*}}}*/

result_c::~result_c()
{/*{{{*/

  // - release curl_ptr -
  if (m_curl_ptr != nullptr)
  {
    curl_easy_cleanup(m_curl_ptr);
  }
}/*}}}*/

std::string result_c::info_string(CURLINFO a_info)
{/*{{{*/

  // - ERROR -
  if ((a_info & CURLINFO_TYPEMASK) != CURLINFO_STRING)
  {
    cclthrow(error_CURL_RESULT_INVALID_INFO_VALUE_TYPE);
  }

  // - ERROR -
  char *data;
  if (curl_easy_getinfo(m_curl_ptr,a_info,&data) != CURLE_OK)
  {
    cclthrow(error_CURL_RESULT_ERROR_WHILE_GET_INFO);
  }

  return data;
}/*}}}*/

int64_t result_c::info_long(CURLINFO a_info)
{/*{{{*/

  // - ERROR -
  if ((a_info & CURLINFO_TYPEMASK) != CURLINFO_LONG)
  {
    cclthrow(error_CURL_RESULT_INVALID_INFO_VALUE_TYPE);
  }

  long data; // NOLINT: ignore [google-runtime-int]
  if (curl_easy_getinfo(m_curl_ptr,a_info,&data) != CURLE_OK)
  {
    cclthrow(error_CURL_RESULT_ERROR_WHILE_GET_INFO);
  }

  return data;
}/*}}}*/

double result_c::info_double(CURLINFO a_info)
{/*{{{*/

  // - ERROR -
  if ((a_info & CURLINFO_TYPEMASK) != CURLINFO_DOUBLE)
  {
    cclthrow(error_CURL_RESULT_INVALID_INFO_VALUE_TYPE);
  }

  double data;
  if (curl_easy_getinfo(m_curl_ptr,a_info,&data) != CURLE_OK)
  {
    cclthrow(error_CURL_RESULT_ERROR_WHILE_GET_INFO);
  }

  return data;
}/*}}}*/

cclcurl_c::cclcurl_c() throw()
{/*{{{*/
  curl_global_init(CURL_GLOBAL_ALL);
}/*}}}*/

cclcurl_c::~cclcurl_c()
{/*{{{*/
  curl_global_cleanup();
}/*}}}*/

// - curl global init object -
cclcurl_c g_cclcurl;

} // namespace cclcurl

