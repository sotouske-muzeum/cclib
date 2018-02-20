
#ifndef CCLIB_CCLWEBS_H
#define CCLIB_CCLWEBS_H

#include "cclbase.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <poll.h>
#include <libwebsockets.h>

namespace cclwebs {

class connection_c;

// - exceptions list -
const std::string error_WS_CONTEXT_CREATE_ERROR = "error while creating websocket context";
const std::string error_WS_CONTEXT_PROTOCOL_NAME_NOT_FOUND = "protocol was not found by name";
const std::string error_WS_CONTEXT_PROTOCOL_INDEX_EXCEEDS_RANGE = "protocol index exceeds protocol count";
const std::string error_WS_CONN_WRITE_ERROR = "error while writing to websocket";

typedef void (*conn_callback_t)(connection_c &);
typedef std::vector<conn_callback_t> conn_callbacks_t;

class pollfd_compare
{/*{{{*/
  public:
  bool operator() (const pollfd &a_first,const pollfd &a_second) const
  {
    return a_first.fd < a_second.fd;
  }
};/*}}}*/

class prot_descr_c
{/*{{{*/
  private:
  std::string m_name;
  conn_callback_t m_callback;
  public:
  prot_descr_c(std::string a_name,conn_callback_t a_callback) :
    m_name(std::move(a_name)), m_callback{a_callback} {}
  prot_descr_c(const prot_descr_c &a_prot) :
    m_name(a_prot.m_name), m_callback{a_prot.m_callback} {}
  prot_descr_c(prot_descr_c &&a_prot) throw() :
    m_name(std::move(a_prot.m_name)), m_callback{a_prot.m_callback} {}

  const std::string &name() const { return m_name; }
  conn_callback_t callback() const { return m_callback; }
};/*}}}*/

typedef std::vector<prot_descr_c> prot_descrs_t;

class context_c
{/*{{{*/
  friend class connection_c;

  typedef std::set<pollfd,pollfd_compare> pollfd_set_t;
  typedef std::vector<std::string> prot_names_t;
  typedef std::vector<libwebsocket_protocols> libwebsocket_protocols_t;

  private:
  std::exception_ptr m_exc_ptr;
  libwebsocket_context *m_context;
  libwebsocket_protocols_t m_protocols;
  void *m_user_data;
  prot_names_t m_prot_names;
  conn_callbacks_t m_prot_cbs;
  pollfd_set_t m_pfd_set;

  static int http_func(libwebsocket_context *ctx,libwebsocket *wsi,
      enum libwebsocket_callback_reasons reason,void *user,void *in,size_t len);

  static int protocol_func(libwebsocket_context *ctx,libwebsocket *wsi,
      enum libwebsocket_callback_reasons reason,void *user,void *in,size_t len);

  public:
  context_c(const context_c &) = delete;
  void operator = (const context_c &) = delete;
  context_c(int a_port,const prot_descrs_t &a_prot_descrs);
  ~context_c();

  static std::string version() { return lws_get_library_version(); };
  void *&user_data() { return m_user_data; }
  context_c &retrieve_poll_fds(std::vector<pollfd> &a_poll_fds);
  context_c &process(int a_timeout);
  unsigned protocol_idx(const std::string &a_prot_name) const;
  context_c &callback_on_writable(unsigned a_prot_idx);
};/*}}}*/

class connection_c
{/*{{{*/
  friend class context_c;

  typedef std::vector<char> data_buffer_t;

  private:
  context_c *m_wsc_ptr;
  unsigned m_prot_idx;
  libwebsocket *m_ws_ptr;
  void *m_user_data;
  libwebsocket_callback_reasons m_reason;
  data_buffer_t m_data_buffer;
  void *data_in;
  size_t data_len;

  connection_c() :
    m_wsc_ptr{nullptr}, m_ws_ptr{nullptr}, m_user_data{nullptr},
    data_in{nullptr}, data_len{0} {}
  public:
  connection_c(const connection_c &) = delete;
  void operator = (const connection_c &) = delete;
  ~connection_c() = default;

  libwebsocket_callback_reasons reason() { return m_reason; }
  std::string received_data() const { return std::string(static_cast<const char *>(data_in),data_len); }
  void *&user_data() { return m_user_data; }
  void *ctx_user_data() { return m_wsc_ptr->user_data(); }
  std::string protocol_name() const;
  const connection_c &callback_on_writable() const;
  connection_c &set_timeout(enum pending_timeout a_reason,int a_seconds);
  connection_c &write(const std::string &a_data,enum libwebsocket_write_protocol a_type);
  connection_c &write(const std::string &a_data) { return write(a_data,LWS_WRITE_TEXT); }
};/*}}}*/

class client_c
{/*{{{*/
  friend class context_c;

  private:
  context_c *m_wsc_ptr;      // NOLINT
  libwebsocket *m_ws_ptr;    // NOLINT
  client_c **m_wscl_udp_ptr; // NOLINT
  bool m_connected;

  client_c(context_c &a_ctx,const std::string a_addr,int a_port,const std::string a_path,const std::string a_prot);
  public:
  client_c(const client_c &) = delete;
  void operator = (const client_c &) = delete;
};/*}}}*/

class cclwebs_c
{/*{{{*/
  private:
  static void log_emit(int a_level,const char *a_line);

  public:
  cclwebs_c() throw();
};/*}}}*/

} // namespace cclwebs

#endif

