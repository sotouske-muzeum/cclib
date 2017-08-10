
#include <cassert>

#include "ccljson.h"
#include "cclstr.h"

using cclvar::var_c;
using cclvar::string_t;
using cclvar::array_t;
using cclvar::list_t;
using cclvar::dict_t;

const std::string ref_json
{/*{{{*/
"{\n"
"  \"friends\":[\n"
"    {\n"
"      \"name\":\"Peter\",\n"
"      \"surname\":\"Carlson\"\n"
"    },\n"
"    {\n"
"      \"name\":\"Mike\",\n"
"      \"surname\":\"Olsen\"\n"
"    },\n"
"    {\n"
"      \"name\":\"Luke\",\n"
"      \"surname\":\"Johanson\"\n"
"    }\n"
"  ]\n"
"}"
};/*}}}*/

void test_ccljson_create()
{/*{{{*/
  typedef cclvar::dict_pair_t dp_t;

  assert(ccljson::create(var_c{}) == "null");
  assert(ccljson::create(var_c{true}) == "true");
  assert(ccljson::create(var_c{false}) == "false");
  assert(ccljson::create(var_c{120}) == "120");
  assert(ccljson::create(var_c{120.0}) == "120.000000");
  assert(ccljson::create(var_c{120.25}) == "120.250000");
  assert(ccljson::create(var_c{120.25675}) == "120.256750");
  assert(ccljson::create(var_c{"Hello world!!!"}) == "\"Hello world!!!\"");
  assert(ccljson::create(var_c{array_t{1,2,3,4,5}}) == "[1,2,3,4,5]");
  assert(ccljson::create(var_c{list_t{1,2,3,4,5}}) == "[1,2,3,4,5]");
  assert(ccljson::create(var_c{cclvar::dict_t{
    dp_t{"one",1},
    dp_t{"two",2},
    dp_t{"three",3},
    dp_t{"four",4},
  }}) == "{\"four\":4,\"one\":1,\"three\":3,\"two\":2}");

  assert(ccljson::create_nice(var_c{cclvar::dict_t{
    dp_t{"friends",array_t{
      dict_t{
        dp_t{"name","Peter"},
        dp_t{"surname","Carlson"},
      },
      dict_t{
        dp_t{"name","Mike"},
        dp_t{"surname","Olsen"},
      },
      dict_t{
        dp_t{"name","Luke"},
        dp_t{"surname","Johanson"},
      },
    }},
  }},"  ") == ref_json);
}/*}}}*/

void test_ccljson_parse()
{/*{{{*/
  const std::string ref_json_1
  {/*{{{*/
"{\n"
"  \"friends\":[\n"
"    {\n"
"      \"age\":32,\n"
"      \"height\":127.090000,\n"
"      \"male\":true,\n"
"      \"name\":\"Peter\",\n"
"      \"surname\":\"Carlson\"\n"
"    },\n"
"    {\n"
"      \"age\":28,\n"
"      \"height\":7.090000,\n"
"      \"male\":null,\n"
"      \"name\":\"Mike\",\n"
"      \"surname\":\"Olsen\"\n"
"    },\n"
"    {\n"
"      \"age\":16,\n"
"      \"height\":121.090000,\n"
"      \"male\":false,\n"
"      \"name\":\"Luke\",\n"
"      \"surname\":\"Johanson\"\n"
"    }\n"
"  ]\n"
"}"
  };/*}}}*/

  const std::string ref_str_1 = "[friends:[[age:32,height:127.09,male:true,name:Peter,surname:Carlson],[age:28,height:7.09,male:<blank>,name:Mike,surname:Olsen],[age:16,height:121.09,male:false,name:Luke,surname:Johanson]]]";

  assert(cclvar::to_string(ccljson::parse(ref_json_1)) == ref_str_1);
  assert(ccljson::create_nice(ccljson::parse(ref_json_1),"  ") == ref_json_1);

  ccljson::parse(ref_json);
  assert(ccljson::parse(cclstr::read_file("../tests/data/ccljson/data.json")) ==
         ccljson::parse(cclstr::read_file("../tests/data/ccljson/data1.json")));

  assert(cclvar::to_string(ccljson::parse(cclstr::read_file("../tests/data/ccljson/data.json"))) ==
    cclstr::read_file("../tests/data/ccljson/data.txt"));
}/*}}}*/

void test_ccljson_all()
{/*{{{*/
  test_ccljson_create();
  test_ccljson_parse();
}/*}}}*/

int main(int argc,char **argv)
{/*{{{*/
  if (argc > 1)
  {
    int arg_idx = 1;
    do {
      if (std::string("create") == argv[arg_idx])
      {
        test_ccljson_create();
      }
      else if (std::string("parse") == argv[arg_idx])
      {
        test_ccljson_parse();
      }
      else if (std::string("all") == argv[arg_idx])
      {
        test_ccljson_all();
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

