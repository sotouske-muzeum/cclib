
#include "cclwebs.h"

namespace cclwebs {

  int context_c::http_func(libwebsocket_context *ctx,libwebsocket *wsi,
      enum libwebsocket_callback_reasons reason,void *user,void *in,size_t len)
{/*{{{*/
  (void)wsi;(void)user;(void)len;

  switch (reason)
  {
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
      break;
    case LWS_CALLBACK_ADD_POLL_FD:
    case LWS_CALLBACK_DEL_POLL_FD:
    case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
      {
        context_c *wsc_ptr = static_cast<context_c *>(libwebsocket_context_user(ctx));
        pollfd_set_t &pfd_set = wsc_ptr->m_pfd_set;

        libwebsocket_pollargs *args_ptr = static_cast<libwebsocket_pollargs *>(in);
        pollfd pfd{args_ptr->fd,static_cast<int16_t>(args_ptr->events),0};

        switch (reason)
        {
          // - add poll fd -
          case LWS_CALLBACK_ADD_POLL_FD:
            pfd_set.insert(pfd);
            break;

          // - delete poll fd -
          case LWS_CALLBACK_DEL_POLL_FD:
            pfd_set.erase(pfd);
            break;

          // - change poll fd mode -
          case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
            {
              auto iter = pfd_set.find(pfd);
              assert(iter != pfd_set.end());

              const_cast<struct pollfd &>(*iter).events = pfd.events;
            }
            break;

          default:
            break;
        }
      }
      break;
    default:
      break;
  }

  return 0;
}/*}}}*/

int context_c::protocol_func(libwebsocket_context *ctx,libwebsocket *wsi,
    enum libwebsocket_callback_reasons reason,void *user,void *in,size_t len)
{/*{{{*/
  switch (reason)
  {
    case LWS_CALLBACK_ESTABLISHED:
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
    case LWS_CALLBACK_CLOSED:
    case LWS_CALLBACK_RECEIVE:
    case LWS_CALLBACK_CLIENT_RECEIVE:
    case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
    case LWS_CALLBACK_CLIENT_WRITEABLE:
    case LWS_CALLBACK_SERVER_WRITEABLE:
      {
        context_c *wsc_ptr = static_cast<context_c *>(libwebsocket_context_user(ctx));

        if (wsc_ptr->m_exc_ptr == nullptr)
        {
          // - if connection established -
          if (reason == LWS_CALLBACK_ESTABLISHED ||
              reason == LWS_CALLBACK_CLIENT_ESTABLISHED)
          {
            // - create websocket connection object -
            auto wscn_ptr = new connection_c;

            // - set websocket context pointer -
            wscn_ptr->m_wsc_ptr = wsc_ptr;

            // - set protocol index pointer -
            const libwebsocket_protocols *protocol = libwebsockets_get_protocol(wsi);
            prot_names_t &prot_names = wsc_ptr->m_prot_names;

            auto pn_iter = std::find(prot_names.begin(),prot_names.end(),protocol->name);
            assert(pn_iter != prot_names.end());
            wscn_ptr->m_prot_idx = pn_iter - prot_names.begin();

            // - set websocket pointer -
            wscn_ptr->m_ws_ptr = wsi;

            // - set websocket client status -
            if (reason == LWS_CALLBACK_CLIENT_ESTABLISHED)
            {
              client_c *wscl_ptr = *(static_cast<client_c **>(user));

              // - set websocket client connected flag -
              wscl_ptr->m_connected = true;
            }

            // - store websocket connection pointer -
            *(static_cast<connection_c **>(user)) = wscn_ptr;
          }

          // - retrieve connection pointer -
          connection_c *wscn_ptr = *static_cast<connection_c **>(user);

          // - retrieve count of remaining bytes of message -
          size_t remaining = libwebsockets_remaining_packet_payload(wsi);
          auto &data_buffer = wscn_ptr->m_data_buffer;

          // - message is not complete or buffered data exists -
          if (remaining != 0 || !data_buffer.empty())
          {
            const char *char_in = static_cast<const char *>(in);

            data_buffer.reserve(len + remaining);
            data_buffer.insert(data_buffer.end(),char_in,char_in + len);
          }

          if (remaining == 0)
          {
            // - retrieve delegate pointer -
            conn_callback_t callback = wsc_ptr->m_prot_cbs[wscn_ptr->m_prot_idx];

            // - set callback reason -
            wscn_ptr->m_reason = reason;

            // - set callback input data -
            if (!data_buffer.empty())
            {
              wscn_ptr->data_in = data_buffer.data();
              wscn_ptr->data_len = data_buffer.size();
              data_buffer.clear();
            }
            else
            {
              wscn_ptr->data_in = in;
              wscn_ptr->data_len = len;
            }

            // - call callback - 
            try {
              callback(*wscn_ptr);
            }
            catch (...)
            {
              // - release connection -
              delete wscn_ptr;

              // - store current exception -
              wsc_ptr->m_exc_ptr = std::current_exception();

              return 1;
            }
          }

          // - if connection closed -
          if (reason == LWS_CALLBACK_CLOSED)
          {
            // - release connection -
            delete wscn_ptr;
          }
        }
      }
      break;
    default:
      break;
  }

  return 0;
}/*}}}*/

context_c::context_c(int a_port,const prot_descrs_t a_prot_descrs) :
  m_context{nullptr}, m_user_data{nullptr}
{/*{{{*/
  m_exc_ptr = nullptr;

  // - create websocket protocols structure -
  m_protocols.resize(2 + a_prot_descrs.size());

  // - configure http protocol -
  libwebsocket_protocols &http_prot = m_protocols[0];
  http_prot.name = "http-only";
  http_prot.callback = context_c::http_func;
  http_prot.per_session_data_size = 0;
  http_prot.rx_buffer_size = 0;

  // - process protocol names and delegates -
  int prot_idx = 1;
  for (auto prot_descr_i = a_prot_descrs.begin();
            prot_descr_i != a_prot_descrs.end();
          ++prot_descr_i)
  {
    m_prot_names.push_back(prot_descr_i->name());
    m_prot_cbs.push_back(prot_descr_i->callback());

    // - configure websocket protocol -
    libwebsocket_protocols &ws_prot = m_protocols[prot_idx++];
    ws_prot.name = m_prot_names.back().data();
    ws_prot.callback = context_c::protocol_func;
    ws_prot.per_session_data_size = sizeof(void *);
    ws_prot.rx_buffer_size = 0;
    ws_prot.no_buffer_all_partial_tx = 1;
  }

  // - configure terminator protocol -
  libwebsocket_protocols &term_prot = m_protocols[prot_idx];
  term_prot.name = nullptr;
  term_prot.callback = nullptr;
  term_prot.per_session_data_size = 0;
  term_prot.rx_buffer_size = 0;

  // - create websocket context creation info -
  lws_context_creation_info info;
  memset(&info,0,sizeof info);

  info.port = a_port;
  info.iface = nullptr;
  info.protocols = m_protocols.data();
  info.extensions = libwebsocket_get_internal_extensions();
  info.gid = -1;
  info.uid = -1;
  info.options = 0;
  info.user = this;

  // - create wesocket context -
  m_context = libwebsocket_create_context(&info);

  // - ERROR -
  if (m_context == nullptr)
  {
    cclthrow(error_WS_CONTEXT_CREATE_ERROR);
  }
}/*}}}*/

context_c::~context_c()
{/*{{{*/

  // - destroy websocket context -
  if (m_context != nullptr)
  {
    libwebsocket_context_destroy(m_context);
  }
}/*}}}*/

context_c &context_c::retrieve_poll_fds(std::vector<pollfd> &a_poll_fds)
{/*{{{*/
  a_poll_fds.insert(a_poll_fds.begin(),m_pfd_set.begin(),m_pfd_set.end());
  return *this;
}/*}}}*/

context_c &context_c::process(int a_timeout)
{/*{{{*/

  // - reset exception pointer -
  if (m_exc_ptr != nullptr)
  {
    m_exc_ptr = nullptr;
  }

  // - service websocket context -
  libwebsocket_service(m_context,a_timeout);

  // - rethrow catched exception -
  if (m_exc_ptr != nullptr)
  {
    std::rethrow_exception(m_exc_ptr);
  }

  return *this;
}/*}}}*/

unsigned context_c::protocol_idx(const std::string &a_prot_name) const
{/*{{{*/
  auto pn_iter = std::find(m_prot_names.begin(),m_prot_names.end(),a_prot_name);

  // - ERROR -
  if (pn_iter == m_prot_names.end())
  {
    cclthrow(error_WS_CONTEXT_PROTOCOL_NAME_NOT_FOUND);
  }

  return pn_iter - m_prot_names.begin();
}/*}}}*/

context_c &context_c::callback_on_writable(unsigned a_prot_idx)
{/*{{{*/

  // - ERROR -
  if (a_prot_idx >= m_prot_names.size())
  {
    cclthrow(error_WS_CONTEXT_PROTOCOL_INDEX_EXCEEDS_RANGE);
  }

  libwebsocket_callback_on_writable_all_protocol(&m_protocols[1 + a_prot_idx]);

  return *this;
}/*}}}*/

std::string connection_c::protocol_name() const
{/*{{{*/

  // - retrieve websocket protocol -
  const libwebsocket_protocols *protocol = libwebsockets_get_protocol(m_ws_ptr);

  return protocol->name;
}/*}}}*/

const connection_c &connection_c::callback_on_writable() const
{/*{{{*/
  libwebsocket_callback_on_writable(m_wsc_ptr->m_context,m_ws_ptr);
  return *this;
}/*}}}*/

connection_c &connection_c::set_timeout(enum pending_timeout a_reason,int a_seconds)
{/*{{{*/

  // - set connection timeout -
  libwebsocket_set_timeout(m_ws_ptr,a_reason,a_seconds);

  return *this;
}/*}}}*/

connection_c &connection_c::write(const std::string &a_data,enum libwebsocket_write_protocol a_type)
{/*{{{*/

  // - allocate data buffer -
  auto buffer = new unsigned char[
      LWS_SEND_BUFFER_PRE_PADDING + a_data.length() +
      LWS_SEND_BUFFER_POST_PADDING];

  // - pointer to data in buffer -
  unsigned char *buff_ptr = buffer + LWS_SEND_BUFFER_PRE_PADDING;

  // - fill data to buffer -
  memcpy(buff_ptr,a_data.data(),a_data.length());

  // - ERROR -
  if (libwebsocket_write(m_ws_ptr,buff_ptr,a_data.length(),a_type) < 0)
  {
    // - release data buffer -
    delete [] buffer;

    cclthrow(error_WS_CONN_WRITE_ERROR);
  }

  // - release data buffer -
  delete [] buffer;

  return *this;
}/*}}}*/

client_c::client_c(context_c &a_ctx,const std::string a_addr,int a_port,const std::string a_path,const std::string a_prot) :
  m_wsc_ptr{&a_ctx}, m_ws_ptr{nullptr}, m_wscl_udp_ptr{nullptr}, m_connected{false}
{/*{{{*/
  (void)a_addr;(void)a_port;(void)a_path;(void)a_prot;

  cclthrow("websocket client not implemented");
}/*}}}*/

void cclwebs_c::log_emit(int a_level,const char *a_line)
{/*{{{*/
  (void)a_level;(void)a_line;

  //fprintf(stderr,"LWS_LOG: %d,%s",a_level,a_line);
}/*}}}*/

cclwebs_c::cclwebs_c() throw()
{/*{{{*/
  lws_set_log_level(9,log_emit);
}/*}}}*/

// - websocket global init object -
cclwebs_c g_cclwebs;

} // namespace cclwebs

