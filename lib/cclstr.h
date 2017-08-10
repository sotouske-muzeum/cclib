
#ifndef CCLIB_CCLSTR_H
#define CCLIB_CCLSTR_H

#include "cclbase.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include <regex.h>
#include <stdarg.h>

namespace cclstr {

// - exceptions list -
const std::string error_REGEX_INVALID_EXPRESSION_STRING = "invalid regular expression string";
const std::string error_REGEX_WRONG_MATCH_COUNT = "wrong count of regex matches";

bool read_file(const std::string &a_file_name,std::string &a_string);
std::string read_file(const std::string &a_file_name);

const std::string &split(const std::string &a_string,const std::string &a_delim,std::vector<std::string> &a_tokens);
std::vector<std::string> split(const std::string &a_string,const std::string &a_delim);

std::string join(const std::vector<std::string> &a_tokens,const std::string &a_delim);
std::string format(const char *a_format,...);
std::string replace(const std::string &a_source,const std::string &a_old,const std::string &a_new);

class regex_c
{/*{{{*/
  private:
  regex_t m_regex;

  public:
  regex_c(const regex_c &) = delete;
  void operator = (const regex_c &) = delete;
  explicit regex_c(const std::string &a_string);
  ~regex_c();

  bool match(const std::string &a_string);

  bool match(const std::string &a_string,regmatch_t &a_match);
  bool match(const std::string &a_string,std::string &a_match);

  bool match_sub(const std::string &a_string,size_t a_nmatch,std::vector<regmatch_t> &a_matches);
  bool match_sub(const std::string &a_string,size_t a_nmatch,std::vector<std::string> &a_matches);
  std::vector<std::string> match_sub(const std::string &a_string,size_t a_nmatch);

  regex_c &split(const std::string &a_string,std::vector<std::string> &a_tokens);
  std::vector<std::string> split(const std::string &a_string);

  std::string replace(const std::string &a_source,const std::string &a_new);
};/*}}}*/

} // namespace cclstr

#endif

