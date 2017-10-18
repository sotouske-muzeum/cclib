
#include <cassert>

#include "cclsys.h"
#include "cclwebs.h"

typedef std::vector<std::string> strings_t;

bool g_terminate = false;
void signal_handler(int signum)
{/*{{{*/
  if (signum != SIGCHLD)
  {
    g_terminate = true;
  }
}/*}}}*/

void server_callback(cclwebs::connection_c &a_conn)
{/*{{{*/
  switch (a_conn.reason())
  {
    case LWS_CALLBACK_ESTABLISHED:
      std::cout << "LWS_CALLBACK_ESTABLISHED" << std::endl;
      a_conn.callback_on_writable();
      break;
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
      std::cout << "LWS_CALLBACK_CLIENT_CONNECTION_ERROR" << std::endl;
      break;
    case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
      std::cout << "LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH" << std::endl;
      break;
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
      std::cout << "LWS_CALLBACK_CLIENT_ESTABLISHED" << std::endl;
      break;
    case LWS_CALLBACK_CLOSED:
      {
        std::cout << "LWS_CALLBACK_CLOSED" << std::endl;

        // - delete user data if any -
        void *&user_data = a_conn.user_data();
        if (user_data != nullptr) {
          delete static_cast<std::string *>(a_conn.user_data());
          user_data = nullptr;
        }
      }
      break;
    case LWS_CALLBACK_RECEIVE:
      {
        std::cout << "LWS_CALLBACK_RECEIVE" << std::endl;
        
        std::string *message = new std::string(a_conn.received_data());
        void *&user_data = a_conn.user_data();

        // - delete user data if any -
        if (user_data != nullptr) {
          delete static_cast<std::string *>(a_conn.user_data());
        }

        user_data = message;

        a_conn.callback_on_writable();
      }
      break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
      std::cout << "LWS_CALLBACK_CLIENT_RECEIVE" << std::endl;
      break;
    case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
      std::cout << "LWS_CALLBACK_CLIENT_RECEIVE_PONG" << std::endl;
      break;
    case LWS_CALLBACK_CLIENT_WRITEABLE:
      std::cout << "LWS_CALLBACK_CLIENT_WRITEABLE" << std::endl;
      break;
    case LWS_CALLBACK_SERVER_WRITEABLE:
      {
        std::cout << "LWS_CALLBACK_SERVER_WRITEABLE" << std::endl;

        void *&user_data = a_conn.user_data();
        if (user_data != nullptr)
        {
          std::string *string_ptr = static_cast<std::string *>(user_data);
          user_data = nullptr;

          a_conn.write("Echo: " + *string_ptr);
          delete string_ptr;
        }
        else
        {
          a_conn.write("Hello from C++ server");
        }
      }
      break;
    case LWS_CALLBACK_HTTP:
      std::cout << "LWS_CALLBACK_HTTP" << std::endl;
      break;
    case LWS_CALLBACK_HTTP_FILE_COMPLETION:
      std::cout << "LWS_CALLBACK_HTTP_FILE_COMPLETION" << std::endl;
      break;
    case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
      std::cout << "LWS_CALLBACK_FILTER_NETWORK_CONNECTION" << std::endl;
      break;
    case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
      std::cout << "LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION" << std::endl;
      break;
    case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
      std::cout << "LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS" << std::endl;
      break;
    case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS:
      std::cout << "LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS" << std::endl;
      break;
    case LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
      std::cout << "LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION" << std::endl;
      break;
    case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
      std::cout << "LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER" << std::endl;
      break;
    case LWS_CALLBACK_CONFIRM_EXTENSION_OKAY:
      std::cout << "LWS_CALLBACK_CONFIRM_EXTENSION_OKAY" << std::endl;
      break;
    case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
      std::cout << "LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED" << std::endl;
      break;
    case LWS_CALLBACK_PROTOCOL_INIT:
      std::cout << "LWS_CALLBACK_PROTOCOL_INIT" << std::endl;
      break;
    case LWS_CALLBACK_PROTOCOL_DESTROY:
      std::cout << "LWS_CALLBACK_PROTOCOL_DESTROY" << std::endl;
      break;
    case LWS_CALLBACK_ADD_POLL_FD:
      std::cout << "LWS_CALLBACK_ADD_POLL_FD" << std::endl;
      break;
    case LWS_CALLBACK_DEL_POLL_FD:
      std::cout << "LWS_CALLBACK_DEL_POLL_FD" << std::endl;
      break;
    case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
      std::cout << "LWS_CALLBACK_CHANGE_MODE_POLL_FD" << std::endl;
      break;
    default:
      assert(false);
  }
}/*}}}*/

void test_cclwebs_server()
{/*{{{*/
  cclsys::install_signals(signal_handler);

  cclwebs::context_c context{8001,cclwebs::prot_descrs_t{
    {"dumb-protocol",server_callback}
  }};
  std::vector<pollfd> pfds;

  std::cout << "version: " << context.version() << std::endl;
  std::cout << "protocol_idx: " << context.protocol_idx("dumb-protocol") << std::endl;

  do {
    pfds.clear();
    context.retrieve_poll_fds(pfds);
    
    int res = poll(pfds.data(),pfds.size(),1000);
    if (res > 0)
    {
      context.process(0);

      // - test output -
      std::cout << "served ..." << std::endl;
    }
    else
    {
      // - test output -
      std::cout << "waiting ..." << std::endl;
    }
  } while(!g_terminate);
}/*}}}*/

void test_cclwebs_client()
{/*{{{*/
  assert(false);
}/*}}}*/

void test_cclwebs_all()
{/*{{{*/
  cclsys::pid_c server{strings_t{"./cclwebs_test","server"}};
  cclsys::sleep(100);

  int count = 5;
  do {
    cclsys::pid_c client{strings_t{"nodejs","../tests/tools/websocket/ws-client.js"}};
    assert(client.wait(0) == 0);
  } while(--count > 0);

  assert(kill(server.pid(),SIGTERM) == 0);
  assert(server.wait(0) == 0);
}/*}}}*/

int main(int argc,char **argv)
{/*{{{*/
  if (argc > 1)
  {
    int arg_idx = 1;
    do {
      if (std::string("server") == argv[arg_idx])
      {
        test_cclwebs_server();
      }
      else if (std::string("client") == argv[arg_idx])
      {
        test_cclwebs_client();
      }
      else if (std::string("all") == argv[arg_idx])
      {
        test_cclwebs_all();
      }
      else {
        // - ERROR -
        throw std::runtime_error{"unrecognized argument"};
      }
    } while(++arg_idx < argc);
  }

  return 0;
}/*}}}*/

