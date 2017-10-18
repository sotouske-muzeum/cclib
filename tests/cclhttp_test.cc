
#include <cassert>
#include <sstream>
#include <fstream>

#include "cclsys.h"
#include "cclhttp.h"

typedef std::vector<std::string> strings_t;

bool g_terminate = false;
void signal_handler(int signum)
{/*{{{*/
  if (signum != SIGCHLD)
  {
    g_terminate = true;
  }
}/*}}}*/

void server_connection(cclhttp::connection_c &a_conn)
{/*{{{*/
  std::cout << "a_conn.url(): " << a_conn.url() << std::endl;
  std::cout << "a_conn.method(): " << a_conn.method() << std::endl;
  std::cout << "a_conn.version(): " << a_conn.version() << std::endl;

  switch (a_conn.type())
  {
  case cclhttp::Method::HEAD:
  case cclhttp::Method::GET:
  {/*{{{*/
    cclhttp::connection_c::values_map_t values_map;
    a_conn.values(MHD_HEADER_KIND,values_map);
    
    for (auto value_i = values_map.begin();value_i != values_map.end();++value_i)
    {
      std::cout << "value: " << value_i->first << ": " << value_i->second << std::endl;
    }

    if (a_conn.url() == "/file")
    {
      cclhttp::response_c resp{"../tests/data/cclhttp/track.gpx",cclhttp::c_resp_from_file};
      resp.add_header("Content-Type","text/xml");
      a_conn.queue_response(MHD_HTTP_OK,resp);
    }
    else
    {
      cclhttp::response_c resp{"Response created from string",cclhttp::c_resp_from_string};
      resp.add_header("Content-Type","text/plain");
      a_conn.queue_response(MHD_HTTP_OK,resp);
    }
  }/*}}}*/
  break;
  case cclhttp::Method::DELETE:
  {/*{{{*/
    cclhttp::response_c resp{"<html><body>OK</body></html>",cclhttp::c_resp_from_string};
    resp.add_header("Content-Type","text/html");
    a_conn.queue_response(MHD_HTTP_OK,resp);
  }/*}}}*/
  break;
  case cclhttp::Method::PUT:
  {/*{{{*/

    // - retrieve data -
    auto data = a_conn.upload_data();

    // - retrieve string buffer -
    std::stringbuf *buff_ptr{static_cast<std::stringbuf *>(a_conn.user_data())};
    if (buff_ptr == nullptr)
    {
      a_conn.user_data() = buff_ptr = new std::stringbuf{std::ios_base::out};
    }

    // - if some data was received -
    if (data != "")
    {
      buff_ptr->sputn(data.data(),data.length());
    }
    else
    {
      // - write string buffer to file -
      std::string data = buff_ptr->str();
      std::ofstream out_stream("Testing/Temporary/data.txt",std::ofstream::out);
      out_stream.write(data.data(),data.length());

      cclhttp::response_c resp{"<html><body>OK</body></html>",cclhttp::c_resp_from_string};
      resp.add_header("Content-Type","text/html");
      a_conn.queue_response(MHD_HTTP_OK,resp);
    }
  }/*}}}*/
  break;
  default:
    assert(false);
  }
}/*}}}*/

void server_completed(cclhttp::connection_c &a_conn)
{/*{{{*/
  switch (a_conn.type())
  {
  case cclhttp::Method::PUT:
  {/*{{{*/

    // - retrieve string buffer -
    std::stringbuf *buff_ptr{static_cast<std::stringbuf *>(a_conn.user_data())};
    if (buff_ptr != nullptr)
    {
      delete buff_ptr;
    }
  }/*}}}*/
  break;
  default:
    ;
  }
}/*}}}*/

void auth_server_callback(cclhttp::connection_c &a_conn)
{/*{{{*/
  const std::string page_granted = "<html><head><title>auth</title></head><body>Access granted</body></html>";
  const std::string page_denied = "<html><head><title>auth</title></head><body>Access denied</body></html>";

  const std::string realm = "user@testing.cz";
  const std::string password = "password";

  const enum
  {
    c_auth_basic = 0,
    c_auth_digest,
  }
  auth_type = c_auth_digest;

  // - basic authentication -
  if (auth_type == c_auth_basic)
  {/*{{{*/
    std::string user;
    std::string pass;
    a_conn.basic_auth_username_password(user,pass);

    if (user == "jirka" && pass == password)
    {
      // - test output -
      {
        auto values_map = a_conn.values(MHD_HEADER_KIND);
        std::cout << "Authorization: " << values_map["Authorization"] << std::endl;
      }

      cclhttp::response_c resp{page_granted,cclhttp::c_resp_from_string};
      resp.add_header("Content-Type","text/html");
      a_conn.queue_response(MHD_HTTP_OK,resp);
      return;
    }

    // - authentication failed (fallback page) -
    cclhttp::response_c resp{page_denied,cclhttp::c_resp_from_string};
    resp.add_header("Content-Type","text/html");
    a_conn.queue_basic_auth_fail_response(realm,resp);
  }/*}}}*/

  // - digest authentication -
  if (auth_type == c_auth_digest)
  {/*{{{*/
    const std::string opaque = "some-random-hash";

    std::string user = a_conn.auth_username();
    if (user != "")
    {
      // - test output -
      {
        auto values_map = a_conn.values(MHD_HEADER_KIND);
        std::cout << "Authorization: " << values_map["Authorization"] << std::endl;
      }

      int reason;
      if (a_conn.auth_check(realm,user,password,300,reason))
      {
        // - authenticated user -
        cclhttp::response_c resp{page_granted,cclhttp::c_resp_from_string};
        resp.add_header("Content-Type","text/html");
        a_conn.queue_response(MHD_HTTP_OK,resp);
        return;
      }

      // - authentication failed (fallback page) -
      cclhttp::response_c resp{page_denied,cclhttp::c_resp_from_string};
      resp.add_header("Content-Type","text/html");
      a_conn.queue_auth_fail_response(realm,opaque,resp,
        reason == MHD_INVALID_NONCE ? MHD_YES : MHD_NO);
      return;
    }

    // - authentication failed (fallback page) -
    cclhttp::response_c resp{page_denied,cclhttp::c_resp_from_string};
    resp.add_header("Content-Type","text/html");
    a_conn.queue_auth_fail_response(realm,opaque,resp,MHD_NO);
  }/*}}}*/

}/*}}}*/

void test_cclhttp_server()
{/*{{{*/
  cclsys::install_signals(signal_handler);

  cclhttp::server_c server{8888,server_connection,server_completed};
  std::vector<pollfd> pfds;

  do {
    pfds.clear();
    server.retrieve_poll_fds(pfds);
    
    int res = poll(pfds.data(),pfds.size(),1000);
    if (res > 0)
    {
      server.process();

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

void test_cclhttp_all()
{/*{{{*/
  cclsys::pid_c server{strings_t{"./cclhttp_test","server"}};
  cclsys::sleep(100);

  int count = 5;
  do {
    cclsys::pid_c client{strings_t{"curl","127.0.0.1:8888/data"}};
    assert(client.wait(0) == 0);
  } while(--count > 0);

  cclsys::pid_c client{strings_t{"wget","http://127.0.0.1:8888/file","-O","Testing/Temporary/track.gpx"}};
  assert(client.wait(0) == 0);
  assert(std::system("diff Testing/Temporary/track.gpx ../tests/data/cclhttp/track.gpx") == 0);

  assert(kill(server.pid(),SIGTERM) == 0);
  assert(server.wait(0) == 0);
}/*}}}*/

void test_cclhttp_auth_server()
{/*{{{*/
  cclsys::install_signals(signal_handler);

  cclhttp::server_c server{8888,auth_server_callback,server_completed};
  std::vector<pollfd> pfds;

  do {
    pfds.clear();
    server.retrieve_poll_fds(pfds);
    
    int res = poll(pfds.data(),pfds.size(),1000);
    if (res > 0)
    {
      server.process();

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

void test_cclhttp_auth_all()
{/*{{{*/
  cclsys::pid_c server{strings_t{"./cclhttp_test","auth_server"}};
  cclsys::sleep(100);

  int count = 5;
  do {
    cclsys::pid_c client{strings_t{"curl","--digest","--user","jirka:password","127.0.0.1:8888"}};
    assert(client.wait(0) == 0);
  } while(--count > 0);

  {
    assert(std::system("curl --digest --user jirka:password 127.0.0.1:8888 > Testing/Temporary/auth_granted.html") == 0);
    assert(std::system("diff Testing/Temporary/auth_granted.html ../tests/data/cclhttp/auth_granted.html") == 0);
  }

  {
    assert(std::system("curl --digest --user logout:logout 127.0.0.1:8888 > Testing/Temporary/auth_denied.html") == 0);
    assert(std::system("diff Testing/Temporary/auth_denied.html ../tests/data/cclhttp/auth_denied.html") == 0);
  }

  // - test by real wget (no busybox) -
  //{
  //  cclsys::pid_c client{strings_t{"wget","--user=logout","--password=logout","127.0.0.1:8888","-O","Testing/Temporary/auth_denied.html"}};

  //  int status = client.wait(0);
  //  assert(WEXITSTATUS(status) == 6);
  //}

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
        test_cclhttp_server();
      }
      else if (std::string("all") == argv[arg_idx])
      {
        test_cclhttp_all();
      }
      else if (std::string("auth_server") == argv[arg_idx])
      {
        test_cclhttp_auth_server();
      }
      else if (std::string("auth_all") == argv[arg_idx])
      {
        test_cclhttp_auth_all();
      }
      else {
        // - ERROR -
        throw std::runtime_error{"unrecognized argument"};
      }
    } while(++arg_idx < argc);
  }

  return 0;
}/*}}}*/

