
#ifndef CCLIB_CCLJSON_H
#define CCLIB_CCLJSON_H

#include "cclbase.h"
#include "cclvar.h"

#include <limits>
#include <map>
#include <sstream>
#include <vector>

namespace ccljson {

// - exceptions list -
const std::string error_JSON_CREATE_UNSUPPORTED_VARIABLE_TYPE = "json create, unsupported variable type";
const std::string error_JSON_CREATE_NO_STRING_DICT_KEY = "json create, dictionary key must be string value";
const std::string error_JSON_PARSE_ERROR_UNRECOGNIZED_TERMINAL = "json parse, unrecognized terminal symbol";
const std::string error_JSON_PARSE_ERROR_INVALID_SYNTAX = "json parse, invalid json syntax";
const std::string error_JSON_PARSE_ERROR = "json parse error";

using cclvar::var_c;
using cclvar::string_t;
using cclvar::array_t;
using cclvar::list_t;
using cclvar::dict_t;

typedef array_t::iterator array_iter_t;
typedef list_t::iterator list_iter_t;
typedef dict_t::iterator dict_iter_t;

std::stringbuf &append_string(std::stringbuf &a_strbuf,const std::string &a_string);
std::string create(var_c a_var);

std::string create_nice(var_c a_var,const std::string &a_tab_str);
var_c parse(const std::string &a_source);

class create_elem_c
{/*{{{*/
  friend std::string create(var_c a_var);
  friend std::string create_nice(var_c a_var,const std::string &a_tab_str);

  private:
  var_c m_var;
  bool m_init;
  array_iter_t array_i;
  list_iter_t list_i;
  dict_iter_t dict_i;

  public:
  explicit create_elem_c(var_c a_var) : m_var{a_var}, m_init{true} {}
};/*}}}*/

class stack_elem_c
{/*{{{*/
  friend class parser_c;

  private:
  uint32_t m_state;
  uint32_t m_term_start;
  uint32_t m_term_end;

  public:
  explicit stack_elem_c() = default;
  explicit stack_elem_c(uint32_t a_state) : m_state{a_state}, m_term_start{0}, m_term_end{0} {}
  stack_elem_c(uint32_t a_state,uint32_t a_term_start,uint32_t a_term_end) :
    m_state{a_state}, m_term_start{a_term_start}, m_term_end{a_term_end} {}
};/*}}}*/

class parser_c
{/*{{{*/
  typedef std::vector<var_c> var_stack_t;
  typedef std::vector<stack_elem_c> lalr_stack_t;

  typedef std::map<int64_t,var_c> integer_map_t;
  typedef std::map<double,var_c> double_map_t;
  typedef std::map<string_t,var_c> string_map_t;

  private:
  const std::string &m_source;

  uint32_t m_old_input_idx;
  lalr_stack_t m_lalr_stack;

  var_c m_var_null;
  var_c m_var_true;
  var_c m_var_false;

  integer_map_t m_integer_map;
  double_map_t m_double_map;
  string_map_t m_string_map;

  var_stack_t m_strings;
  var_stack_t m_values;
  var_stack_t m_arrays;
  var_stack_t m_objects;

  static bool pa_json_null(parser_c &_this);
  static bool pa_json_object_begin(parser_c &_this);
  static bool pa_json_object_pair(parser_c &_this);
  static bool pa_json_array_begin(parser_c &_this);
  static bool pa_json_array_value(parser_c &_this);
  static bool pa_json_val_string(parser_c &_this);
  static bool pa_json_val_integer(parser_c &_this);
  static bool pa_json_val_float(parser_c &_this);
  static bool pa_json_val_object(parser_c &_this);
  static bool pa_json_val_array(parser_c &_this);
  static bool pa_json_val_true(parser_c &_this);
  static bool pa_json_val_false(parser_c &_this);
  static bool pa_json_val_null(parser_c &_this);
  static bool pa_json_string(parser_c &_this);

  static bool(*json_pa_callers[])(parser_c &_this);

  public:
  explicit parser_c(const std::string &a_source) :
    m_source(a_source), m_var_true(true), m_var_false(false) {}
  uint32_t recognize_terminal(uint32_t &a_input_idx);
  var_c parse_source();
};/*}}}*/

} // namespace ccljson

#endif

