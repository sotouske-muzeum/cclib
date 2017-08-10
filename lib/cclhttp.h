
#ifndef CCLIB_CCLHTTP_H
#define CCLIB_CCLHTTP_H

#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <fcntl.h>
#include <microhttpd.h>
#include <poll.h>
#include <sys/stat.h>

namespace cclhttp {

class connection_c;
class post_proc_c;
class response_c;

// - exceptions list -
const std::string error_HTTP_SERVER_CANNOT_START_DAEMON = "cannot start http server daemon";
const std::string error_HTTP_SERVER_INTERNAL_ERROR = "internal error of http server";
const std::string error_HTTP_RESP_CANNOT_READ_SOURCE_FILE = "cannot read http response file";
const std::string error_HTTP_RESP_UNKNOWN_DATA_SOURCE_IDENTIFIER = "unknown identifier of http response data source";
const std::string error_HTTP_RESP_CREATE_ERROR = "error while creating http response";
const std::string error_HTTP_CONN_CANNOT_QUEUE_RESPONSE = "cannot queue response to http connection";
const std::string error_POST_PROC_FILE_OPEN_ERROR = "post processor cannot open file";
const std::string error_POST_PROC_FILE_WRITE_ERROR = "error while writing to post processor file";
const std::string error_POST_PROC_FILE_POST_PROCESSOR_CREATE_ERROR = "error while creating post processor";
const std::string error_HTTP_RESP_CANNOT_ADD_HEADER_FOOTER = "cannot add header or footer to response";

// - connection types -
enum class Method
{/*{{{*/
  NONE = 0,
  CONNECT,
  DELETE,
  GET,
  HEAD,
  OPTIONS,
  POST,
  PUT,
  TRACE,
};/*}}}*/

class server_c
{/*{{{*/
  typedef void (*callback_t)(connection_c &);

  private:
  MHD_Daemon *m_daemon_ptr;
  callback_t m_connection_cb;
  callback_t m_completed_cb;
  std::exception_ptr m_exc_ptr;
 
  static int connection_func(void *cls,struct MHD_Connection *connection,
      const char *url,const char *method,const char *version,
      const char *upload_data,size_t *upload_data_size,void **con_cls);

  static void completed_func(void *cls,struct MHD_Connection *connection,
      void **con_cls,enum MHD_RequestTerminationCode toe);

  public:
  server_c(const server_c &) = delete;
  void operator = (const server_c &) = delete;

  server_c(uint16_t a_port,callback_t a_connection_cb,callback_t a_completed_cb);
  ~server_c();

  server_c &retrieve_poll_fds(std::vector<pollfd> &a_poll_fds);
  server_c &process();
};/*}}}*/

class connection_c
{/*{{{*/
  friend class server_c;

  public:
  typedef std::unordered_map<std::string,std::string> values_map_t;

  private:
  server_c *m_srv_ptr;

  Method m_conn_type;
  MHD_Connection *m_conn_ptr;
  values_map_t m_key_value_map;
  void *m_user_data;

  const char *m_url;
  const char *m_method;
  const char *m_version;
  const char *m_upload_data;
  size_t *m_upload_data_size;

  static int key_value_func(void *cls,enum MHD_ValueKind kind,
      const char *key,const char *value);

  public:
  connection_c() : m_srv_ptr{nullptr}, m_conn_type{Method::NONE},
  m_conn_ptr{nullptr}, m_user_data{nullptr}, m_url{nullptr}, m_method{nullptr},
  m_version{nullptr}, m_upload_data{nullptr}, m_upload_data_size{nullptr} {}
  connection_c(const connection_c &) = delete;

  Method type() const { return m_conn_type; }
  std::string url() const { return m_url; }
  std::string method() const { return m_method; }
  std::string version() const { return m_version; }
  std::string upload_data();
  void *&user_data() { return m_user_data; }
  connection_c &values(enum MHD_ValueKind a_kind,values_map_t &a_values_map);
  values_map_t values(enum MHD_ValueKind a_kind);
  connection_c &queue_response(unsigned a_status,const response_c &a_resp);

  connection_c &queue_basic_auth_fail_response(
      const std::string &a_realm, 
      const response_c &a_resp);

  connection_c &basic_auth_username_password(
      std::string &a_user,
      std::string &a_pass);

  connection_c &queue_auth_fail_response(
      const std::string &a_realm,
      const std::string &a_opaque,
      const response_c &a_resp,
      int a_signal_stale);

  std::string auth_username();
  bool auth_check(
      const std::string &a_realm,
      const std::string &a_user,
      const std::string &a_pass,
      unsigned a_nonce_timeout,
      int &a_reason);

  post_proc_c *new_post_proc(const std::string &a_file_name);
};/*}}}*/

class post_proc_c
{/*{{{*/
  friend class connection_c;

  private:
  MHD_PostProcessor *m_post_proc;
  size_t m_total_size;
  size_t m_upload_size;
  FILE *m_file;

  post_proc_c(const post_proc_c &) = delete;
  post_proc_c(MHD_Connection *a_conn_ptr,const std::string &a_file_name);
  void operator = (const post_proc_c &) = delete;
  int post_callback(const char *key,const char *data,size_t size);

  public:
  ~post_proc_c();
  static int cb_post_callback(void *coninfo_cls,enum MHD_ValueKind kind,
      const char *key,const char *filename,const char *content_type,
      const char *transfer_encoding,const char *data,uint64_t off,size_t size)
  {/*{{{*/
    (void)coninfo_cls;
    (void)kind;
    (void)filename;
    (void)content_type;
    (void)transfer_encoding;
    (void)off;

    return static_cast<post_proc_c *>(coninfo_cls)->post_callback(key,data,size);
  }/*}}}*/
  void process(const std::string &a_data);
  inline bool is_dummy() { return m_post_proc == nullptr; }
  double progress();
};/*}}}*/

// - response sources -
enum
{/*{{{*/
  c_resp_from_string = 0,
  c_resp_from_file,
};/*}}}*/

class response_c
{/*{{{*/
  friend class connection_c;

  private:
  MHD_Response *m_resp_ptr;
  public:
  response_c(const response_c &) = delete;
  response_c(const std::string &a_source,int a_type);
  ~response_c();

  response_c &add_header(const std::string &a_header,const std::string &a_value);
  response_c &add_footer(const std::string &a_footer,const std::string &a_value);
};/*}}}*/

} // namespace cclhttp

#endif

