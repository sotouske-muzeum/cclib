
#ifndef CCLIB_CCLSYS_H
#define CCLIB_CCLSYS_H

#include "cclbase.h"

#include <cstring>
#include <iostream>
#include <set>
#include <vector>

#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

namespace cclsys {

// - exceptions list -
const std::string error_SYSTEM_MISSING_PROGRAM_NAME = "missing name of program to execute";
const std::string error_SYSTEM_CANNOT_CREATE_NEW_PROCESS = "cannot create new process";
const std::string error_SYSTEM_FILE_DOES_NOT_EXIST = "file does not exist";
const std::string error_SYSTEM_HOSTNAME_ERROR = "error while retrieving hostname";
const std::string error_SYSTEM_SETENV_ERROR = "error while setting environment variable";
const std::string error_SYSTEM_GETENV_ERROR = "error while retrieving environment variable";
const std::string error_TIMER_CREATE_ERROR = "error while creating timer";
const std::string error_TIMER_SETTIME_ERROR = "error while setting timer";
const std::string error_STREAM_NOT_OPENED = "stream is not opened";
const std::string error_STREAM_WRITE_ERROR = "error while writing to stream";
const std::string error_STREAM_FLUSH_ERROR = "error while flushing stream";
const std::string error_STREAM_READ_ERROR = "error while reading from stream";
const std::string error_PIPE_OPEN_ERROR = "cannot open pipe";
const std::string error_PIPE_CLOSE_ERROR = "cannot close pipe";
const std::string error_PIPE_NOT_OPENED = "pipe is not opened";
const std::string error_FILE_OPEN_ERROR = "cannot open file";
const std::string error_FILE_CLOSE_ERROR = "cannot close file";
const std::string error_FILE_NOT_OPENED = "file is not opened";
const std::string error_FILE_SEEK_ERROR = "cannot set position (seek) in file";
const std::string error_FILE_TELL_ERROR = "cannot retrieve position in file";
const std::string error_FD_NOT_OPENED = "file descriptor is not opened";
const std::string error_FD_WRITE_ERROR = "error while writing to file descriptor";
const std::string error_FD_READ_ERROR = "error while reading from file descriptor";
const std::string error_SOCKET_ADDRESS_RESOLVE_ERROR = "error while resolving socket address";
const std::string error_SOCKET_CREATE_ERROR = "cannot create socket";
const std::string error_SOCKET_NOT_OPENED = "socket is not opened";
const std::string error_SOCKET_CLOSE_ERROR = "cannot close socket";
const std::string error_SOCKET_BIND_ERROR = "error while binding address to socket";
const std::string error_SOCKET_LISTEN_ERROR = "error while initiating listening";
const std::string error_SOCKET_UNKNOWN_DOMAIN = "unknown (unimplemented) socket domain";
const std::string error_SOCKET_ACCEPT_ERROR = "error while accepting connection";
const std::string error_SOCKET_ADDRESS_UNKNOWN_FORMAT = "unknown socket address format";
const std::string error_SOCKET_CONNECT_ERROR = "error while connecting to host";
const std::string error_SOCKET_SENDTO_ERROR = "error while sending data to address";
const std::string error_SOCKET_RECVFROM_ERROR = "error while receiving data from socket";
const std::string error_SOCKET_UDP_MAX_MSG_SIZE_EXCEEDED = "maximal size of udp message was exceeded";
const std::string error_SOCKET_SET_TIMEOUT_ERROR = "error while setting socket send/receive timeout";
const std::string error_CLOCK_CANNOT_GET_TIME = "cannot get clock time";

typedef void (*signal_handler_t)(int);

void install_signals(signal_handler_t a_handler);
pid_t execute(const std::vector<std::string> &a_args);
int64_t sleep(int64_t a_delay);
int64_t file_size(const std::string a_file_name);
std::string hostname();
void setenv(const std::string a_name,const std::string a_value);
std::string getenv(const std::string a_name);

class pid_c
{/*{{{*/
  private:
  pid_t m_pid;

  public:
  explicit pid_c(int a_pid) : m_pid{a_pid} {}
  pid_c(const pid_c &) = delete;
  pid_c(pid_c &&a_pid) throw() : m_pid{a_pid.m_pid}
  {/*{{{*/
    a_pid.m_pid = -1;
  }/*}}}*/
  void operator = (const pid_c &) = delete;
  pid_c &operator = (pid_c &&a_pid)
  {/*{{{*/
    m_pid = a_pid.m_pid;
    a_pid.m_pid = -1;
    return *this;
  }/*}}}*/
  explicit pid_c(const std::vector<std::string> &a_args)
  {/*{{{*/
    m_pid = execute(a_args);
  }/*}}}*/
  ~pid_c();

  pid_t pid() { return m_pid; }
  void terminate(int a_signum);
  int wait(int options);
};/*}}}*/

class pollfd_compare
{/*{{{*/
  public:
  bool operator() (const pollfd &a_first,const pollfd &a_second) const
  {
    return a_first.fd < a_second.fd;
  }
};/*}}}*/

class poll_c
{/*{{{*/
  typedef std::set<pollfd,pollfd_compare> pollfd_set_t;

  private:
  bool m_set_changed;
  pollfd_set_t m_pfd_set;
  std::vector<pollfd> m_pfd_array;

  public:
  poll_c() : m_set_changed{false} {}
  poll_c(const poll_c &) = delete;
  void operator = (const poll_c &) = delete;

  poll_c &insert(int a_fd,int16_t a_events);
  poll_c &update(int a_fd,int16_t a_events);
  poll_c &remove(int a_fd);

  poll_c &process_changes();
  bool ready(int a_timeout,int &a_result);

  friend std::ostream &operator << (std::ostream &a_out,poll_c &a_this);
};/*}}}*/

class timer_c
{/*{{{*/
  private:
  timer_t m_timer;

  public:
  timer_c(const timer_c &) = delete;
  explicit timer_c(clockid_t a_clock_id);
  void operator = (const timer_c &) = delete;
  ~timer_c();

  timer_c &set_value(int64_t a_value);
  timer_c &set_interval(int64_t a_value);
  timer_c &reset();
};/*}}}*/

class stream_c
{/*{{{*/
  protected:
  FILE *m_file;

  public:
  void write(const std::string &a_data);
  void flush();
  std::string read();
  bool readln(std::string &a_line);
  std::string read(size_t a_size);
  int get_fd();
};/*}}}*/

class pipe_c : public stream_c
{/*{{{*/
  private:
  public:
  pipe_c(const std::string &a_command,const std::string &a_type);
  pipe_c(const pipe_c &) = delete;
  pipe_c(pipe_c &&a_pipe) throw()
  {/*{{{*/
    m_file = a_pipe.m_file;
    a_pipe.m_file = nullptr;
  }/*}}}*/
  void operator = (const pipe_c &) = delete;
  pipe_c &operator = (pipe_c &&a_pipe) throw()
  {/*{{{*/
    m_file = a_pipe.m_file;
    a_pipe.m_file = nullptr;
    return *this;
  }/*}}}*/
  ~pipe_c();

  void close();
  pipe_c &write(const std::string &a_data) { stream_c::write(a_data); return *this; }
  pipe_c &flush() { stream_c::flush(); return *this; }
  std::string read_close();
};/*}}}*/

class file_c : public stream_c
{/*{{{*/
  private:
  public:
  file_c(const std::string &a_file_name,const std::string &a_mode);
  file_c(const file_c &) = delete;
  file_c(file_c &&a_file) throw()
  {/*{{{*/
    m_file = a_file.m_file;
    a_file.m_file = nullptr;
  }/*}}}*/
  void operator = (const file_c &) = delete;
  file_c &operator = (file_c &&a_file) throw()
  {/*{{{*/
    m_file = a_file.m_file;
    a_file.m_file = nullptr;
    return *this;
  }/*}}}*/
  ~file_c();

  file_c &seek(int64_t a_offset,int a_whence);
  int64_t tell();
  void close();
  file_c &write(const std::string &a_data) { stream_c::write(a_data); return *this; }
  file_c &flush() { stream_c::flush(); return *this; }
  std::string read_close();
};/*}}}*/

class fd_c
{/*{{{*/
  protected:
  int m_fd;
  public:
  void write(const std::string &a_data);
  std::string read();
  int get_fd() { return m_fd; }
};/*}}}*/

class socket_addr_c
{/*{{{*/
  friend class socket_c;

  private:
  sockaddr_in m_addr;

  socket_addr_c();
  public:
  socket_addr_c(const std::string &a_host,in_port_t a_port);

  std::string name() const;
  in_port_t port() const;
};/*}}}*/

class socket_c : public fd_c
{/*{{{*/
  typedef std::pair<socket_c,socket_addr_c> connection_t; 
  typedef std::pair<std::string,socket_addr_c> udp_data_t; 

  private:
  static const int value_on;
  static const int value_off;

  socket_c() = default;
  explicit socket_c(int a_fd) { m_fd = a_fd; }

  public:
  socket_c(int a_domain,int a_type);
  socket_c(const socket_c &) = delete;
  socket_c(socket_c &&a_socket) throw()
  {/*{{{*/
    m_fd = a_socket.m_fd;
    a_socket.m_fd = -1;
  }/*}}}*/
  void operator = (const socket_c &) = delete;
  socket_c &operator = (socket_c &&a_socket) throw()
  {/*{{{*/
    m_fd = a_socket.m_fd;
    a_socket.m_fd = -1;
    return *this;
  }/*}}}*/
  ~socket_c();

  socket_c &listen(const socket_addr_c &a_addr,int a_queue_len);
  connection_t accept();
  connection_t accept_nonblock();
  socket_c &connect(const socket_addr_c &a_addr);
  socket_c &bind(const socket_addr_c &a_addr);
  socket_c &sendto(const socket_addr_c &a_addr,const std::string &a_data);
  udp_data_t recvfrom();
  void close();
  socket_c &set_timeout(int a_optname,uint64_t a_microsec);
  socket_c &write(const std::string &a_data) { fd_c::write(a_data); return *this; }
};/*}}}*/

class clock_c
{
  public:
  static int64_t gettime(clockid_t a_id);
};

} // namespace cclsys

#endif

