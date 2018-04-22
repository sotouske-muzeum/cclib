
#include <cassert>

#include "cclstr.h"

void test_cclstr_read_file()
{/*{{{*/
  std::string string;
  assert(cclstr::read_file(CMAKE_CCLIB_DIR "/tests/data/cclstr/track.gpx",string));

  string = cclstr::read_file(CMAKE_CCLIB_DIR "/tests/data/cclstr/track.gpx");
  assert(cclstr::read_file("non-existing") == std::string{});
}/*}}}*/

void test_cclstr_split()
{/*{{{*/
  typedef std::vector<std::string> tokens_t;
  tokens_t tokens;

  cclstr::split("hello-cruel-world","-",tokens);
  assert((tokens == tokens_t{"hello","cruel","world"}));
  assert((cclstr::split("hello-cruel-world","-") == tokens_t{"hello","cruel","world"}));

  assert((cclstr::split("----","-") == tokens_t{"","","","",""}));
  assert((cclstr::split("----","--") == tokens_t{"","",""}));
  assert((cclstr::split("----","----") == tokens_t{"",""}));

  assert((cclstr::split("","----") == tokens_t{""}));

  assert((cclstr::split("abcde","abc") == tokens_t{"","de"}));
  assert((cclstr::split("abcde","cde") == tokens_t{"ab",""}));
}/*}}}*/

void test_cclstr_join()
{/*{{{*/
  std::string string = cclstr::read_file(CMAKE_CCLIB_DIR "/tests/data/cclstr/track.gpx");
  auto split = cclstr::split(string,"\n");
  assert(split.size() == 1449);
  assert(cclstr::join(split,"\n") == string);
}/*}}}*/

void test_cclstr_format()
{/*{{{*/
  assert(cclstr::format("Hello: %d %f %s",120,120.55,"String") ==
    "Hello: 120 120.550000 String");

  std::string string(cclstr::format("Hello: %d %f %s",120,120.55,"String"));
  assert(string.capacity() == 128);

  std::string data(cclstr::read_file(CMAKE_CCLIB_DIR "/tests/data/cclstr/track.gpx"));
  assert((cclstr::format("Data: %s",data.data())) == ("Data: " + data));
}/*}}}*/

void test_cclstr_replace()
{/*{{{*/
  assert(cclstr::replace("Hello world","l","xx") == "Hexxxxo worxxd");
  assert(cclstr::replace("Hello world","","xx") == "Hello world");
  assert(cclstr::replace("Hello world","Hello","") == " world");
  assert(cclstr::replace("Hello world","world","xx") == "Hello xx");
  assert(cclstr::replace("Hello world","o"," O ") == "Hell O  w O rld");
}/*}}}*/

void test_cclstr_regex()
{/*{{{*/
  cclstr::regex_c("abcde"); // NOLINT

  bool thrown = false;
  try {
    cclstr::regex_c("***");
  }
  catch (std::exception &e)
  {
    thrown = true;
  }

  assert(thrown);

  assert(cclstr::regex_c{"bb*"}.match("abbbc"));
  assert(cclstr::regex_c{"^abbbc$"}.match("abbbc"));

  regmatch_t regmatch;
  assert(cclstr::regex_c{"bb*"}.match("abbbc",regmatch));
  assert(regmatch.rm_so == 1 && regmatch.rm_eo == 4);

  std::string match_str;
  assert(cclstr::regex_c{"bb*"}.match("abbbc",match_str));
  assert(match_str == "bbb");

  {
    typedef std::vector<regmatch_t> matches_t;
    matches_t matches;
    assert(cclstr::regex_c{"\\(aa*\\)\\(bb*\\)\\(cc*\\)"}.match_sub("xxxabcyyy",4,matches));
  }

  {
    typedef std::vector<std::string> matches_t;
    matches_t matches;
    assert(cclstr::regex_c{"\\(aa*\\)\\(bb*\\)\\(cc*\\)"}.match_sub("xxxabcyyy",4,matches));
    assert((matches == matches_t{"abc","a","b","c"}));
    assert((cclstr::regex_c{"\\(aa*\\)\\(bb*\\)\\(cc*\\)"}.match_sub("xxxabcyyy",4) == matches_t{"abc","a","b","c"}));
  }

  typedef std::vector<std::string> tokens_t;
  tokens_t tokens;
  cclstr::regex_c{"bb*"}.split("abbbcbbbdbbbe",tokens);
  assert((tokens == tokens_t{"a","c","d","e"}));

  assert((cclstr::regex_c{"bb*"}.split("abbbc") == tokens_t{"a","c"}));
  assert((cclstr::regex_c{"bb*"}.split("") == tokens_t{""}));
  assert((cclstr::regex_c{"-"}.split("hello-cruel-world") ==
        tokens_t{"hello","cruel","world"}));

  assert((cclstr::regex_c{"bb*"}.replace("abbbcbbbdbbbe","XX") == "aXXcXXdXXe"));
  assert((cclstr::regex_c{"bb*"}.replace("abbbcbbbdbbbe","bbb") == "abbbcbbbdbbbe"));
  assert((cclstr::regex_c{"cb*d"}.replace("abbbcbbbdbbbe","XX") == "abbbXXbbbe"));
  assert((cclstr::regex_c{"cb*d"}.replace("abbbcbbbdbbbe","cbd") == "abbbcbdbbbe"));
  assert((cclstr::regex_c{"w.*d"}.replace("Hello world!","user") == "Hello user!"));
}/*}}}*/

void test_cclstr_all()
{/*{{{*/
  test_cclstr_read_file();
  test_cclstr_split();
  test_cclstr_join();
  test_cclstr_format();
  test_cclstr_replace();
  test_cclstr_regex();
}/*}}}*/

int main(int argc,char **argv)
{/*{{{*/
  if (argc > 1)
  {
    int arg_idx = 1;
    do {
      if (std::string("read_file") == argv[arg_idx])
      {
        test_cclstr_read_file();
      }
      else if (std::string("split") == argv[arg_idx])
      {
        test_cclstr_split();
      }
      else if (std::string("join") == argv[arg_idx])
      {
        test_cclstr_join();
      }
      else if (std::string("format") == argv[arg_idx])
      {
        test_cclstr_format();
      }
      else if (std::string("replace") == argv[arg_idx])
      {
        test_cclstr_replace();
      }
      else if (std::string("regex") == argv[arg_idx])
      {
        test_cclstr_regex();
      }
      else if (std::string("all") == argv[arg_idx])
      {
        test_cclstr_all();
      }
      else
      {
        // - ERROR -
        throw std::runtime_error{"unrecognized argument"};
      }
    } while(++arg_idx < argc);
  }

  return 0;
}/*}}}*/

