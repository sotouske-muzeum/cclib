
#include <cassert>
#include <iostream>
#include <sstream>

#include "cclvar.h"

using cclvar::var_c;
using cclvar::string_t;
using cclvar::array_t;
using cclvar::list_t;
using cclvar::dict_t;

void test_cclvar_blank()
{/*{{{*/
  std::stringbuf buffer;
  std::ostream os(&buffer);

  buffer.str({});
  os << var_c{};
  assert(buffer.str() == "<blank>");

  var_c blank_0{};
  assert(blank_0.type() == cclvar::type_blank);

  buffer.str({});
  os << blank_0;
  assert(buffer.str() == "<blank>");

  var_c blank_1 = blank_0;
  buffer.str({});
  os << blank_1;
  assert(buffer.str() == "<blank>");

  var_c blank_2 = blank_0.copy();
  assert(blank_0 == blank_2);

  bool thrown = false;
  try
  {
    (void)blank_0.to_int();
  }
  catch (...)
  {
    thrown = true;
  }
  assert(thrown);
}/*}}}*/

void test_cclvar_bool()
{/*{{{*/
  assert(var_c{true}.to_bool() == true);
  assert(var_c{false}.to_bool() == false);

  var_c bool_0{true};
  assert(bool_0.type() == cclvar::type_bool);
  assert(bool_0.to_bool());

  std::stringbuf buffer;
  std::ostream os(&buffer);

  buffer.str({});
  os << var_c{true};
  assert(buffer.str() == "true");

  buffer.str({});
  os << var_c{false};
  assert(buffer.str() == "false");

  var_c bool_1 = bool_0.copy();
  assert(bool_0 == bool_1);

  bool thrown = false;
  try
  {
    (void)bool_0.to_float();
  }
  catch (...)
  {
    thrown = true;
  }
  assert(thrown);
}/*}}}*/

void test_cclvar_integer()
{/*{{{*/
  int value = 125;
  assert(var_c{value}.to_int() == value);

  var_c integer_0{value};
  assert(integer_0.type() == cclvar::type_integer);
  assert(integer_0.to_int() == value);

  var_c integer_1 = integer_0.to_int() + 10;
  assert(integer_1.to_int() == (value + 10));

  var_c integer_2 = integer_0.copy();
  assert(integer_0 == integer_2);

  bool thrown = false;
  try
  {
    (void)integer_0.to_float();
  }
  catch (...)
  {
    thrown = true;
  }
  assert(thrown);
}/*}}}*/

void test_cclvar_double()
{/*{{{*/
  double value = 125.5;
  assert(var_c{value}.to_float() == value);

  var_c double_0{value};
  assert(double_0.type() == cclvar::type_float);
  assert(double_0.to_float() == value);

  var_c double_1 = double_0.to_float() + 10;
  assert(double_1.to_float() == (value + 10));

  var_c double_2 = double_0.copy();
  assert(double_0 == double_2);

  bool thrown = false;
  try
  {
    (void)double_0.to_int();
  }
  catch (...)
  {
    thrown = true;
  }
  assert(thrown);
}/*}}}*/

void test_cclvar_string()
{/*{{{*/
  string_t value = "Hello world !!!";
  string_t append_value = "Appended string";

  assert(var_c{value}.to_str() == value);

  var_c string_0{value};
  assert(string_0.type() == cclvar::type_string);
  assert(string_0.to_str() == value);

  var_c string_1 = string_0.to_str() + append_value;
  assert(string_1.to_str() == (value + append_value));

  std::stringbuf buffer;
  std::ostream os(&buffer);
  buffer.str({});
  os << string_0;
  assert(buffer.str() == value);

  var_c string_2 = string_0.copy();
  assert(string_0 == string_2);

  bool thrown = false;
  try
  {
    (void)string_0.to_int();
  }
  catch (...)
  {
    thrown = true;
  }
  assert(thrown);
}/*}}}*/

void test_cclvar_array()
{/*{{{*/
  std::stringbuf buffer;
  std::ostream os(&buffer);

  buffer.str({});
  os << var_c{array_t{1,2,3,4,5}};
  assert(buffer.str() == "[1,2,3,4,5]");

  var_c array_0{array_t{1,2.5,"Hello",array_t{1,2,3},5}};
  assert(array_0.type() == cclvar::type_array);
  buffer.str({});
  os << array_0;
  assert(buffer.str() == "[1,2.5,Hello,[1,2,3],5]");

  assert(array_0[0].to_int() == 1);
  assert(array_0[1].to_float() == 2.5);
  assert(array_0[2].to_str() == "Hello");
  assert(array_0[3].to_array() == (array_t{1,2,3}));

  buffer.str({});
  os << array_0[3];
  assert(buffer.str() == "[1,2,3]");

  array_0[3] = var_c();
  buffer.str({});
  os << array_0;
  assert(buffer.str() == "[1,2.5,Hello,<blank>,5]");

  int idx = 0;
  auto &array_ref = array_0.to_array();
  for (auto value_i = array_ref.begin();
            value_i != array_ref.end();
          ++value_i)
  {
    *value_i = idx++;
  }
  assert(array_0.to_array() == (array_t{0,1,2,3,4}));

  array_0 = array_t{1,2,3,4,array_t{1,2,3}};
  var_c array_1 = array_0.copy();
  assert(array_0 == array_1);

  array_1[4][0] = {};
  assert(array_0 != array_1);

  bool thrown = false;
  try
  {
    (void)array_0.to_int();
  }
  catch (...)
  {
    thrown = true;
  }
  assert(thrown);
}/*}}}*/

void test_cclvar_list()
{/*{{{*/
  std::stringbuf buffer;
  std::ostream os(&buffer);

  buffer.str({});
  os << var_c{list_t{1,2,3,4,5}};
  assert(buffer.str() == "[1,2,3,4,5]");

  var_c list_0{list_t{1,2.5,"Hello",list_t{1,2,3},5}};
  assert(list_0.type() == cclvar::type_list);
  buffer.str({});
  os << list_0;
  assert(buffer.str() == "[1,2.5,Hello,[1,2,3],5]");

  buffer.str({});
  list_t &list_ref = list_0.to_list();
  os << list_ref.front() << ','; list_ref.pop_front();
  os << list_ref.front() << ','; list_ref.pop_front();
  os << list_ref.back() << ','; list_ref.pop_back();
  os << list_ref.back() << ','; list_ref.pop_back();
  os << list_ref.back() << ','; list_ref.pop_back();
  assert(buffer.str() == "1,2.5,5,[1,2,3],Hello,");

  list_ref.push_front(1);
  list_ref.push_front(2);
  list_ref.push_front(3);
  list_ref.push_back(4);
  list_ref.push_back(5);

  buffer.str({});
  os << list_0;
  assert(buffer.str() == "[3,2,1,4,5]");

  int idx = 0;
  for (auto value_i = list_ref.begin();
            value_i != list_ref.end();
          ++value_i)
  {
    *value_i = idx++;
  }
  assert(list_0.to_list() == (list_t{0,1,2,3,4}));

  list_0 = list_t{array_t{1,2,3},1,2,3,4};
  var_c list_1 = list_0.copy();
  assert(list_0 == list_1);

  list_1.to_list().front()[0] = {};
  assert(list_0 != list_1);

  bool thrown = false;
  try
  {
    (void)list_0.to_int();
  }
  catch (...)
  {
    thrown = true;
  }
  assert(thrown);
}/*}}}*/

void test_cclvar_dict()
{/*{{{*/
  typedef cclvar::dict_pair_t dp_t;

  std::stringbuf buffer;
  std::ostream os(&buffer);

  buffer.str({});
  os << var_c{dict_t{
    dp_t{"One",1},
    dp_t{"Two",2},
    dp_t{"Three",3},
    dp_t{"Four",4},
  }};
  assert(buffer.str() == "[Four:4,One:1,Three:3,Two:2]");

  var_c dict_0{dict_t{
    dp_t{"Blank",{}},
    dp_t{"Bool",true},
    dp_t{"Integer",1},
    dp_t{"Double",1.25},
    dp_t{"String","Hello world"},
    dp_t{"Array",array_t{1,2,3,4,5}},
    dp_t{"Dict",dict_t{
      dp_t{"One",1},
      dp_t{"Two",2},
      dp_t{"Three",3},
      dp_t{"Four",4},
    }},
  }};

  assert(dict_0.type() == cclvar::type_dict);
  buffer.str({});
  os << dict_0;
  assert(buffer.str() == "[Array:[1,2,3,4,5],Blank:<blank>,Bool:true,Dict:[Four:4,One:1,Three:3,Two:2],Double:1.25,Integer:1,String:Hello world]");

  assert(dict_0["Integer"].to_int() == 1);
  assert(dict_0["Double"].to_float() == 1.25);
  assert(dict_0["String"].to_str() == "Hello world");
  assert(dict_0["Array"].to_array() == (array_t{1,2,3,4,5}));
  assert(dict_0["Dict"].to_dict() == (dict_t{
    dp_t{"One",1},
    dp_t{"Two",2},
    dp_t{"Three",3},
    dp_t{"Four",4},
  }));

  buffer.str({});
  os << dict_0["Fish"];
  assert(buffer.str() == "<blank>");

  buffer.str({});
  os << dict_0["Array"];
  assert(buffer.str() == "[1,2,3,4,5]");

  dict_0["Array"][2] = var_c();
  buffer.str({});
  os << dict_0["Array"];
  assert(buffer.str() == "[1,2,<blank>,4,5]");

  int idx = 0;
  auto &dict_ref = dict_0.to_dict();
  for (auto pair_i = dict_ref.begin();
            pair_i != dict_ref.end();
          ++pair_i)
  {
    pair_i->second = idx++;
  }

  buffer.str({});
  os << dict_0;
  assert(buffer.str() == "[Array:0,Blank:1,Bool:2,Dict:3,Double:4,Fish:5,Integer:6,String:7]");

  var_c dict_1 = dict_t{
    dp_t{"first",dict_t{
      dp_t{"second",dict_t{
        dp_t{"third",100}
      }},
    }},
  };

  var_c value;
  assert(dict_1.has_key("first",value) &&
         value.has_key("second",value) &&
         value.has_key("third",value));

  assert(value == 100);

  assert(!(dict_1.has_key("first",value) &&
         value.has_key("second",value) &&
         value.has_key("wrong",value)));

  var_c key = array_t{1,2,3};
  value = array_t{"One","Two","Three"};
  
  dict_0 = dict_t{
    dp_t{key,value},
  };

  var_c dict_2 = dict_0.copy();
  assert(dict_0 == dict_2);
  key[0] = {};
  assert(dict_0 != dict_2);

  dict_2 = dict_0.copy();
  assert(dict_0 == dict_2);
  value[0] = {};
  assert(dict_0 != dict_2);

  bool thrown = false;
  try
  {
    (void)dict_0.to_int();
  }
  catch (...)
  {
    thrown = true;
  }
  assert(thrown);
}/*}}}*/

void test_cclvar_all()
{/*{{{*/
  test_cclvar_blank();
  test_cclvar_bool();
  test_cclvar_integer();
  test_cclvar_double();
  test_cclvar_string();
  test_cclvar_array();
  test_cclvar_list();
  test_cclvar_dict();
}/*}}}*/

int main(int argc,char **argv)
{/*{{{*/
  if (argc > 1)
  {
    int arg_idx = 1;
    do {
      if (std::string("blank") == argv[arg_idx])
      {
        test_cclvar_blank();
      }
      else if (std::string("integer") == argv[arg_idx])
      {
        test_cclvar_integer();
      }
      else if (std::string("bool") == argv[arg_idx])
      {
        test_cclvar_bool();
      }
      else if (std::string("double") == argv[arg_idx])
      {
        test_cclvar_double();
      }
      else if (std::string("string") == argv[arg_idx])
      {
        test_cclvar_string();
      }
      else if (std::string("array") == argv[arg_idx])
      {
        test_cclvar_array();
      }
      else if (std::string("list") == argv[arg_idx])
      {
        test_cclvar_list();
      }
      else if (std::string("dict") == argv[arg_idx])
      {
        test_cclvar_dict();
      }
      else if (std::string("all") == argv[arg_idx])
      {
        test_cclvar_all();
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

