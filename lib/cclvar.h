
#ifndef CCLIB_CCLVAR_H
#define CCLIB_CCLVAR_H

#include "cclbase.h"

#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include <cassert>

namespace cclvar {

class var_c;

// - exceptions list -
const std::string error_VARIABLE_INVALID_TYPE_REQUESTED = "cclvar: invalid variable type requested";
const std::string error_INVALID_OPERATION_ON_VARIABLE = "cclvar: invalid operation on variable";
const std::string error_ARRAY_INDEX_EXCEEDS_RANGE = "cclvar: index exceeds array range";
const std::string error_UNSUPPORTED_VARIABLE_TYPE = "cclvar: unsupported variable type";
const std::string error_UNEXPECTED_VARIABLE_TYPE = "cclvar: unexpected variable type";
const std::string error_INVALID_ARRAY_SIZE = "cclvar: invalid array size";

enum
{/*{{{*/
  type_blank = 0,
  type_bool,
  type_integer,
  type_float,
  type_string,
  type_array,
  type_list,
  type_dict,
};/*}}}*/

union data_type_u
{/*{{{*/
  int64_t m_int64;
  double m_double;
  void *m_ptr;

  data_type_u() = default;
  data_type_u(bool a_bool)                 : m_int64{a_bool}    {} // NOLINT
  data_type_u(int64_t a_int64)             : m_int64{a_int64}   {} // NOLINT
  data_type_u(double a_double)             : m_double{a_double} {} // NOLINT
  template <class T> data_type_u(T* a_ptr) : m_ptr{a_ptr}       {} // NOLINT

  operator bool                  () const { return static_cast<bool>(m_int64); }
  operator int64_t               () const { return m_int64; }
  operator double                () const { return m_double; }
  template <class T> operator T* () const { return static_cast<T *>(m_ptr); }
};/*}}}*/

typedef std::string string_t;
typedef std::vector<var_c> array_t;
typedef std::list<var_c> list_t;
typedef std::map<var_c,var_c> dict_t;
typedef std::pair<var_c,var_c> dict_pair_t;

class loc_c
{/*{{{*/
  friend class var_c;

  private:
  int m_type;
  volatile int m_ref_cnt;
  data_type_u m_data_ptr;

  const loc_c &check_type(int a_type) const
  {/*{{{*/

    // - ERROR -
    if (m_type != a_type)
    {
      cclthrow(error_VARIABLE_INVALID_TYPE_REQUESTED);
    }

    return *this;
  }/*}}}*/

  loc_c()               : m_type{type_blank},   m_ref_cnt{1} {}
  loc_c(bool a_val)     : m_type{type_bool},    m_ref_cnt{1}, m_data_ptr{a_val} {} // NOLINT
  loc_c(int64_t a_val)  : m_type{type_integer}, m_ref_cnt{1}, m_data_ptr{a_val} {} // NOLINT
  loc_c(double a_val)   : m_type{type_float},   m_ref_cnt{1}, m_data_ptr{a_val} {} // NOLINT
  loc_c(string_t a_val) : m_type{type_string},  m_ref_cnt{1}, m_data_ptr{new string_t(std::move(a_val))} {} // NOLINT
  loc_c(array_t a_val)  : m_type{type_array},   m_ref_cnt{1}, m_data_ptr{new array_t(std::move(a_val))} {} // NOLINT
  loc_c(list_t a_val)   : m_type{type_list},    m_ref_cnt{1}, m_data_ptr{new list_t(std::move(a_val))} {} // NOLINT
  inline loc_c(dict_t a_val); // NOLINT

  ~loc_c();

  loc_c &ref() { __sync_add_and_fetch(&m_ref_cnt,1); return *this; }
  void unref() { if (__sync_sub_and_fetch(&m_ref_cnt,1) == 0) { delete this; } }

  operator bool            () const { check_type(type_bool);    return m_data_ptr; }
  operator int64_t         () const { check_type(type_integer); return m_data_ptr; }
  operator double          () const { check_type(type_float);   return m_data_ptr; }
  operator const string_t &() const { check_type(type_string);  return *(static_cast<string_t *>(m_data_ptr)); }
  operator array_t &       () const { check_type(type_array);   return *(static_cast<array_t *>(m_data_ptr)); }
  operator list_t &        () const { check_type(type_list);    return *(static_cast<list_t *>(m_data_ptr)); }
  operator dict_t &        () const { check_type(type_dict);    return *(static_cast<dict_t *>(m_data_ptr)); }

  int type() const { return m_type; }
  int compare_value(const loc_c &a_loc) const;
  int compare(const loc_c &a_loc) const
  {/*{{{*/
    return m_type != a_loc.m_type ?
      (m_type < a_loc.m_type ? -1 : 1) :
      compare_value(a_loc);
  }/*}}}*/

  var_c &operator [] (int64_t a_idx) const;
  var_c &operator [] (const var_c &a_key) const;

  bool has_idx(int64_t a_idx,var_c &a_value) const;
  bool has_key(const var_c &a_key,var_c &a_value) const;

  friend std::ostream &operator << (std::ostream &a_os,const loc_c &a_loc);
};/*}}}*/

class var_c
{/*{{{*/
  private:
  static std::unordered_map<std::string,var_c> m_str_map;

  loc_c *m_loc_ptr;

  public:
  var_c() : m_loc_ptr{new loc_c{}} {}
  var_c(bool        a_val) : m_loc_ptr{new loc_c{a_val}}            {} // NOLINT
  var_c(int64_t     a_val) : m_loc_ptr{new loc_c{a_val}}            {} // NOLINT
  var_c(double      a_val) : m_loc_ptr{new loc_c{a_val}}            {} // NOLINT
  var_c(string_t    a_val) : m_loc_ptr{new loc_c{std::move(a_val)}} {} // NOLINT
  var_c(const char *a_val); // NOLINT
  var_c(array_t     a_val) : m_loc_ptr{new loc_c{std::move(a_val)}} {} // NOLINT
  var_c(list_t      a_val) : m_loc_ptr{new loc_c{std::move(a_val)}} {} // NOLINT
  var_c(dict_t      a_val) : m_loc_ptr{new loc_c{std::move(a_val)}} {} // NOLINT

  var_c(const var_c &a_var) : m_loc_ptr{a_var.m_loc_ptr} { m_loc_ptr->ref(); }

  var_c(int8_t   a_val) : m_loc_ptr{new loc_c{static_cast<int64_t>(a_val)}} {} // NOLINT
  var_c(int16_t  a_val) : m_loc_ptr{new loc_c{static_cast<int64_t>(a_val)}} {} // NOLINT
  var_c(int32_t  a_val) : m_loc_ptr{new loc_c{static_cast<int64_t>(a_val)}} {} // NOLINT
  var_c(uint8_t  a_val) : m_loc_ptr{new loc_c{static_cast<int64_t>(a_val)}} {} // NOLINT
  var_c(uint16_t a_val) : m_loc_ptr{new loc_c{static_cast<int64_t>(a_val)}} {} // NOLINT
  var_c(uint32_t a_val) : m_loc_ptr{new loc_c{static_cast<int64_t>(a_val)}} {} // NOLINT
  var_c(float    a_val) : m_loc_ptr{new loc_c{static_cast<double>(a_val)}}  {} // NOLINT

  ~var_c() { m_loc_ptr->unref(); }

  var_c &operator = (const var_c &a_var)
  {/*{{{*/
    a_var.m_loc_ptr->ref();
    m_loc_ptr->unref();
    m_loc_ptr = a_var.m_loc_ptr;
    return *this;
  }/*}}}*/

  bool            to_bool () const { return *m_loc_ptr; }
  int64_t         to_int  () const { return *m_loc_ptr; }
  double          to_float() const { return *m_loc_ptr; }
  const string_t &to_str  () const { return *m_loc_ptr; }
  array_t &       to_array() const { return *m_loc_ptr; }
  list_t &        to_list () const { return *m_loc_ptr; }
  dict_t &        to_dict () const { return *m_loc_ptr; }

  int type() const { return m_loc_ptr->type(); }
  int compare(const var_c &a_var) const { return m_loc_ptr->compare(*a_var.m_loc_ptr); }
  var_c copy() const;
  bool operator == (const var_c &a_var) const { return compare(a_var) == 0; }
  bool operator != (const var_c &a_var) const { return compare(a_var) != 0; }
  bool operator < (const var_c &a_var) const { return compare(a_var) < 0; }

  var_c &operator [] (int64_t a_idx) const { return (*m_loc_ptr)[a_idx]; }
  var_c &operator [] (const var_c &a_key) const { return (*m_loc_ptr)[a_key]; }

  bool has_idx(int64_t a_idx,var_c &a_value) const
  {/*{{{*/
    return (*m_loc_ptr).has_idx(a_idx,a_value);
  }/*}}}*/
  bool has_key(const var_c &a_key,var_c &a_value) const
  {/*{{{*/
    return (*m_loc_ptr).has_key(a_key,a_value);
  }/*}}}*/

  friend std::ostream &operator << (std::ostream &a_os,const var_c &a_var)
  {/*{{{*/
    a_os << *a_var.m_loc_ptr;
    return a_os;
  }/*}}}*/
};/*}}}*/

inline loc_c::loc_c(dict_t a_val) : m_type{type_dict}, m_ref_cnt{1}, m_data_ptr{new dict_t(std::move(a_val))} {}

std::string to_string(const var_c &a_var);

} // namespace cclvar

#endif

