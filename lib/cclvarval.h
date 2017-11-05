
#ifndef CCLIB_CCLVARVAL_H
#define CCLIB_CCLVARVAL_H

#include "cclvar.h"

#include <algorithm>

namespace cclvarval {

// - exceptions list -
const std::string error_VARVAL_INVALID_SCHEMA_REFERENCE = "validation: invalid schema reference";
const std::string error_VARVAL_INVALID_SCHEMA_PROPERTY = "validation: invalid schema property";
const std::string error_VARVAL_INVALID_TYPE = "validation: invalid type";
const std::string error_VARVAL_INVALID_VALUE = "validation: invalid value";
const std::string error_VARVAL_VALUE_INVALID_SIZE = "validation: invalid value size";
const std::string error_VARVAL_ARRAY_INVALID_SIZE = "validation: invalid array size";
const std::string error_VARVAL_DICT_MISSING_KEY = "validation: missing dictionary key";

enum
{/*{{{*/
  prop_type = 0,
  prop_equal,
  prop_not_equal,
  prop_lesser,
  prop_greater,
  prop_lesser_equal,
  prop_greater_equal,
  prop_size_equal,
  prop_size_not_equal,
  prop_size_lesser,
  prop_size_greater,
  prop_size_lesser_equal,
  prop_size_greater_equal,
  prop_items,
  prop_opt_items,
  prop_all_keys,
  prop_all_items,
};/*}}}*/

typedef std::unordered_map<std::string,int> string_map_t;

class validator_c
{
  private:
  static string_map_t c_type_map;
  static string_map_t c_prop_map;

  cclvar::var_c m_schema;
  cclvar::array_t m_value_stack;
  cclvar::array_t m_props_stack;

  void validate_pair(cclvar::var_c a_value,cclvar::var_c a_props);

  public:
  validator_c(const validator_c &) = delete;
  void operator = (const validator_c &) = delete;
  explicit validator_c(cclvar::var_c a_schema) :
    m_schema{a_schema}, m_value_stack{}, m_props_stack{} {};

  void validate(cclvar::var_c a_value);
  inline cclvar::var_c value_stack();
  inline cclvar::var_c props_stack();
};

inline cclvar::var_c validator_c::value_stack()
{/*{{{*/
  return m_value_stack;
}/*}}}*/

inline cclvar::var_c validator_c::props_stack()
{/*{{{*/
  return m_props_stack;
}/*}}}*/

} // namespace cclvarval

#endif

