
#include <cassert>

#include "cclstr.h"
#include "cclsys.h"
#include "cclhttp.h"
#include "cclcurl.h"

typedef std::vector<std::string> strings_t;

bool g_terminate = false;
void signal_handler(int signum)
{/*{{{*/
  (void)signum;

  g_terminate = true;
}/*}}}*/

void test_cclcurl_get()
{/*{{{*/
  cclsys::pid_c server{strings_t{"./cclhttp_test","server"}};
  cclsys::sleep(100);

  int count = 5;
  do {
    auto res(cclcurl::GET("127.0.0.1:8888/data"));
    assert(res.data() == "Response created from string");
  } while(--count > 0);

  // - read reference file -
  auto res(cclcurl::GET("127.0.0.1:8888/file"));
  auto ref_file(cclstr::read_file("../tests/data/cclcurl/track.gpx"));
  assert(res.data() == ref_file);

  std::cout << "res.info_string(CURLINFO_EFFECTIVE_URL): " << res.info_string(CURLINFO_EFFECTIVE_URL) << std::endl;
  std::cout << "res.info_long(CURLINFO_RESPONSE_CODE): " << res.info_long(CURLINFO_RESPONSE_CODE) << std::endl;
  std::cout << "res.info_double(CURLINFO_TOTAL_TIME): " << res.info_double(CURLINFO_TOTAL_TIME) << std::endl;

  assert(kill(server.pid(),SIGTERM) == 0);
  assert(server.wait(0) == 0);
}/*}}}*/

void test_cclcurl_put()
{/*{{{*/
  cclsys::pid_c server{strings_t{"./cclhttp_test","server"}};
  cclsys::sleep(100);

  int count = 5;
  do {
    auto res(cclcurl::PUT("127.0.0.1:8888/index.html","Hello server C++ client!!!"));
    assert(res.data() == "<html><body>OK</body></html>");
  } while(--count > 0);

  // - read upload file -
  auto upload_file(cclstr::read_file("../tests/data/cclcurl/track.gpx"));
  auto res(cclcurl::PUT("127.0.0.1:8888/file",upload_file));
  assert(res.data() == "<html><body>OK</body></html>");
  assert(std::system("diff Testing/Temporary/data.txt ../tests/data/cclcurl/track.gpx") == 0);

  std::cout << "res.info_string(CURLINFO_EFFECTIVE_URL): " << res.info_string(CURLINFO_EFFECTIVE_URL) << std::endl;
  std::cout << "res.info_long(CURLINFO_RESPONSE_CODE): " << res.info_long(CURLINFO_RESPONSE_CODE) << std::endl;
  std::cout << "res.info_double(CURLINFO_TOTAL_TIME): " << res.info_double(CURLINFO_TOTAL_TIME) << std::endl;

  assert(kill(server.pid(),SIGTERM) == 0);
  assert(server.wait(0) == 0);
}/*}}}*/

void test_cclcurl_delete()
{/*{{{*/
  cclsys::pid_c server{strings_t{"./cclhttp_test","server"}};
  cclsys::sleep(100);

  int count = 5;
  do {
    auto res(cclcurl::DELETE("127.0.0.1:8888/index.html"));
    assert(res.data() == "<html><body>OK</body></html>");
  } while(--count > 0);

  auto res(cclcurl::DELETE("127.0.0.1:8888/file"));
  assert(res.data() == "<html><body>OK</body></html>");

  std::cout << "res.info_string(CURLINFO_EFFECTIVE_URL): " << res.info_string(CURLINFO_EFFECTIVE_URL) << std::endl;
  std::cout << "res.info_long(CURLINFO_RESPONSE_CODE): " << res.info_long(CURLINFO_RESPONSE_CODE) << std::endl;
  std::cout << "res.info_double(CURLINFO_TOTAL_TIME): " << res.info_double(CURLINFO_TOTAL_TIME) << std::endl;

  assert(kill(server.pid(),SIGTERM) == 0);
  assert(server.wait(0) == 0);
}/*}}}*/

void test_cclcurl_head()
{/*{{{*/
  cclsys::pid_c server{strings_t{"./cclhttp_test","server"}};
  cclsys::sleep(100);

  // - regex to match head response -
  cclstr::regex_c resp_regex{
"^HTTP/1.1 200 OK\r\n"
".*"
"Content-Length: [0-9]\\+\r\n"
"Content-Type: .*\r\n"
"Date: .* GMT\r\n"
"\r\n"
"$"};

  int count = 5;
  do {
    auto res(cclcurl::HEAD("127.0.0.1:8888/data"));
    assert (resp_regex.match(res.data()));
  } while(--count > 0);

  auto res(cclcurl::HEAD("127.0.0.1:8888/file"));
  assert(resp_regex.match(res.data()));

  std::cout << "res.info_string(CURLINFO_EFFECTIVE_URL): " << res.info_string(CURLINFO_EFFECTIVE_URL) << std::endl;
  std::cout << "res.info_long(CURLINFO_RESPONSE_CODE): " << res.info_long(CURLINFO_RESPONSE_CODE) << std::endl;
  std::cout << "res.info_double(CURLINFO_TOTAL_TIME): " << res.info_double(CURLINFO_TOTAL_TIME) << std::endl;

  assert(kill(server.pid(),SIGTERM) == 0);
  assert(server.wait(0) == 0);
}/*}}}*/

void test_cclcurl_all()
{/*{{{*/
  test_cclcurl_get();
  test_cclcurl_put();
  test_cclcurl_delete();
  test_cclcurl_head();
}/*}}}*/

int main(int argc,char **argv)
{/*{{{*/
  if (argc > 1)
  {
    int arg_idx = 1;
    do {
      if (std::string("get") == argv[arg_idx])
      {
        test_cclcurl_get();
      }
      else if (std::string("put") == argv[arg_idx])
      {
        test_cclcurl_put();
      }
      else if (std::string("delete") == argv[arg_idx])
      {
        test_cclcurl_delete();
      }
      else if (std::string("head") == argv[arg_idx])
      {
        test_cclcurl_head();
      }
      else if (std::string("all") == argv[arg_idx])
      {
        test_cclcurl_all();
      }
      else {
        // - ERROR -
        throw std::runtime_error{"unrecognized argument"};
      }
    } while(++arg_idx < argc);
  }

  return 0;
}/*}}}*/

