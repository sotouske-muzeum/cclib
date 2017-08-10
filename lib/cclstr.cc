
#include "cclstr.h"

namespace cclstr {

bool read_file(const std::string &a_file_name,std::string &a_string)
{/*{{{*/
  std::ifstream in_stream(a_file_name,std::ifstream::in);

  // - check if stream was opened -
  if (!in_stream.is_open())
  {
    return false;
  }

  in_stream.seekg(0,std::ios::end);   
  a_string.reserve(in_stream.tellg());
  in_stream.seekg(0,std::ios::beg);

  a_string.assign((std::istreambuf_iterator<char>(in_stream)),std::istreambuf_iterator<char>());

  return true;
}/*}}}*/

std::string read_file(const std::string &a_file_name)
{/*{{{*/
  std::string string;

  if (!read_file(a_file_name,string))
  {
    return std::string{};
  }

  return string;
}/*}}}*/

const std::string &split(const std::string &a_string,const std::string &a_delim,std::vector<std::string> &a_tokens)
{/*{{{*/
  a_tokens.clear();

  size_t last_pos = 0;
  size_t pos;

  // - for each delimiter position -
  while (std::string::npos != (pos = a_string.find(a_delim,last_pos)))
  {
    // - insert token -
    a_tokens.emplace_back(a_string,last_pos,pos - last_pos);

    // - store last position -
    last_pos = pos + a_delim.length();
  }

  // - insert last token -
  a_tokens.emplace_back(a_string,last_pos);

  return a_string;
}/*}}}*/

std::vector<std::string> split(const std::string &a_string,const std::string &a_delim)
{/*{{{*/
  std::vector<std::string> tokens;
  split(a_string,a_delim,tokens);
  return tokens;
}/*}}}*/

std::string join(const std::vector<std::string> &a_tokens,const std::string &a_delim)
{/*{{{*/
  std::string result;

  if (!a_tokens.empty())
  {
    // - adjust result string size -
    size_t result_length = a_delim.length()*(a_tokens.size() - 1);
    
    // - retrieve result string size -
    for (auto token_i = a_tokens.begin();
              token_i != a_tokens.end();
            ++token_i)
    {
      result_length += token_i->length();
    }

    // - reserve result string space -
    result.reserve(result_length);

    auto iter = a_tokens.begin();
    auto iter_end = a_tokens.end();

    // - copy first string -
    result += *iter;

    while (++iter != iter_end)
    {
      result += a_delim;
      result += *iter;
    }
  }

  return result;
}/*}}}*/

std::string format(const char *a_format,...) // NOLINT
{/*{{{*/
  va_list ap;

  std::string result;
  result.resize(128);

  va_start(ap,a_format);
  int fmt_length = vsnprintf(&result[0],result.length() + 1,a_format,ap);
  va_end(ap);

  if (fmt_length > static_cast<int>(result.length()))
  {
    result.resize(fmt_length);

    va_start(ap,a_format);
    vsnprintf(&result[0],result.length() + 1,a_format,ap);
    va_end(ap);
  }
  else
  {
    result.resize(fmt_length);
  }

  return result;
}/*}}}*/

std::string replace(const std::string &a_source,const std::string &a_old,const std::string &a_new)
{/*{{{*/
  if (a_old.empty())
  {
    return a_source;
  }

  std::string result;

  size_t pos = 0;
  do {
    size_t old_pos = pos;
    pos = a_source.find(a_old,old_pos);

    if (pos == std::string::npos)
    {
      pos = a_source.length();
    }

    // - append part of original string to result -
    result += a_source.substr(old_pos,pos - old_pos);

    if (pos >= a_source.length())
    {
      break;
    }

    // - append new string to result -
    result += a_new;

    // - jump over substring -
    pos += a_old.length();

  } while(true);

  return result;
}/*}}}*/

regex_c::regex_c(const std::string &a_string)
{/*{{{*/
  if (a_string.length() <= 0 ||
      regcomp(&m_regex,a_string.data(),0) != 0)
  {
    cclthrow(error_REGEX_INVALID_EXPRESSION_STRING);
  }
}/*}}}*/

regex_c::~regex_c()
{/*{{{*/
  regfree(&m_regex);
}/*}}}*/

bool regex_c::match(const std::string &a_string)
{/*{{{*/
  regmatch_t regmatch;
  return regexec(&m_regex,a_string.data(),1,&regmatch,0) == 0;
}/*}}}*/

bool regex_c::match(const std::string &a_string,regmatch_t &a_match)
{/*{{{*/
  return regexec(&m_regex,a_string.data(),1,&a_match,0) == 0;
}/*}}}*/

bool regex_c::match(const std::string &a_string,std::string &a_match)
{/*{{{*/
  regmatch_t regmatch;

  if (!match(a_string,regmatch))
  {
    return false;
  }

  a_match.assign(a_string,regmatch.rm_so,regmatch.rm_eo - regmatch.rm_so);

  return true;
}/*}}}*/

bool regex_c::match_sub(const std::string &a_string,size_t a_nmatch,std::vector<regmatch_t> &a_matches)
{/*{{{*/
  if (a_nmatch < 1)
  {
    cclthrow(error_REGEX_WRONG_MATCH_COUNT);
  }

  a_matches.resize(a_nmatch);

  return regexec(&m_regex,a_string.data(),a_nmatch,a_matches.data(),0) == 0;
}/*}}}*/

bool regex_c::match_sub(const std::string &a_string,size_t a_nmatch,std::vector<std::string> &a_matches)
{/*{{{*/
  std::vector<regmatch_t> matches;

  if (!match_sub(a_string,a_nmatch,matches))
  {
    return false;
  }

  a_matches.clear();

  for (auto match_i = matches.begin();
            match_i != matches.end();
          ++match_i)
  {
    a_matches.emplace_back(a_string,match_i->rm_so,match_i->rm_eo - match_i->rm_so);
  }

  return true;
}/*}}}*/

std::vector<std::string> regex_c::match_sub(const std::string &a_string,size_t a_nmatch)
{/*{{{*/
  std::vector<std::string> matches;
  match_sub(a_string,a_nmatch,matches);
  return matches;
}/*}}}*/

regex_c &regex_c::split(const std::string &a_string,std::vector<std::string> &a_tokens)
{/*{{{*/
  a_tokens.clear();

  regmatch_t regmatch;
  size_t last_pos = 0;

  // - for each delimiter position -
  while (regexec(&m_regex,&a_string[last_pos],1,&regmatch,0) == 0)
  {
    // - insert token -
    a_tokens.emplace_back(a_string,last_pos,regmatch.rm_so);

    // - store last position -
    last_pos += regmatch.rm_eo;
  }

  // - insert last token -
  a_tokens.emplace_back(a_string,last_pos);

  return *this;
}/*}}}*/

std::vector<std::string> regex_c::split(const std::string &a_string)
{/*{{{*/
  std::vector<std::string> tokens;
  split(a_string,tokens);
  return tokens;
}/*}}}*/

std::string regex_c::replace(const std::string &a_source,const std::string &a_new)
{/*{{{*/
  std::string result;
  regmatch_t regmatch;

  size_t pos = 0;
  do {
    size_t old_pos = pos;

    // - search for substring -
    if (regexec(&m_regex,&a_source[pos],1,&regmatch,0) == 0)
    {
      pos += regmatch.rm_so;
    }
    else
    {
      pos = a_source.length();
    }

    // - append part of original string to result -
    result += a_source.substr(old_pos,pos - old_pos);

    if (pos >= a_source.length())
    {
      break;
    }

    // - append new string to result -
    result += a_new;

    // - jump over substring -
    pos += regmatch.rm_eo - regmatch.rm_so;

  } while(true);

  return result;
}/*}}}*/

} // namespace cclstr

