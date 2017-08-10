
#include "cclvar.h"

namespace cclvar {

loc_c::~loc_c()
{/*{{{*/
  switch (m_type)
  {
    case type_string:
      delete static_cast<string_t *>(m_data_ptr);
      break;
    case type_array:
      delete static_cast<array_t *>(m_data_ptr);
      break;
    case type_list:
      delete static_cast<list_t *>(m_data_ptr);
      break;
    case type_dict:
      delete static_cast<dict_t *>(m_data_ptr);
      break;
    default:
      break;
  }
}/*}}}*/

int loc_c::compare_value(const loc_c &a_loc) const
{/*{{{*/
  switch (m_type)
  {
  case type_blank:
    return 0;
  case type_bool:
  case type_integer:
    {/*{{{*/
      int64_t first = static_cast<int64_t>(m_data_ptr);
      int64_t second = static_cast<int64_t>(a_loc.m_data_ptr);

      return first < second ? -1 : (first > second ? 1 : 0);
    }/*}}}*/
  case type_float:
    {/*{{{*/
      double first = static_cast<double>(m_data_ptr);
      double second = static_cast<double>(a_loc.m_data_ptr);

      return first < second ? -1 : (first > second ? 1 : 0);
    }/*}}}*/
  case type_string:
    return static_cast<string_t *>(m_data_ptr)->compare(
           *static_cast<string_t *>(a_loc.m_data_ptr));
  case type_array:
    {/*{{{*/
      array_t &first = *static_cast<array_t *>(m_data_ptr);
      array_t &second = *static_cast<array_t *>(a_loc.m_data_ptr);

      if (first.size() != second.size())
      {
        return first.size() < second.size() ? -1 : 1;
      }

      auto f_iter = first.begin();
      auto s_iter = second.begin();

      while (f_iter != first.end())
      {
        int result = (f_iter++)->compare(*s_iter++);
        if (result != 0)
        {
          return result;
        }
      }

      return 0;
    }/*}}}*/
  case type_list:
    {/*{{{*/
      list_t &first = *static_cast<list_t *>(m_data_ptr);
      list_t &second = *static_cast<list_t *>(a_loc.m_data_ptr);

      if (first.size() != second.size())
      {
        return first.size() < second.size() ? -1 : 1;
      }

      auto f_iter = first.begin();
      auto s_iter = second.begin();

      while (f_iter != first.end())
      {
        int result = (f_iter++)->compare(*s_iter++);
        if (result != 0)
        {
          return result;
        }
      }

      return 0;
    }/*}}}*/
  case type_dict:
    {/*{{{*/
      dict_t &first = *static_cast<dict_t *>(m_data_ptr);
      dict_t &second = *static_cast<dict_t *>(a_loc.m_data_ptr);

      if (first.size() != second.size())
      {
        return first.size() < second.size() ? -1 : 1;
      }

      auto f_iter = first.begin();
      auto s_iter = second.begin();

      while (f_iter != first.end())
      {
        // - compare map keys -
        int result = f_iter->first.compare(s_iter->first);
        if (result != 0)
        {
          return result;
        }

        // - compare map values -
        result = f_iter->second.compare(s_iter->second);
        if (result != 0)
        {
          return result;
        }

        ++f_iter;
        ++s_iter;
      }

      return 0;
    }/*}}}*/
  default:
    assert(false);
  }
}/*}}}*/

var_c &loc_c::operator [] (int64_t a_idx) const
{/*{{{*/
  switch (m_type)
  {
  case type_array:
    {/*{{{*/
      try {
        return static_cast<array_t *>(m_data_ptr)->at(a_idx);
      }
      catch (...)
      {
        cclthrow(error_ARRAY_INDEX_EXCEEDS_RANGE);
      }
    }/*}}}*/
    break;
  case type_dict:
    {/*{{{*/
      return (*static_cast<dict_t *>(m_data_ptr))[a_idx];
    }/*}}}*/
    break;

  // - ERROR -
  default:

    cclthrow(error_INVALID_OPERATION_ON_VARIABLE);
  }
}/*}}}*/

var_c &loc_c::operator [] (const var_c &a_key) const
{/*{{{*/
  switch (m_type)
  {
  case type_array:
    {/*{{{*/
      int64_t idx = a_key.to_int();

      try {
        return static_cast<array_t *>(m_data_ptr)->at(idx);
      }
      catch (...)
      {
        cclthrow(error_ARRAY_INDEX_EXCEEDS_RANGE);
      }
    }/*}}}*/
    break;
  case type_dict:
    {/*{{{*/
      return (*static_cast<dict_t *>(m_data_ptr))[a_key];
    }/*}}}*/
    break;

  // - ERROR -
  default:

    cclthrow(error_INVALID_OPERATION_ON_VARIABLE);
  }
}/*}}}*/

bool loc_c::has_idx(int64_t a_idx,var_c &a_value) const
{/*{{{*/
  
  // - ERROR -
  if (m_type != type_array)
  {
    cclthrow(error_INVALID_OPERATION_ON_VARIABLE);
  }

  try {
    a_value = (*static_cast<array_t *>(m_data_ptr)).at(a_idx);
    return true;
  }
  catch (...)
  {
    return false;
  }
}/*}}}*/

bool loc_c::has_key(const var_c &a_key,var_c &a_value) const
{/*{{{*/
  
  // - ERROR -
  if (m_type != type_dict)
  {
    cclthrow(error_INVALID_OPERATION_ON_VARIABLE);
  }

  try {
    a_value = (*static_cast<dict_t *>(m_data_ptr)).at(a_key);
    return true;
  }
  catch (...)
  {
    return false;
  }
}/*}}}*/

std::ostream &operator << (std::ostream &a_os,const loc_c &a_loc)
{/*{{{*/
  switch (a_loc.m_type)
  {
  case type_blank:
    a_os << "<blank>";
    break;
  case type_bool:
    a_os << (static_cast<bool>(a_loc.m_data_ptr) ? "true" : "false");
    break;
  case type_integer:
    a_os << static_cast<int64_t>(a_loc.m_data_ptr);
    break;
  case type_float:
    a_os << static_cast<double>(a_loc.m_data_ptr);
    break;
  case type_string:
    a_os << *static_cast<string_t *>(a_loc.m_data_ptr);
    break;
  case type_array:
    {/*{{{*/
      bool first = true;
      auto &array = *static_cast<array_t *>(a_loc.m_data_ptr);
      if (array.empty())
      {
        a_os << "[]";
        break;
      }

      for (auto var_i = array.begin();
                var_i != array.end();
              ++var_i)
      {
        a_os << (first ? '[' : ',') << *var_i;
        first = false;
      }
      a_os << ']';
    }/*}}}*/
    break;
  case type_list:
    {/*{{{*/
      bool first = true;
      auto &list = *static_cast<list_t *>(a_loc.m_data_ptr);
      if (list.empty())
      {
        a_os << "[]";
        break;
      }

      for (auto var_i = list.begin();
                var_i != list.end();
              ++var_i)
      {
        a_os << (first ? '[' : ',') << *var_i;
        first = false;
      }
      a_os << ']';
    }/*}}}*/
    break;
  case type_dict:
    {/*{{{*/
      bool first = true;
      auto &dict = *static_cast<dict_t *>(a_loc.m_data_ptr);
      if (dict.empty())
      {
        a_os << "[]";
        break;
      }

      for (auto pair_i = dict.begin();
                pair_i != dict.end();
              ++pair_i)
      {
        a_os << (first ? '[' : ',') << pair_i->first << ':' << pair_i->second;
        first = false;
      }
      a_os << ']';
    }/*}}}*/
    break;
  default:
    assert(false);
  }

  return a_os;
}/*}}}*/

std::string to_string(const var_c &a_var)
{/*{{{*/
  std::stringbuf strbuf;
  std::ostream os(&strbuf);
  os << a_var;
  return strbuf.str();
}/*}}}*/

} // namespace cclvar

