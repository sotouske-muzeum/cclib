
#include "cclsys.h"
#include "cclhttp.h"

namespace cclhttp {

server_c::server_c(uint16_t a_port,callback_t a_connection_cb,callback_t a_completed_cb) :
  m_daemon_ptr{nullptr}, m_connection_cb{a_connection_cb}, m_completed_cb{a_completed_cb}
{/*{{{*/
  m_exc_ptr = nullptr;

  m_daemon_ptr = MHD_start_daemon(
      MHD_NO_FLAG,a_port,nullptr,nullptr,
      connection_func,this,
      MHD_OPTION_NOTIFY_COMPLETED,completed_func,this,
      MHD_OPTION_END);

  // - ERROR -
  if (m_daemon_ptr == nullptr)
  {
    cclthrow(error_HTTP_SERVER_CANNOT_START_DAEMON);
  }
}/*}}}*/

server_c::~server_c()
{/*{{{*/
  MHD_stop_daemon(m_daemon_ptr);
}/*}}}*/

int server_c::connection_func(void *cls,struct MHD_Connection *connection,
    const char *url,const char *method,const char *version,
    const char *upload_data,size_t *upload_data_size,void **con_cls)
{/*{{{*/
  server_c *srv_ptr = static_cast<server_c *>(cls);

  // - connection pointer -
  connection_c *conn_ptr;

  if (*con_cls == nullptr)
  {
    // - create http_connection object -
    conn_ptr = new connection_c;

    // - set pointer to server -
    conn_ptr->m_srv_ptr = srv_ptr;

    // - retrieve connection type -
    Method conn_type;
    switch (strlen(method))
    {
    // - GET, PUT -
    case 3:
      if (memcmp("GET",method,3) == 0)
      {
        conn_type = Method::GET;
      }
      else
      {
        conn_type = Method::PUT;
      }
      break;

    // - HEAD, POST -
    case 4:
      if (memcmp("HEAD",method,4) == 0)
      {
        conn_type = Method::HEAD;
      }
      else
      {
        conn_type = Method::POST;
      }
      break;

    // - TRACE -
    case 5:
      conn_type = Method::TRACE;
      break;

    // - DELETE -
    case 6:
      conn_type = Method::DELETE;
      break;

    // - CONNECT, OPTIONS -
    case 7:
      if (memcmp("CONNECT",method,7) == 0)
      {
        conn_type = Method::CONNECT;
      }
      else
      {
        conn_type = Method::OPTIONS;
      }
      break;

    // - unrecognized connection type -
    default:
      conn_type = Method::NONE;
    }

    // - set connection properties -
    conn_ptr->m_conn_type = conn_type;
    conn_ptr->m_conn_ptr = connection;

    // - set user data to null -
    conn_ptr->m_user_data = nullptr;

    conn_ptr->m_url = url;
    conn_ptr->m_method = method;
    conn_ptr->m_version = version;

    // - set connection pointer -
    *con_cls = conn_ptr;

    return MHD_YES;
  }

  // - retrieve connection object -
  conn_ptr = static_cast<connection_c *>(*con_cls);

  // - update upload data variables -
  conn_ptr->m_upload_data = upload_data;
  conn_ptr->m_upload_data_size = upload_data_size;

  // - call callback - 
  try {
    srv_ptr->m_connection_cb(*conn_ptr);
  }
  catch (...)
  {
    // - store current exception -
    srv_ptr->m_exc_ptr = std::current_exception();

    return MHD_NO;
  }

  return MHD_YES;
}/*}}}*/

void server_c::completed_func(void *cls,struct MHD_Connection *connection,
    void **con_cls,enum MHD_RequestTerminationCode toe)
{/*{{{*/
  (void)connection;(void)toe;

  server_c *srv_ptr = static_cast<server_c *>(cls);

  // - retrieve connection object -
  connection_c *conn_ptr = static_cast<connection_c *>(*con_cls);

  // - call callback - 
  srv_ptr->m_completed_cb(*conn_ptr);

  // - if connection object exists -
  if (conn_ptr != nullptr)
  {
    delete conn_ptr;
  }
}/*}}}*/

server_c &server_c::retrieve_poll_fds(std::vector<pollfd> &a_poll_fds)
{/*{{{*/

  // - prepare file descriptor sets -
  fd_set rs; FD_ZERO(&rs);
  fd_set ws; FD_ZERO(&ws);
  fd_set es; FD_ZERO(&es);

  // - ERROR -
  int max_fd = 0;
  if (MHD_YES != MHD_get_fdset(m_daemon_ptr,&rs,&ws,&es,&max_fd))
  {
    cclthrow(error_HTTP_SERVER_INTERNAL_ERROR);
  }

  if (max_fd >= 0)
  {
    int fd = 0;
    do {
      int fd_value = -1;
      int16_t events = 0;

      // - test fd in read fd_set -
      if (FD_ISSET(fd,&rs))
      {
        fd_value = fd;
        events |= POLLIN;
      }

      // - test fd in write fd_set -
      if (FD_ISSET(fd,&ws))
      {
        fd_value = fd;
        events |= POLLOUT;
      }

      // - test fd in except fd_set -
      if (FD_ISSET(fd,&es))
      {
        fd_value = fd;
        events |= POLLERR | POLLHUP | POLLNVAL;
      }

      if (fd_value != -1)
      {
        a_poll_fds.push_back(pollfd{fd_value,events,0});
      }

    } while(++fd <= max_fd);
  }

  return *this;
}/*}}}*/

server_c &server_c::process()
{/*{{{*/

  // - reset exception pointer -
  if (m_exc_ptr != nullptr)
  {
    m_exc_ptr = nullptr;
  }

  // - ERROR -
  if (MHD_run(m_daemon_ptr) != MHD_YES)
  {
    cclthrow(error_HTTP_SERVER_INTERNAL_ERROR);
  }

  // - rethrow catched exception -
  if (m_exc_ptr != nullptr)
  {
    std::rethrow_exception(m_exc_ptr);
  }

  return *this;
}/*}}}*/

int connection_c::key_value_func(void *cls,enum MHD_ValueKind kind,
    const char *key,const char *value)
{/*{{{*/
  (void)kind;

  static_cast<connection_c *>(cls)->m_key_value_map.insert({key,value});

  return MHD_YES;
}/*}}}*/

std::string connection_c::upload_data()
{/*{{{*/
  std::string upload_data(m_upload_data,*m_upload_data_size);

  // - reset upload data size -
  *m_upload_data_size = 0;

  return upload_data;
}/*}}}*/

connection_c &connection_c::values(enum MHD_ValueKind a_kind,string_map_t &a_values_map)
{/*{{{*/

  // - retrieve key value locations -
  MHD_get_connection_values(m_conn_ptr,a_kind,&key_value_func,this);

  // - swap key values to result -
  a_values_map.clear();
  a_values_map.swap(m_key_value_map);

  return *this;
}/*}}}*/

connection_c::string_map_t connection_c::values(enum MHD_ValueKind a_kind)
{/*{{{*/
  string_map_t values_map;
  values(a_kind,values_map);
  return values_map;
}/*}}}*/

connection_c &connection_c::queue_response(unsigned a_status,const response_c &a_resp)
{/*{{{*/

  // - queue response to be transmitted to client -
  int result = MHD_queue_response(m_conn_ptr,a_status,a_resp.m_resp_ptr);

  // - ERROR -
  if (result != MHD_YES)
  {
    cclthrow(error_HTTP_CONN_CANNOT_QUEUE_RESPONSE);
  }

  return *this;
}/*}}}*/

connection_c &connection_c::queue_basic_auth_fail_response(
    const std::string &a_realm, 
    const response_c &a_resp)
{/*{{{*/
  int result = MHD_queue_basic_auth_fail_response(
      m_conn_ptr,
      a_realm.data(),
      a_resp.m_resp_ptr);

  // - ERROR -
  if (result != MHD_YES)
  {
    cclthrow(error_HTTP_CONN_CANNOT_QUEUE_RESPONSE);
  }

  return *this;
}/*}}}*/

connection_c &connection_c::basic_auth_username_password(
    std::string &a_user,
    std::string &a_pass)
{/*{{{*/
  char *user;
  char *pass = nullptr;

  user = MHD_basic_auth_get_username_password(m_conn_ptr,&pass);

  if (user != nullptr)
  {
    a_user.assign(user);
    free(user);
  }
  else
  {
    a_user.clear();
  }

  if (pass != nullptr)
  {
    a_pass.assign(pass);
    free(pass);
  }
  else
  {
    a_pass.clear();
  }

  return *this;
}/*}}}*/

connection_c &connection_c::queue_auth_fail_response(
    const std::string &a_realm,
    const std::string &a_opaque,
    const response_c &a_resp,
    int a_signal_stale)
{/*{{{*/
  int result = MHD_queue_auth_fail_response(
      m_conn_ptr,
      a_realm.data(),
      a_opaque.data(),
      a_resp.m_resp_ptr,
      a_signal_stale);

  // - ERROR -
  if (result != MHD_YES)
  {
    cclthrow(error_HTTP_CONN_CANNOT_QUEUE_RESPONSE);
  }

  return *this;
}/*}}}*/

std::string connection_c::auth_username()
{/*{{{*/
  char *user_name = MHD_digest_auth_get_username(m_conn_ptr);

  if (user_name != nullptr)
  {
    std::string string(user_name);
    free(user_name);
    return string;
  }

  return "";
}/*}}}*/

bool connection_c::auth_check(
    const std::string &a_realm,
    const std::string &a_user,
    const std::string &a_pass,
    unsigned a_nonce_timeout,
    int &a_reason)
{/*{{{*/
  a_reason = MHD_digest_auth_check(
      m_conn_ptr,
      a_realm.data(),
      a_user.data(),
      a_pass.data(),
      a_nonce_timeout);

  // - if not authenticated return false -
  return !((a_reason == MHD_INVALID_NONCE) || (a_reason == MHD_NO));
}/*}}}*/

post_proc_c *connection_c::new_post_proc(const std::string &a_file_name)
{/*{{{*/
  return new post_proc_c(m_conn_ptr,a_file_name);
}/*}}}*/

post_proc_c::post_proc_c(MHD_Connection *a_conn_ptr,const std::string &a_file_name) :
m_post_proc{nullptr},m_total_size{0},m_upload_size{0},m_file{nullptr}
{/*{{{*/

  // - if file name is not empty -
  if (!a_file_name.empty())
  {
    m_file = fopen(a_file_name.data(),"w");

    // - ERROR -
    if (m_file == nullptr)
    {
      cclthrow(error_POST_PROC_FILE_OPEN_ERROR);
    }

    m_post_proc = MHD_create_post_processor(a_conn_ptr,65536,cb_post_callback,
        static_cast<void *>(this));

    // - ERROR -
    if (m_post_proc == nullptr)
    {
      cclthrow(error_POST_PROC_FILE_POST_PROCESSOR_CREATE_ERROR);
    }
  }
}/*}}}*/

int post_proc_c::post_callback(const char *key,const char *data,size_t size)
{/*{{{*/
  
  // - process file data -
  if (strcmp(key,"file") == 0)
  {
    // - update upload size -
    m_upload_size += size;

    // - ERROR -
    if (fwrite(data,size,1,m_file) != 1)
    {
      cclthrow(error_POST_PROC_FILE_WRITE_ERROR);
    }
  }

  // - process file size -
  else if (strcmp(key,"size") == 0)
  {
    // - retrieve total size -
    m_total_size = strtoll(data,nullptr,10);
  }
  else
  {
    return MHD_NO;
  }

  return MHD_YES;
}/*}}}*/

post_proc_c::~post_proc_c()
{/*{{{*/
  if (m_file != nullptr)
  {
    fclose(m_file);
  }

  if (m_post_proc != nullptr)
  {
    MHD_destroy_post_processor(m_post_proc);
  }
}/*}}}*/

void post_proc_c::process(const std::string &a_data)
{/*{{{*/
  if (m_post_proc != nullptr)
  {
    MHD_post_process(m_post_proc,a_data.data(),a_data.size());
  }
}/*}}}*/

double post_proc_c::progress()
{/*{{{*/
  if (m_total_size == 0)
  {
    return 0.0;
  }

  return (100.0*m_upload_size)/m_total_size;
}/*}}}*/

response_c::response_c(const std::string &a_source,int a_type) :
  m_resp_ptr{nullptr}
{/*{{{*/
  switch (a_type)
  {
  case c_resp_from_string:
    {
      // - create response from buffer -
      m_resp_ptr = MHD_create_response_from_buffer(a_source.length(),
          const_cast<char *>(a_source.data()),MHD_RESPMEM_MUST_COPY);
    }
    break;

  case c_resp_from_file:
    {
      // - open file descriptor -
      int fd = open(a_source.data(),O_RDONLY);

      // - ERROR -
      if (fd == -1)
      {
        cclthrow(error_HTTP_RESP_CANNOT_READ_SOURCE_FILE);
      }

      // - retrieve file size -
      struct stat file_stat;

      // - ERROR -
      if (fstat(fd,&file_stat) != 0)
      {
        close(fd);

        cclthrow(error_HTTP_RESP_CANNOT_READ_SOURCE_FILE);
      }

      // - create response from file descriptor -
      m_resp_ptr = MHD_create_response_from_fd(file_stat.st_size,fd);
    }
    break;

  // - ERROR -
  default:

    cclthrow(error_HTTP_RESP_UNKNOWN_DATA_SOURCE_IDENTIFIER);
  }

  // - ERROR -
  if (m_resp_ptr == nullptr)
  {
    cclthrow(error_HTTP_RESP_CREATE_ERROR);
  }
}/*}}}*/

response_c::~response_c()
{/*{{{*/
  if (m_resp_ptr != nullptr)
  {
    MHD_destroy_response(m_resp_ptr);
  }
}/*}}}*/

response_c &response_c::add_header(const std::string &a_header,const std::string &a_value)
{/*{{{*/

  // - ERROR -
  if (MHD_add_response_header(m_resp_ptr,a_header.data(),a_value.data()) == MHD_NO)
  {
    cclthrow(error_HTTP_RESP_CANNOT_ADD_HEADER_FOOTER);
  }

  return *this;
}/*}}}*/

response_c &response_c::add_footer(const std::string &a_footer,const std::string &a_value)
{/*{{{*/

  // - ERROR -
  if (MHD_add_response_footer(m_resp_ptr,a_footer.data(),a_value.data()) == MHD_NO)
  {
    cclthrow(error_HTTP_RESP_CANNOT_ADD_HEADER_FOOTER);
  }

  return *this;
}/*}}}*/

} // namespace cclhttp

