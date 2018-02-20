
#include "cclsys.h"

#include <cassert>

namespace cclsys {

void install_signals(signal_handler_t a_handler)
{/*{{{*/

  // - create signal ignore action -
  struct sigaction ignore_action;
  ignore_action.sa_handler = SIG_IGN;
  sigemptyset(&ignore_action.sa_mask);
  ignore_action.sa_flags = 0;

  // - ignore SIGPIPE signal -
  sigaction(SIGPIPE,&ignore_action,nullptr);

  // - create signal handler action -
  struct sigaction signal_action;
  signal_action.sa_handler = a_handler;
  sigemptyset(&signal_action.sa_mask);
  signal_action.sa_flags = 0;

  struct sigaction old_action;
  std::vector<int> signals{SIGINT,SIGTERM,SIGHUP,SIGCHLD,SIGUSR1,SIGALRM};

  // - set signal handlers -
  for (auto sig_i = signals.begin();
            sig_i != signals.end();
          ++sig_i)
  {
    sigaction(*sig_i,nullptr,&old_action);
    if (old_action.sa_handler != SIG_IGN)
    {
      sigaction(*sig_i,&signal_action,nullptr);
    }
  }
}/*}}}*/

pid_t execute(const std::vector<std::string> &a_args)
{/*{{{*/
  if (a_args.empty())
  {
    cclthrow(error_SYSTEM_MISSING_PROGRAM_NAME);
  }

  pid_t pid = fork();
  if (pid == -1)
  {
    cclthrow(error_SYSTEM_CANNOT_CREATE_NEW_PROCESS);
  }

  // - process is child -
  if (pid == 0)
  {
    char *arguments[a_args.size() + 1];
    char **a_ptr = arguments;

    // - initialize argument list -
    for (auto arg_i = a_args.begin();
              arg_i != a_args.end();
            ++arg_i)
    {
      *a_ptr++ = const_cast<char *>(arg_i->data());
    }

    // - add terminating argument -
    *a_ptr = nullptr;

    // - execute target process -
    if (execvp(a_args[0].data(),arguments) == -1)
    {
      exit(0);
    }
  }

  return pid;
}/*}}}*/

int64_t sleep(int64_t a_delay)
{/*{{{*/
  struct timespec delay_ts;
  delay_ts.tv_sec = a_delay / 1000;
  delay_ts.tv_nsec = (a_delay % 1000) * 1000000L;

  struct timespec rest_ts;
  if (nanosleep(&delay_ts,&rest_ts) == 0)
  {
    return 0;
  }

  return (static_cast<int64_t>(rest_ts.tv_sec) * 1000) + (rest_ts.tv_nsec / 1000);
}/*}}}*/

int64_t file_size(const std::string &a_file_name)
{/*{{{*/

  // - ERROR -
  struct stat st;
  if (stat(a_file_name.data(),&st) != 0)
  {
    cclthrow(error_SYSTEM_FILE_DOES_NOT_EXIST);
  }

  // - retrieve file size -
  return st.st_size;
}/*}}}*/

std::string hostname()
{/*{{{*/

  // - ERROR -
  char hname[256];
  if (gethostname(hname,256) != 0)
  {
    cclthrow(error_SYSTEM_HOSTNAME_ERROR);
  }

  return hname;
}/*}}}*/

void setenv(const std::string &a_name,const std::string &a_value)
{/*{{{*/

  // - ERROR -
  if (::setenv(a_name.data(),a_value.data(),1) != 0)
  {
    cclthrow(error_SYSTEM_SETENV_ERROR);
  }
}/*}}}*/

std::string getenv(const std::string &a_name)
{/*{{{*/
  char *value = ::getenv(a_name.data());

  // - ERROR -
  if (value == nullptr)
  {
    cclthrow(error_SYSTEM_GETENV_ERROR);
  }

  return value;
}/*}}}*/

pid_c::~pid_c()
{/*{{{*/
  if (m_pid != -1)
  {
    // - check if process was terminated -
    int status;
    if (waitpid(m_pid,&status,WNOHANG) <= 0)
    {
      // - send terminate signal to process -
      if (kill(m_pid,SIGTERM) == 0)
      {
        // - wait on process to terminate -
        waitpid(m_pid,&status,0);
      }
    }
  }
}/*}}}*/

void pid_c::terminate(int a_signum)
{/*{{{*/

  // - check if process was terminated -
  int status;
  if (waitpid(m_pid,&status,WNOHANG) <= 0)
  {
    // - send terminate/kill signal to process -
    if (kill(m_pid,a_signum) == 0)
    {
      // - wait on process to terminate -
      waitpid(m_pid,&status,0);

      // - reset process identifier -
      m_pid = -1;
    }
  }
}/*}}}*/

int pid_c::wait(int options)
{/*{{{*/
  int status;
  waitpid(m_pid,&status,options);

  // - reset process identifier -
  m_pid = -1;

  return status;
}/*}}}*/

poll_c &poll_c::insert(int a_fd,int16_t a_events)
{/*{{{*/
  m_pfd_set.insert({a_fd,a_events,0});
  m_set_changed = true;

  return *this;
}/*}}}*/

poll_c &poll_c::update(int a_fd,int16_t a_events)
{/*{{{*/
  auto iter = m_pfd_set.find({a_fd,0,0});
  assert(iter != m_pfd_set.end());

  const_cast<struct pollfd &>(*iter).events = a_events;
  m_set_changed = true;

  return *this;
}/*}}}*/

poll_c &poll_c::remove(int a_fd)
{/*{{{*/
  auto res = m_pfd_set.erase({a_fd,0,0});
  assert(res == 1);

  m_set_changed = true;

  return *this;
}/*}}}*/

poll_c &poll_c::process_changes()
{/*{{{*/
  m_pfd_array.resize(m_pfd_set.size());
  std::copy(m_pfd_set.begin(),m_pfd_set.end(),m_pfd_array.begin());

  m_set_changed = false;

  return *this;
}/*}}}*/

bool poll_c::ready(int a_timeout,int &a_result)
{/*{{{*/
  if (m_set_changed)
  {
    process_changes();
  }

  // - wait on events on file descriptors -
  a_result = poll(m_pfd_array.data(),m_pfd_array.size(),a_timeout);

  // - ERROR -
  return a_result > 0;
}/*}}}*/

std::ostream &operator << (std::ostream &a_out,poll_c &a_this)
{/*{{{*/
  if (a_this.m_set_changed)
  {
    a_this.process_changes();
  }

  a_out << '{';

  auto &pfd_array = a_this.m_pfd_array;
  for (auto pollfd_i = pfd_array.begin();
            pollfd_i != pfd_array.end();
          ++pollfd_i)
  {
    a_out << '{' << pollfd_i->fd << ',' << pollfd_i->events << ',' << pollfd_i->revents << "},";
  }

  a_out << '}';

  return a_out;
}/*}}}*/

timer_c::timer_c(clockid_t a_clock_id)
{/*{{{*/

  // - ERROR -
  if (timer_create(a_clock_id,nullptr,&m_timer) == -1)
  {
    cclthrow(error_TIMER_CREATE_ERROR);
  }
}/*}}}*/

timer_c::~timer_c()
{/*{{{*/
  timer_delete(m_timer);
}/*}}}*/

timer_c &timer_c::set_value(int64_t a_value)
{/*{{{*/
  time_t sec = a_value/1000000000LL;
  long nsec = a_value%1000000000LL; // NOLINT

  struct itimerspec timer_vals = {
    {0,0},
    {sec,nsec}
  };

  // - ERROR -
  if (timer_settime(m_timer,0,&timer_vals,nullptr) == -1)
  {
    cclthrow(error_TIMER_SETTIME_ERROR);
  }

  return *this;
}/*}}}*/

timer_c &timer_c::set_interval(int64_t a_value)
{/*{{{*/
  time_t sec = a_value/1000000000LL;
  long nsec = a_value%1000000000LL; // NOLINT

  struct itimerspec timer_vals = {
    {sec,nsec},
    {sec,nsec}
  };

  // - ERROR -
  if (timer_settime(m_timer,0,&timer_vals,nullptr) == -1)
  {
    cclthrow(error_TIMER_SETTIME_ERROR);
  }

  return *this;
}/*}}}*/

timer_c &timer_c::reset()
{/*{{{*/
  struct itimerspec timer_vals = {
    {0,0},
    {0,0}
  };

  // - ERROR -
  if (timer_settime(m_timer,0,&timer_vals,nullptr) == -1)
  {
    cclthrow(error_TIMER_SETTIME_ERROR);
  }

  return *this;
}/*}}}*/

void stream_c::write(const std::string &a_data)
{/*{{{*/

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_STREAM_NOT_OPENED);
  }

  // - ERROR -
  if (fwrite(a_data.data(),1,a_data.length(),m_file) != a_data.length())
  {
    cclthrow(error_STREAM_WRITE_ERROR);
  }
}/*}}}*/

void stream_c::flush()
{/*{{{*/

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_STREAM_NOT_OPENED);
  }

  if (fflush(m_file) != 0)
  {
    cclthrow(error_STREAM_FLUSH_ERROR);
  }
}/*}}}*/

std::string stream_c::read()
{/*{{{*/

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_STREAM_NOT_OPENED);
  }

  std::string result;

  const size_t c_buffer_add = 1024;
  size_t read_cnt;
  do {
    size_t old_used = result.size();
    result.resize(result.size() + c_buffer_add);
    read_cnt = fread(&result[old_used],1,c_buffer_add,m_file);
  } while(read_cnt >= c_buffer_add);

  // - ERROR -
  if (ferror(m_file) != 0)
  {
    cclthrow(error_STREAM_READ_ERROR);
  }

  result.resize((result.size() - c_buffer_add) + read_cnt);

  return result;
}/*}}}*/

bool stream_c::readln(std::string &a_line)
{/*{{{*/

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_STREAM_NOT_OPENED);
  }

  char *data = nullptr;
  size_t count = 0;
  ssize_t result = getline(&data,&count,m_file);

  if (result == -1)
  {
    // - ERROR -
    if (ferror(m_file) != 0)
    {
      free(data);

      cclthrow(error_STREAM_READ_ERROR);
    }

    a_line.clear();
    free(data);
  }
  else
  {
    a_line.assign(data,result - 1);
    free(data);
  }

  return feof(m_file) == 0;
}/*}}}*/

std::string stream_c::read(size_t a_size)
{/*{{{*/

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_STREAM_NOT_OPENED);
  }

  std::string result;
  result.resize(a_size);

  size_t read_cnt = fread(&result[0],1,a_size,m_file);

  // - ERROR -
  if (read_cnt < a_size)
  {
    cclthrow(error_STREAM_READ_ERROR);
  }

  return result;
}/*}}}*/

int stream_c::get_fd()
{/*{{{*/

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_STREAM_NOT_OPENED);
  }

  return fileno(m_file);
}/*}}}*/

pipe_c::pipe_c(const std::string &a_command,const std::string &a_type)
{/*{{{*/

  // - open file -
  m_file = popen(a_command.data(),a_type.data());

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_PIPE_OPEN_ERROR);
  }
}/*}}}*/

std::string pipe_c::read_close()
{/*{{{*/
  std::string result(read());
  close();

  return result;
}/*}}}*/

void pipe_c::close()
{/*{{{*/

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_PIPE_NOT_OPENED);
  }

  // - ERROR -
  if (pclose(m_file) == -1)
  {
    cclthrow(error_PIPE_CLOSE_ERROR);
  }

  m_file = nullptr;
}/*}}}*/

pipe_c::~pipe_c()
{/*{{{*/

  // - if pipe file pointer is not null -
  if (m_file != nullptr)
  {
    pclose(m_file);
  }
}/*}}}*/

file_c::file_c(const std::string &a_file_name,const std::string &a_mode)
{/*{{{*/

  // - open file -
  m_file = fopen(a_file_name.data(),a_mode.data());

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_FILE_OPEN_ERROR);
  }
}/*}}}*/

file_c::~file_c()
{/*{{{*/

  // - if file pointer is not null or standard stream pointer -
  if (m_file != nullptr && m_file != stdin && m_file != stdout && m_file != stderr)
  {
    fclose(m_file);
  }
}/*}}}*/

file_c &file_c::seek(int64_t a_offset,int a_whence)
{/*{{{*/

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_FILE_NOT_OPENED);
  }

  // - ERROR -
  if (fseek(m_file,a_offset,a_whence) != 0)
  {
    cclthrow(error_FILE_SEEK_ERROR);
  }

  return *this;
}/*}}}*/

int64_t file_c::tell()
{/*{{{*/

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_FILE_NOT_OPENED);
  }

  int64_t result = ftell(m_file);

  // - ERROR -
  if (result < 0)
  {
    cclthrow(error_FILE_TELL_ERROR);
  }

  return result;
}/*}}}*/

void file_c::close()
{/*{{{*/

  // - ERROR -
  if (m_file == nullptr)
  {
    cclthrow(error_FILE_NOT_OPENED);
  }

  // - ERROR -
  if (fclose(m_file) != 0)
  {
    cclthrow(error_FILE_CLOSE_ERROR);
  }

  m_file = nullptr;
}/*}}}*/

std::string file_c::read_close()
{/*{{{*/
  std::string result(read());
  close();

  return result;
}/*}}}*/

void fd_c::write(const std::string &a_data)
{/*{{{*/

  // - ERROR -
  if (m_fd == -1)
  {
    cclthrow(error_FD_NOT_OPENED);
  }

  size_t length = a_data.length();

  if (length > 0)
  {
    ssize_t cnt;
    size_t writed = 0;

    do {

      // - ERROR -
      if ((cnt = ::write(m_fd,&a_data[writed],length - writed)) == -1)
      {
        cclthrow(error_FD_WRITE_ERROR);
      }
    } while((writed += cnt) < length);
  }
}/*}}}*/

std::string fd_c::read()
{/*{{{*/

  // - ERROR -
  if (m_fd == -1)
  {
    cclthrow(error_FD_NOT_OPENED);
  }

  std::string result;

  const ssize_t c_buffer_add = 1024;
  ssize_t read_cnt;
  do {
    ssize_t old_used = result.size();
    result.resize(result.size() + c_buffer_add);
    read_cnt = ::read(m_fd,&result[old_used],c_buffer_add);

    // - ERROR -
    if (read_cnt == -1)
    {
      cclthrow(error_FD_READ_ERROR);
    }
  } while(read_cnt >= c_buffer_add);

  result.resize((result.size() - c_buffer_add) + read_cnt);

  return result;
}/*}}}*/

socket_addr_c::socket_addr_c()
{/*{{{*/
  memset(&m_addr,0,sizeof(sockaddr_in));
}/*}}}*/

socket_addr_c::socket_addr_c(const std::string &a_host,in_port_t a_port)
{/*{{{*/

  // - retrieve host by name address -
  struct hostent *host = gethostbyname(a_host.data());

  // - ERROR -
  if (host == nullptr)
  {
    cclthrow(error_SOCKET_ADDRESS_RESOLVE_ERROR);
  }

  // - fill address structure -
  memcpy(&m_addr.sin_addr.s_addr,host->h_addr,host->h_length);
  m_addr.sin_port = htons(a_port);
  m_addr.sin_family = AF_INET;
}/*}}}*/

std::string socket_addr_c::name() const
{/*{{{*/
  std::string result;
  result.resize(256);

  if (getnameinfo(reinterpret_cast<const sockaddr *>(&m_addr),sizeof(sockaddr_in),&result[0],
      result.length(),nullptr,0,NI_NUMERICHOST | NI_NUMERICSERV) == 0)
  {
    result.resize(strlen(&result[0]));
  }
  else
  {
    result.clear();
  }

  return result;
}/*}}}*/

in_port_t socket_addr_c::port() const
{/*{{{*/
  return ntohs(m_addr.sin_port);
}/*}}}*/

const int socket_c::value_on  = 1;
const int socket_c::value_off = 0;

socket_c::socket_c(int a_domain,int a_type)
{/*{{{*/

  // - ERROR -
  if ((m_fd = socket(a_domain,a_type,0)) == -1)
  {
    cclthrow(error_SOCKET_CREATE_ERROR);
  }
}/*}}}*/

socket_c::~socket_c()
{/*{{{*/

  // - if socket fd is not invalid -
  if (m_fd != -1)
  {
    ::close(m_fd);
  }
}/*}}}*/

socket_c &socket_c::listen(const socket_addr_c &a_addr,int a_queue_len)
{/*{{{*/

  // - ERROR -
  if (m_fd == -1)
  {
    cclthrow(error_SOCKET_NOT_OPENED);
  }

  // - set server socket as reusable -
  int yes = 1;
  setsockopt(m_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));

#if !defined(DISABLE_SO_DOMAIN)
  // - retrieve socket domain -
  int domain;
  socklen_t domain_len = sizeof(int);
  getsockopt(m_fd,SOL_SOCKET,SO_DOMAIN,&domain,&domain_len);

  switch (domain)
  {
  case AF_INET:
  {
#endif
    // - ERROR -
    if (::bind(m_fd,reinterpret_cast<const sockaddr *>(&a_addr.m_addr),sizeof(sockaddr_in)) != 0)
    {
      cclthrow(error_SOCKET_BIND_ERROR);
    }

    // - ERROR -
    if (::listen(m_fd,a_queue_len) != 0)
    {
      cclthrow(error_SOCKET_LISTEN_ERROR);
    }
#if !defined(DISABLE_SO_DOMAIN)
  }
  break;

  // - ERROR -
  default:

    cclthrow(error_SOCKET_UNKNOWN_DOMAIN);
  }
#endif

  return *this;
}/*}}}*/

socket_c::connection_t socket_c::accept()
{/*{{{*/

  // - ERROR -
  if (m_fd == -1)
  {
    cclthrow(error_SOCKET_NOT_OPENED);
  }

#if !defined(DISABLE_SO_DOMAIN)
  // - retrieve socket domain -
  int domain;
  socklen_t domain_len = sizeof(int);
  getsockopt(m_fd,SOL_SOCKET,SO_DOMAIN,&domain,&domain_len);

  switch (domain)
  {
  case AF_INET:
  {
#endif
    socket_addr_c addr;
    socklen_t addr_len = sizeof(addr.m_addr);

    // - ERROR -
    int acc_fd = ::accept(m_fd,reinterpret_cast<sockaddr *>(&addr.m_addr),&addr_len);
    if (acc_fd < 0)
    {
      // - if nonblocking return empty value -
      if (errno == EAGAIN || errno == EWOULDBLOCK)
      {
        return {socket_c(),socket_addr_c()};
      }

      cclthrow(error_SOCKET_ACCEPT_ERROR);
    }

    // - ERROR -
    if (addr_len != sizeof(sockaddr_in))
    {
      cclthrow(error_SOCKET_ADDRESS_UNKNOWN_FORMAT);
    }

    return {socket_c(acc_fd),socket_addr_c(addr)};

#if !defined(DISABLE_SO_DOMAIN)
  }
  break;

  // - ERROR -
  default:

    cclthrow(error_SOCKET_UNKNOWN_DOMAIN);
  }
#endif
}/*}}}*/

socket_c::connection_t socket_c::accept_nonblock()
{/*{{{*/

  // - ERROR -
  if (ioctl(m_fd,FIONBIO,reinterpret_cast<const char *>(&value_on)) != 0)
  {
    cclthrow(error_SOCKET_ACCEPT_ERROR);
  }

  try
  {
    auto result = accept();

    if (ioctl(m_fd,FIONBIO,reinterpret_cast<const char *>(&value_off)) == 0)
    {
      return result;
    }
  }
  catch (...)
  {
    ioctl(m_fd,FIONBIO,reinterpret_cast<const char *>(&value_off));

    throw;
  }

  // - ERROR -
  cclthrow(error_SOCKET_ACCEPT_ERROR);
}/*}}}*/

socket_c &socket_c::connect(const socket_addr_c &a_addr)
{/*{{{*/

  // - ERROR -
  if (m_fd == -1)
  {
    cclthrow(error_SOCKET_NOT_OPENED);
  }

#if !defined(DISABLE_SO_DOMAIN)
  // - retrieve socket domain -
  int domain;
  socklen_t domain_len = sizeof(int);
  getsockopt(m_fd,SOL_SOCKET,SO_DOMAIN,&domain,&domain_len);

  switch (domain)
  {
  case AF_INET:
  {
#endif
    // - ERROR -
    if (::connect(m_fd,reinterpret_cast<const sockaddr *>(&a_addr.m_addr),sizeof(sockaddr_in)) != 0)
    {
      cclthrow(error_SOCKET_CONNECT_ERROR);
    }
#if !defined(DISABLE_SO_DOMAIN)
  }
  break;

  // - ERROR -
  default:

    cclthrow(error_SOCKET_UNKNOWN_DOMAIN);
  }
#endif

  return *this;
}/*}}}*/

socket_c &socket_c::bind(const socket_addr_c &a_addr)
{/*{{{*/

  // - ERROR -
  if (m_fd == -1)
  {
    cclthrow(error_SOCKET_NOT_OPENED);
  }

#if !defined(DISABLE_SO_DOMAIN)
  // - retrieve socket domain -
  int domain;
  socklen_t domain_len = sizeof(int);
  getsockopt(m_fd,SOL_SOCKET,SO_DOMAIN,&domain,&domain_len);

  switch (domain)
  {
  case AF_INET:
  {
#endif
    // - ERROR -
    if (::bind(m_fd,reinterpret_cast<const sockaddr *>(&a_addr.m_addr),sizeof(sockaddr_in)) != 0)
    {
      cclthrow(error_SOCKET_BIND_ERROR);
    }
#if !defined(DISABLE_SO_DOMAIN)
  }
  break;

  // - ERROR -
  default:

    cclthrow(error_SOCKET_UNKNOWN_DOMAIN);
  }
#endif

  return *this;
}/*}}}*/

socket_c &socket_c::sendto(const socket_addr_c &a_addr,const std::string &a_data)
{/*{{{*/

  // - ERROR -
  if (m_fd == -1)
  {
    cclthrow(error_SOCKET_NOT_OPENED);
  }

  ssize_t length = a_data.length();

  if (length > 0)
  {
    ssize_t cnt;
    ssize_t writed = 0;

    do {
      // - ERROR -
      if ((cnt = ::sendto(m_fd,&a_data[writed],length - writed,0,
          reinterpret_cast<const sockaddr *>(&a_addr.m_addr),sizeof(sockaddr_in))) == -1)
      {
        cclthrow(error_SOCKET_SENDTO_ERROR);
      }
    } while((writed += cnt) < length);
  }

  return *this;
}/*}}}*/

socket_c::udp_data_t socket_c::recvfrom()
{/*{{{*/

  // - ERROR -
  if (m_fd == -1)
  {
    cclthrow(error_SOCKET_NOT_OPENED);
  }

  // - max count of udp message bytes -
  const ssize_t c_udp_max_cnt = 4096;

  // - udp message string -
  std::string data;
  data.resize(c_udp_max_cnt);

  // - udp message source address -
  socket_addr_c addr;
  socklen_t addr_len = sizeof(addr.m_addr);

  // - ERROR -
  ssize_t cnt;
  if ((cnt = ::recvfrom(m_fd,&data[0],data.length(),0,reinterpret_cast<sockaddr *>(&addr.m_addr),&addr_len)) == -1)
  {
    cclthrow(error_SOCKET_RECVFROM_ERROR);
  }

  // - ERROR -
  if (cnt >= c_udp_max_cnt)
  {
    cclthrow(error_SOCKET_UDP_MAX_MSG_SIZE_EXCEEDED);
  }

  data.resize(cnt);

  // - ERROR -
  if (addr_len != sizeof(sockaddr_in))
  {
    cclthrow(error_SOCKET_ADDRESS_UNKNOWN_FORMAT);
  }

  return {data,addr};
}/*}}}*/

void socket_c::close()
{/*{{{*/

  // - ERROR -
  if (m_fd == -1)
  {
    cclthrow(error_SOCKET_NOT_OPENED);
  }

  // - ERROR -
  if (::close(m_fd) != 0)
  {
    cclthrow(error_SOCKET_CLOSE_ERROR);
  }

  m_fd = -1;
}/*}}}*/

socket_c &socket_c::set_timeout(int a_optname,uint64_t a_microsec)
{/*{{{*/

  // - ERROR -
  if (m_fd == -1)
  {
    cclthrow(error_SOCKET_NOT_OPENED);
  }

  struct timeval timeout;
  timeout.tv_sec = a_microsec / 1000000;
  timeout.tv_usec = a_microsec % 1000000;

  // - ERROR -
  if (setsockopt(m_fd,SOL_SOCKET,a_optname,reinterpret_cast<char *>(&timeout),sizeof(timeout)) < 0)
  {
    cclthrow(error_SOCKET_SET_TIMEOUT_ERROR);
  }

  return *this;
}/*}}}*/

int64_t clock_c::gettime(clockid_t a_id)
{/*{{{*/
  timespec tp;

  // - ERROR -
  if (clock_gettime(a_id,&tp) != 0)
  {
    cclthrow(error_CLOCK_CANNOT_GET_TIME);
  }

  return tp.tv_sec*1000000000LL + tp.tv_nsec;
}/*}}}*/

} // namespace cclsys

