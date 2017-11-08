
#include <cassert>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "cclstr.h"
#include "cclsys.h"

typedef std::vector<std::string> strings_t;

// - static type assumptions -
static_assert(sizeof(int16_t) == sizeof(short),"unexpected integer size"); // NOLINT
static_assert(sizeof(uint16_t) == sizeof(unsigned short),"unexpected integer size"); // NOLINT

static_assert(sizeof(int32_t) == sizeof(int),"unexpected integer size");
static_assert(sizeof(uint32_t) == sizeof(unsigned),"unexpected integer size");

static_assert(sizeof(int64_t) == sizeof(long long),"unexpected integer size"); // NOLINT
static_assert(sizeof(uint64_t) == sizeof(unsigned long long),"unexpected integer size"); // NOLINT

bool g_terminate = false;
int g_signum;

void signal_handler(int signum)
{/*{{{*/
  g_signum = signum;

  switch (signum)
  {
  case SIGALRM:
  case SIGCHLD:
    break;
  default:
    g_terminate = true;
  }
}/*}}}*/

void test_cclsys_poll()
{/*{{{*/
  cclsys::poll_c poll;
  poll.insert(-1,0);
  poll.insert(-2,0);
  //std::cout << poll << std::endl;
  poll.update(-2,1);
  //std::cout << poll << std::endl;
  poll.remove(-1);
  //std::cout << poll << std::endl;
  poll.remove(-2);

  int fd = open("../tests/data/cclsys/track.gpx",O_RDONLY);
  poll.insert(fd,POLLIN | POLLPRI);

  ssize_t byte_cnt = 0;
  do {
    int result;
    assert(poll.ready(1000,result));
    assert(result > 0);

    char buff[64];
    ssize_t size = read(fd,buff,64);
    byte_cnt += size;

    if (size == 0)
    {
      break;
    }

  } while(true);

  struct stat stat;
  fstat(fd,&stat);
  assert(byte_cnt == stat.st_size);
}/*}}}*/

void test_cclsys_signals()
{/*{{{*/
  cclsys::install_signals(signal_handler);

  auto signums = {SIGINT,SIGTERM,SIGHUP,SIGUSR1,SIGCHLD,SIGALRM};
  for (auto signum_i = signums.begin();signum_i != signums.end();++signum_i)
  {
    g_signum = -1;
    kill(getpid(),*signum_i);
    assert(g_signum == *signum_i);
  }
}/*}}}*/

void test_cclsys_pid()
{/*{{{*/

  // - test execute -
  {
    pid_t pid = cclsys::execute(strings_t{"ls","-l"});
    int status;
    assert(pid == waitpid(pid,&status,0));
  }

  // - test waiting on pid -
  {
    std::vector<cclsys::pid_c> pids;
    pids.emplace_back(strings_t{"ls","-l"});
    pids.emplace_back(strings_t{"grep","-q","60.21057","../tests/data/cclsys/track.gpx"});

    cclsys::sleep(100);
  }
}/*}}}*/

void test_cclsys_timer()
{/*{{{*/
  cclsys::install_signals(signal_handler);

  // - test one shot timer -
  {
    cclsys::timer_c timer{CLOCK_MONOTONIC};
    timer.set_value(250000000);

    g_signum = -1;
    cclsys::sleep(5000);
    assert(g_signum == SIGALRM);
  }

  // - test interval timer -
  {
    cclsys::timer_c timer{CLOCK_MONOTONIC};
    timer.set_interval(100000);

    int count = 0;
    do {
      g_signum = -1;
      cclsys::sleep(5000);
      assert(g_signum == SIGALRM);
    } while(++count < 10);
  }

  // - test reset timer -
  {
    cclsys::timer_c timer{CLOCK_MONOTONIC};
    timer.set_value(250000000);
    timer.reset();

    int64_t start = cclsys::clock_c::gettime(CLOCK_MONOTONIC);
    cclsys::sleep(500);
    int64_t elapsed = cclsys::clock_c::gettime(CLOCK_MONOTONIC) - start;
    assert(elapsed >= 500000000);
  }
}/*}}}*/

void test_cclsys_pipe()
{/*{{{*/
  cclsys::pipe_c pipe{"ls","r"};
  std::cout << pipe.get_fd() << std::endl;
  pipe.read_close();

  cclsys::pipe_c pipe1{"ls","r"};
  pipe1.read();
  pipe1.close();

  cclsys::pipe_c pipe2{"cat","w"};
  pipe2.write("Hello world\n");
  pipe2.flush();
  pipe2.close();

  cclsys::pipe_c{"cat","w"}.write("Hello world\n").close();

  std::string ref_str(cclstr::read_file("../tests/data/cclsys/track.gpx"));

  cclsys::pipe_c pipe3{"cat ../tests/data/cclsys/track.gpx","r"};
  assert(pipe3.read() == ref_str);

  // - read lines from pipe -
  cclsys::pipe_c pipe4{"cat ../tests/data/cclsys/track.gpx","r"};
  std::vector<std::string> lines;
  do {
    lines.emplace_back(std::string());
    if (!pipe4.readln(lines.back()))
    {
      break;
    }
  } while(true);
  pipe4.close();
  assert(cclstr::join(lines,"\n") == ref_str);

  cclsys::pipe_c pipe5{"cat ../tests/data/cclsys/track.gpx","r"};
  assert(pipe5.read(20) == "<?xml version=\"1.0\" ");
  pipe5.read();

  assert((cclsys::pipe_c{"cat ../tests/data/cclsys/track.gpx","r"}.read_close()) ==
      ref_str);
}/*}}}*/

void test_cclsys_file()
{/*{{{*/
  cclsys::file_c file{"../tests/data/cclsys/track.gpx","r"};
  file.seek(256,SEEK_SET);
  assert(file.tell() == 256);
  file.close();

  std::string ref_str(cclsys::file_c("../tests/data/cclsys/track.gpx","r").read_close());

  cclsys::file_c file1{"Testing/Temporary/track.gpx","w"};
  file1.write(ref_str);
  file1.close();
  assert(std::system("diff Testing/Temporary/track.gpx ../tests/data/cclsys/track.gpx") == 0);

  cclsys::file_c{"Testing/Temporary/track.gpx","w"}.write(ref_str).close();
  assert(std::system("diff Testing/Temporary/track.gpx ../tests/data/cclsys/track.gpx") == 0);

  assert(cclsys::file_size("../tests/data/cclsys/track.gpx") == 116403);
}/*}}}*/

void test_cclsys_hostname()
{/*{{{*/
#ifdef MINT_DEVEL_PC
  assert(cclsys::hostname() == "debian-lenovo");
  std::cout << "hostname: " << cclsys::hostname() << std::endl;
#else
  std::cout << "hostname: " << cclsys::hostname() << std::endl;
#endif
}/*}}}*/

void test_cclsys_envvar()
{/*{{{*/
  bool thrown = false;
  try
  {
    cclsys::getenv("NON_EXISTENT");
  }
  catch (...)
  {
    thrown = true;
  }
  assert(thrown);

  std::string var_name = "ENV_NAME";
  std::string var_value = "ENV_VALUE";

  cclsys::setenv(var_name,var_value);
  assert(cclsys::getenv(var_name) == var_value);
}/*}}}*/

void test_cclsys_socket_tcp_server()
{/*{{{*/
  cclsys::install_signals(signal_handler);

  {
    cclsys::socket_c socket{AF_INET,SOCK_STREAM};
    socket.listen({"127.0.0.1",5555},10);

    cclsys::poll_c poll;
    poll.insert(socket.get_fd(),POLLIN | POLLPRI);

    do {
      int res; 
      if (poll.ready(1000,res))
      {
        auto conn = socket.accept();
        auto &acc_sock = conn.first;

        //auto &acc_addr = conn.second;
        //std::cout << "from: " << conn.second.name() << ":" << conn.second.port() << std::endl;

        auto data = acc_sock.read();
        //std::cout << "data: '" << data << '\'' << std::endl;

        acc_sock.write(data);
        acc_sock.close();
      }
      else
      {
        std::cout << "waiting ..." << std::endl;
      }
    } while(!g_terminate);
  }

  std::cout << cclsys::pipe_c{cclstr::format("ls -l /proc/%d/fd",getpid()),"r"}.read();
}/*}}}*/

void test_cclsys_socket_tcp_client()
{/*{{{*/
  int idx = 0;
  do {
    cclsys::socket_c socket{AF_INET,SOCK_STREAM};
    socket.connect({"127.0.0.1",5555});

    auto data = socket
      .write(cclstr::format("Hello from client #%d",idx))
      .read();

    std::cout << "received: " << data << std::endl;

    socket.close();
  } while(++idx < 5);
}/*}}}*/

void test_cclsys_socket_tcp()
{/*{{{*/
  cclsys::pid_c server{strings_t{"./cclsys_test","socket_tcp_server"}};
  cclsys::sleep(100);

  const std::string data_ref =
/*{{{*/
"received: Hello from client #0\n"
"received: Hello from client #1\n"
"received: Hello from client #2\n"
"received: Hello from client #3\n"
"received: Hello from client #4\n"
;/*}}}*/

  int count = 5;
  do {
    auto data = cclsys::pipe_c{"./cclsys_test socket_tcp_client","r"}.read();
    assert(data == data_ref);
  } while(--count > 0);

  assert(kill(server.pid(),SIGTERM) == 0);
  assert(server.wait(0) == 0);
}/*}}}*/

void test_cclsys_socket_udp_recv()
{/*{{{*/
  cclsys::install_signals(signal_handler);

  cclsys::socket_c socket{AF_INET,SOCK_DGRAM};
  socket.bind({"127.0.0.1",5601});

  cclsys::poll_c poll;
  poll.insert(socket.get_fd(),POLLIN | POLLPRI);

  do {
    int res; 
    if (poll.ready(1000,res))
    {
      auto udp_data = socket.recvfrom();
      auto &data = udp_data.first;

      // - terminate message received -
      if (data == "terminate")
      {
        break;
      }

      std::cout << "received: " << data << std::endl;
    }
    else
    {
      std::cout << "waiting ..." << std::endl;
    }
  } while(!g_terminate);

  socket.close();
}/*}}}*/

void test_cclsys_socket_udp_send()
{/*{{{*/
  cclsys::socket_c socket{AF_INET,SOCK_DGRAM};
  socket.bind({"127.0.0.1",5600});

  cclsys::socket_addr_c target_addr{"127.0.0.1",5601};

  int idx = 0;
  do {
    socket.sendto(target_addr,cclstr::format("Hello from client #%d",idx));
  } while(++idx < 5);
}/*}}}*/

void test_cclsys_socket_udp()
{/*{{{*/
  cclsys::pipe_c recv_pipe{"./cclsys_test socket_udp_recv","r"};
  cclsys::sleep(100);

  const std::string data_ref =
/*{{{*/
"received: Hello from client #0\n"
"received: Hello from client #1\n"
"received: Hello from client #2\n"
"received: Hello from client #3\n"
"received: Hello from client #4\n"
;/*}}}*/

  std::string mult_ref;

  mult_ref.reserve(data_ref.length()*5);
  for (int idx = 5;idx-- > 0;)
  {
    mult_ref += data_ref;
  }

  int count = 5;
  do {
    cclsys::pid_c client{strings_t{"./cclsys_test","socket_udp_send"}};
    assert(client.wait(0) == 0);
  } while(--count > 0);

  cclsys::socket_c{AF_INET,SOCK_DGRAM}
    .bind({"127.0.0.1",5600})
    .sendto({"127.0.0.1",5601},"terminate")
    .close();

  assert(recv_pipe.read() == mult_ref);
}/*}}}*/

void test_cclsys_clock()
{/*{{{*/
  std::cout << "CLOCK_REALTIME:           " << cclsys::clock_c::gettime(CLOCK_REALTIME) << std::endl;
  std::cout << "CLOCK_MONOTONIC:          " << cclsys::clock_c::gettime(CLOCK_MONOTONIC) << std::endl;
  std::cout << "CLOCK_PROCESS_CPUTIME_ID: " << cclsys::clock_c::gettime(CLOCK_PROCESS_CPUTIME_ID) << std::endl;
  std::cout << "CLOCK_THREAD_CPUTIME_ID:  " << cclsys::clock_c::gettime(CLOCK_THREAD_CPUTIME_ID) << std::endl;
}/*}}}*/

void test_cclsys_all()
{/*{{{*/
  test_cclsys_poll();
  test_cclsys_signals();
  test_cclsys_pid();
  test_cclsys_timer();
  test_cclsys_pipe();
  test_cclsys_file();
  test_cclsys_hostname();
  test_cclsys_envvar();
  test_cclsys_socket_tcp();
  test_cclsys_socket_udp();
  test_cclsys_clock();
}/*}}}*/

int main(int argc,char **argv)
{/*{{{*/
  if (argc > 1)
  {
    int arg_idx = 1;
    do {
      if (std::string("poll") == argv[arg_idx])
      {
        test_cclsys_poll();
      }
      else if (std::string("signals") == argv[arg_idx])
      {
        test_cclsys_signals();
      }
      else if (std::string("pid") == argv[arg_idx])
      {
        test_cclsys_pid();
      }
      else if (std::string("timer") == argv[arg_idx])
      {
        test_cclsys_timer();
      }
      else if (std::string("pipe") == argv[arg_idx])
      {
        test_cclsys_pipe();
      }
      else if (std::string("file") == argv[arg_idx])
      {
        test_cclsys_file();
      }
      else if (std::string("hostname") == argv[arg_idx])
      {
        test_cclsys_hostname();
      }
      else if (std::string("envvar") == argv[arg_idx])
      {
        test_cclsys_envvar();
      }
      else if (std::string("socket_tcp_server") == argv[arg_idx])
      {
        test_cclsys_socket_tcp_server();
      }
      else if (std::string("socket_tcp_client") == argv[arg_idx])
      {
        test_cclsys_socket_tcp_client();
      }
      else if (std::string("socket_tcp") == argv[arg_idx])
      {
        test_cclsys_socket_tcp();
      }
      else if (std::string("socket_udp_recv") == argv[arg_idx])
      {
        test_cclsys_socket_udp_recv();
      }
      else if (std::string("socket_udp_send") == argv[arg_idx])
      {
        test_cclsys_socket_udp_send();
      }
      else if (std::string("socket_udp") == argv[arg_idx])
      {
        test_cclsys_socket_udp();
      }
      else if (std::string("clock") == argv[arg_idx])
      {
        test_cclsys_clock();
      }
      else if (std::string("all") == argv[arg_idx])
      {
        test_cclsys_all();
      }
      else
      {
        // - ERROR -
        throw std::runtime_error{"unrecognized test argument"};
      }
    } while(++arg_idx < argc);
  }

  return 0;
}/*}}}*/

