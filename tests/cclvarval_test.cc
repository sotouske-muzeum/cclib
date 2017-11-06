
#include "cclvarval.h"
#include "ccljson.h"

using cclvar::var_c;
using cclvar::string_t;
using cclvar::array_t;
using cclvar::list_t;
using cclvar::dict_t;

typedef cclvar::dict_pair_t dp_t;

#define VALIDATE_CATCH_CHECK(CODE,EXPECTED) \
{/*{{{*/\
  bool catched = false;\
  try { CODE; }\
  catch (std::exception &e)\
  {\
    std::cout << "exception: " << e.what() <<\
      ", value: " << val.value_stack() << \
      ", props: " << val.props_stack() << \
      std::endl;\
    catched = e.what() == cclvarval::EXPECTED;\
  }\
  assert(catched);\
}/*}}}*/

void test_cclvarval_blank()
{/*{{{*/
  cclvarval::validator_c val{dict_t
  {/*{{{*/
    dp_t{"root",dict_t{
      dp_t{"type","blank"},
    }},
  }};/*}}}*/

  val.validate({});

  VALIDATE_CATCH_CHECK(val.validate(1);,error_VARVAL_INVALID_TYPE);
}/*}}}*/

void test_cclvarval_bool()
{/*{{{*/
  cclvarval::validator_c val{dict_t
  {/*{{{*/
    dp_t{"root",dict_t{
      dp_t{"type","array"},
      dp_t{"items",dict_t{
        dp_t{0,dict_t{
          dp_t{"type","bool"},
        }},
        dp_t{1,dict_t{
          dp_t{"type","bool"},
          dp_t{"==",false},
        }},
      }},
    }},
  }};/*}}}*/

  val.validate(array_t{true,false});

  VALIDATE_CATCH_CHECK(val.validate(array_t{1,false});,error_VARVAL_INVALID_TYPE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{true,true});,error_VARVAL_INVALID_VALUE);
}/*}}}*/

void test_cclvarval_integer()
{/*{{{*/
  cclvarval::validator_c val{dict_t
  {/*{{{*/
    dp_t{"root",dict_t{
      dp_t{"type","array"},
      dp_t{"items",dict_t{
        dp_t{0,dict_t{
          dp_t{"type","integer"},
        }},
        dp_t{1,dict_t{
          dp_t{"type","integer"},
          dp_t{"==",2},
        }},
      }},
    }},
  }};/*}}}*/

  val.validate(array_t{1,2,3,4,5});

  VALIDATE_CATCH_CHECK(val.validate(array_t{false,2,3,4,5});,error_VARVAL_INVALID_TYPE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{1,3,3,4,5});,error_VARVAL_INVALID_VALUE);
}/*}}}*/

void test_cclvarval_float()
{/*{{{*/
  cclvarval::validator_c val{dict_t
  {/*{{{*/
    dp_t{"root",dict_t{
      dp_t{"type","array"},
      dp_t{"items",dict_t{
        dp_t{0,dict_t{
          dp_t{"type","float"},
        }},
        dp_t{1,dict_t{
          dp_t{"type","float"},
          dp_t{"==",1.0},
        }},
      }},
    }},
  }};/*}}}*/

  val.validate(array_t{0.0,1.0,2.0,3.0,4.0});

  VALIDATE_CATCH_CHECK(val.validate(array_t{0,1.0,2.0,3.0,4.0});,error_VARVAL_INVALID_TYPE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{0.0,2.0,2.0,3.0,4.0});,error_VARVAL_INVALID_VALUE);
}/*}}}*/

void test_cclvarval_string()
{/*{{{*/
  cclvarval::validator_c val{dict_t
  {/*{{{*/
    dp_t{"root",dict_t{
      dp_t{"type","array"},
      dp_t{"items",dict_t{
        dp_t{0,dict_t{
          dp_t{"type","string"},
        }},
        dp_t{1,dict_t{
          dp_t{"type","string"},
          dp_t{"==","world"},
        }},
      }},
    }},
  }};/*}}}*/

  val.validate(array_t{"hello","world"});

  VALIDATE_CATCH_CHECK(val.validate(array_t{true,"world"});,error_VARVAL_INVALID_TYPE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{"hello","hello world"});,error_VARVAL_INVALID_VALUE);
}/*}}}*/

void test_cclvarval_array()
{/*{{{*/
  cclvarval::validator_c val{dict_t
  {/*{{{*/
    dp_t{"root",dict_t{
      dp_t{"type","array"},
      dp_t{"items",dict_t{
        dp_t{0,dict_t{
          dp_t{"type","array"},
          dp_t{"==",array_t{1,2,3}},
        }},
        dp_t{1,dict_t{
          dp_t{"type","array"},
        }},
        dp_t{2,dict_t{
          dp_t{"type","array"},
          dp_t{"all-items",dict_t{
            dp_t{"type","integer"},
          }},
        }},
      }},
    }},
  }};/*}}}*/

  val.validate(array_t{array_t{1,2,3},array_t{1,2,3},array_t{1,2,3}});

  VALIDATE_CATCH_CHECK(
      val.validate(array_t{array_t{1,2,3},array_t{1,2,3},false});,
      error_VARVAL_INVALID_TYPE);

  VALIDATE_CATCH_CHECK(
      val.validate(array_t{array_t{1,2,3,4},array_t{1,2,3},array_t{1,2,3}});,
      error_VARVAL_INVALID_VALUE);

  VALIDATE_CATCH_CHECK(
      val.validate(array_t{array_t{1,2,3},array_t{1,2,3}});,
      error_VARVAL_ARRAY_INVALID_SIZE);

  VALIDATE_CATCH_CHECK(
      val.validate(array_t{array_t{1,2,3},array_t{1,2,3},array_t{1,2,3.0}});,
      error_VARVAL_INVALID_TYPE);
}/*}}}*/

void test_cclvarval_dict()
{/*{{{*/
  cclvarval::validator_c val{dict_t
  {/*{{{*/
    dp_t{"type-integer",dict_t{
      dp_t{"type","integer"},
    }},
    dp_t{"root",dict_t{
      dp_t{"type","dict"},
      dp_t{"items",dict_t{
        dp_t{"first",dict_t{
          dp_t{"type","dict"},
          dp_t{"==",dict_t{
            dp_t{"first",1},
            dp_t{"second",2},
            dp_t{"third",3},
          }},
        }},
        dp_t{"second",dict_t{
          dp_t{"type","dict"},
          dp_t{"all-keys",dict_t{
            dp_t{"type","string"},
          }},
        }},
        dp_t{"third",dict_t{
          dp_t{"type","dict"},
          dp_t{"all-items","type-integer"},
          dp_t{"opt-items",dict_t{
            dp_t{"four","type-integer"},
            dp_t{"five","type-integer"},
          }},
        }},
      }},
    }},
  }};/*}}}*/

  val.validate(dict_t{
    dp_t{"first",dict_t{dp_t{"first",1},dp_t{"second",2},dp_t{"third",3},}},
    dp_t{"second",dict_t{
      dp_t{"one",1},
      dp_t{"two",2},
      dp_t{"three",3},
    }},
    dp_t{"third",dict_t{
      dp_t{"one",1},
      dp_t{"two",2},
      dp_t{"three",3},
    }},
  });

  VALIDATE_CATCH_CHECK(
    val.validate(dict_t{
      dp_t{"first",dict_t{dp_t{"first",1},dp_t{"second",2},dp_t{"third",3},}},
      dp_t{"second",dict_t{}},
      dp_t{"third",true},
    });,
    error_VARVAL_INVALID_TYPE);

  VALIDATE_CATCH_CHECK(
    val.validate(dict_t{
      dp_t{"first",dict_t{dp_t{"first",1},dp_t{"second",2},dp_t{"third",false},}},
      dp_t{"second",dict_t{}},
      dp_t{"third",true},
    });,
    error_VARVAL_INVALID_VALUE);

  VALIDATE_CATCH_CHECK(
    val.validate(dict_t{
      dp_t{"first",dict_t{dp_t{"first",1},dp_t{"second",2},dp_t{"third",3},}},
      dp_t{"second",dict_t{}},
    });,
    error_VARVAL_DICT_MISSING_KEY);

  VALIDATE_CATCH_CHECK(
    val.validate(dict_t{
      dp_t{"first",dict_t{dp_t{"first",1},dp_t{"second",2},dp_t{"third",3},}},
      dp_t{"second",dict_t{
        dp_t{"one",1},
        dp_t{"two",2},
        dp_t{3,3},
      }},
      dp_t{"third",dict_t{}},
    });,
    error_VARVAL_INVALID_TYPE);

  VALIDATE_CATCH_CHECK(
    val.validate(dict_t{
      dp_t{"first",dict_t{dp_t{"first",1},dp_t{"second",2},dp_t{"third",3},}},
      dp_t{"second",dict_t{}},
      dp_t{"third",dict_t{
        dp_t{"one",1},
        dp_t{"two",2},
        dp_t{"three","three"},
      }},
    });,
    error_VARVAL_INVALID_TYPE);

  VALIDATE_CATCH_CHECK(
    val.validate(dict_t{
      dp_t{"first",dict_t{dp_t{"first",1},dp_t{"second",2},dp_t{"third",3},}},
      dp_t{"second",dict_t{}},
      dp_t{"third",dict_t{
        dp_t{"five","five"},
      }},
    });,
    error_VARVAL_INVALID_TYPE);

}/*}}}*/

void test_cclvarval_compare()
{/*{{{*/
  cclvarval::validator_c val{dict_t
  {/*{{{*/
    dp_t{"root",dict_t{
      dp_t{"type","array"},
      dp_t{"items",dict_t{
        dp_t{0,dict_t{
          dp_t{"type","integer"},
          dp_t{"==",100},
        }},
        dp_t{1,dict_t{
          dp_t{"type","integer"},
          dp_t{"!=",100},
        }},
        dp_t{2,dict_t{
          dp_t{"type","integer"},
          dp_t{"<",100},
        }},
        dp_t{3,dict_t{
          dp_t{"type","integer"},
          dp_t{">",100},
        }},
        dp_t{4,dict_t{
          dp_t{"type","integer"},
          dp_t{"<=",100},
        }},
        dp_t{5,dict_t{
          dp_t{"type","integer"},
          dp_t{">=",100},
        }},
      }},
    }},
  }};/*}}}*/

  val.validate(array_t{100,99,99,101,100,100});

  VALIDATE_CATCH_CHECK(val.validate(array_t{99,99,99,101,100,100});,error_VARVAL_INVALID_VALUE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{100,100,99,101,100,100});,error_VARVAL_INVALID_VALUE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{100,99,100,101,100,100});,error_VARVAL_INVALID_VALUE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{100,99,99,100,100,100});,error_VARVAL_INVALID_VALUE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{100,99,99,101,101,100});,error_VARVAL_INVALID_VALUE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{100,99,99,101,100,99});,error_VARVAL_INVALID_VALUE);
}/*}}}*/

void test_cclvarval_size()
{/*{{{*/
  cclvarval::validator_c val{dict_t
  {/*{{{*/
    dp_t{"root",dict_t{
      dp_t{"type","array"},
      dp_t{"items",dict_t{
        dp_t{0,dict_t{
          dp_t{"size ==",3},
        }},
        dp_t{1,dict_t{
          dp_t{"size !=",2},
        }},
        dp_t{2,dict_t{
          dp_t{"size <",4},
        }},
        dp_t{3,dict_t{
          dp_t{"size >",2},
        }},
        dp_t{4,dict_t{
          dp_t{"size <=",3},
        }},
        dp_t{5,dict_t{
          dp_t{"size >=",3},
        }},
      }},
    }},
  }};/*}}}*/

  cclvar::var_c str2{"xx"};
  cclvar::var_c str3{"xxx"};
  cclvar::var_c str4{"xxxx"};

  cclvar::var_c arr2{array_t{1,2}};
  cclvar::var_c arr3{array_t{1,2,3}};
  cclvar::var_c arr4{array_t{1,2,3,4}};

  cclvar::var_c lst2{list_t{1,2}};
  cclvar::var_c lst3{list_t{1,2,3}};
  cclvar::var_c lst4{list_t{1,2,3,4}};

  cclvar::var_c dct2{dict_t{dp_t{"first",1},dp_t{"second",2} }};
  cclvar::var_c dct3{dict_t{dp_t{"first",1},dp_t{"second",2},dp_t{"third",3} }};
  cclvar::var_c dct4{dict_t{dp_t{"first",1},dp_t{"second",2},dp_t{"third",3},dp_t{"fourth",4} }};

  val.validate(array_t{str3,str3,str3,str3,str3,str3});
  val.validate(array_t{arr3,arr3,arr3,arr3,arr3,arr3});
  val.validate(array_t{lst3,lst3,lst3,lst3,lst3,lst3});
  val.validate(array_t{dct3,dct3,dct3,dct3,dct3,dct3});

  VALIDATE_CATCH_CHECK(val.validate(array_t{str2,str3,str3,str3,str3,str3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{str3,str2,str3,str3,str3,str3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{str3,str3,str4,str3,str3,str3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{str3,str3,str3,str2,str3,str3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{str3,str3,str3,str3,str4,str3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{str3,str3,str3,str3,str3,str2});,error_VARVAL_VALUE_INVALID_SIZE);

  VALIDATE_CATCH_CHECK(val.validate(array_t{arr2,arr3,arr3,arr3,arr3,arr3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{arr3,arr2,arr3,arr3,arr3,arr3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{arr3,arr3,arr4,arr3,arr3,arr3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{arr3,arr3,arr3,arr2,arr3,arr3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{arr3,arr3,arr3,arr3,arr4,arr3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{arr3,arr3,arr3,arr3,arr3,arr2});,error_VARVAL_VALUE_INVALID_SIZE);

  VALIDATE_CATCH_CHECK(val.validate(array_t{lst2,lst3,lst3,lst3,lst3,lst3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{lst3,lst2,lst3,lst3,lst3,lst3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{lst3,lst3,lst4,lst3,lst3,lst3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{lst3,lst3,lst3,lst2,lst3,lst3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{lst3,lst3,lst3,lst3,lst4,lst3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{lst3,lst3,lst3,lst3,lst3,lst2});,error_VARVAL_VALUE_INVALID_SIZE);

  VALIDATE_CATCH_CHECK(val.validate(array_t{dct2,dct3,dct3,dct3,dct3,dct3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{dct3,dct2,dct3,dct3,dct3,dct3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{dct3,dct3,dct4,dct3,dct3,dct3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{dct3,dct3,dct3,dct2,dct3,dct3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{dct3,dct3,dct3,dct3,dct4,dct3});,error_VARVAL_VALUE_INVALID_SIZE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{dct3,dct3,dct3,dct3,dct3,dct2});,error_VARVAL_VALUE_INVALID_SIZE);
}/*}}}*/

void test_cclvarval_regex()
{/*{{{*/
  cclvarval::validator_c val{dict_t
  {/*{{{*/
    dp_t{"root",dict_t{
      dp_t{"type","array"},
      dp_t{"items",dict_t{
        dp_t{0,dict_t{
          dp_t{"regex","^.*$"},
        }},
        dp_t{1,dict_t{
          dp_t{"regex","^ab*c$"},
        }},
      }},
    }},
  }};/*}}}*/

  val.validate(array_t{"hello","ac"});
  val.validate(array_t{"hello","abc"});

  VALIDATE_CATCH_CHECK(val.validate(array_t{true,"ac"});,error_VARVAL_INVALID_TYPE);
  VALIDATE_CATCH_CHECK(val.validate(array_t{"hello","ab"});,error_VARVAL_INVALID_VALUE);
}/*}}}*/

void test_cclvarval_all()
{/*{{{*/
  test_cclvarval_blank();
  test_cclvarval_bool();
  test_cclvarval_integer();
  test_cclvarval_float();
  test_cclvarval_string();
  test_cclvarval_array();
  test_cclvarval_dict();
  test_cclvarval_compare();
  test_cclvarval_size();
  test_cclvarval_regex();
}/*}}}*/

int main(int argc,char **argv)
{/*{{{*/
  if (argc > 1)
  {
    int arg_idx = 1;
    do {
      if (std::string("blank") == argv[arg_idx])
      {
        test_cclvarval_blank();
      }
      else if (std::string("bool") == argv[arg_idx])
      {
        test_cclvarval_bool();
      }
      else if (std::string("integer") == argv[arg_idx])
      {
        test_cclvarval_integer();
      }
      else if (std::string("float") == argv[arg_idx])
      {
        test_cclvarval_float();
      }
      else if (std::string("string") == argv[arg_idx])
      {
        test_cclvarval_string();
      }
      else if (std::string("array") == argv[arg_idx])
      {
        test_cclvarval_array();
      }
      else if (std::string("dict") == argv[arg_idx])
      {
        test_cclvarval_dict();
      }
      else if (std::string("compare") == argv[arg_idx])
      {
        test_cclvarval_compare();
      }
      else if (std::string("size") == argv[arg_idx])
      {
        test_cclvarval_size();
      }
      else if (std::string("regex") == argv[arg_idx])
      {
        test_cclvarval_regex();
      }
      else if (std::string("all") == argv[arg_idx])
      {
        test_cclvarval_all();
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

