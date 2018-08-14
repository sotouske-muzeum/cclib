
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
    case type_set:
      delete static_cast<set_t *>(m_data_ptr);
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
      auto first = static_cast<int64_t>(m_data_ptr);
      auto second = static_cast<int64_t>(a_loc.m_data_ptr);

      return first < second ? -1 : (first > second ? 1 : 0);
    }/*}}}*/
  case type_float:
    {/*{{{*/
      auto first = static_cast<double>(m_data_ptr);
      auto second = static_cast<double>(a_loc.m_data_ptr);

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
  case type_set:
    {/*{{{*/
      set_t &first = *static_cast<set_t *>(m_data_ptr);
      set_t &second = *static_cast<set_t *>(a_loc.m_data_ptr);

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

std::unordered_map<std::string,var_c> var_c::m_str_map;

var_c::var_c(const char *a_val)
{/*{{{*/
  std::string value{a_val};

  auto map_i = m_str_map.find(value);
  if (map_i != m_str_map.end())
  {
    map_i->second.m_loc_ptr->ref();
    m_loc_ptr = map_i->second.m_loc_ptr;
  }
  else
  {
    m_loc_ptr = new loc_c{value};
    m_str_map[value] = *this;
  }
}/*}}}*/

var_c var_c::copy() const
{/*{{{*/
  switch (m_loc_ptr->m_type)
  {
  case type_blank:
  case type_bool:
  case type_integer:
  case type_float:
  case type_string:
    return *this;
  case type_array:
    {/*{{{*/
      var_c res_value{array_t{}};

      array_t &source = *static_cast<array_t *>(m_loc_ptr->m_data_ptr);
      array_t &target = *static_cast<array_t *>(res_value.m_loc_ptr->m_data_ptr);

      for (auto s_iter = source.begin();
                s_iter != source.end();
              ++s_iter)
      {
        target.push_back(s_iter->copy());
      }

      return res_value;
    }/*}}}*/
  case type_list:
    {/*{{{*/
      var_c res_value{list_t{}};

      list_t &source = *static_cast<list_t *>(m_loc_ptr->m_data_ptr);
      list_t &target = *static_cast<list_t *>(res_value.m_loc_ptr->m_data_ptr);

      for (auto s_iter = source.begin();
                s_iter != source.end();
              ++s_iter)
      {
        target.push_back(s_iter->copy());
      }

      return res_value;
    }/*}}}*/
  case type_set:
    {/*{{{*/
      var_c res_value{set_t{}};

      set_t &source = *static_cast<set_t *>(m_loc_ptr->m_data_ptr);
      set_t &target = *static_cast<set_t *>(res_value.m_loc_ptr->m_data_ptr);

      for (auto s_iter = source.begin();
                s_iter != source.end();
              ++s_iter)
      {
        target.insert(target.end(),s_iter->copy());
      }

      return res_value;
    }/*}}}*/
  case type_dict:
    {/*{{{*/
      var_c res_value{dict_t{}};

      dict_t &source = *static_cast<dict_t *>(m_loc_ptr->m_data_ptr);
      dict_t &target = *static_cast<dict_t *>(res_value.m_loc_ptr->m_data_ptr);

      for (auto s_iter = source.begin();
                s_iter != source.end();
              ++s_iter)
      {
        target[s_iter->first.copy()] = s_iter->second.copy();
      }

      return res_value;
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

var_c loc_c::keys() const
{/*{{{*/

  // - ERROR -
  if (m_type != type_dict)
  {
    cclthrow(error_INVALID_OPERATION_ON_VARIABLE);
  }

  var_c res_value{array_t{}};
  auto &res_array = res_value.to_array();

  auto &dict = *static_cast<dict_t *>(m_data_ptr);
  for (auto pair_i = dict.begin();
            pair_i != dict.end();
          ++pair_i)
  {
    res_array.push_back(pair_i->first);
  }

  return res_value;
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

std::string loc_c::to_string(const std::string &a_separator)
{/*{{{*/
  std::stringbuf strbuf;
  std::ostream os(&strbuf);

  switch (m_type)
  {
  case type_array:
    {/*{{{*/
      bool first = true;
      auto &array = *static_cast<array_t *>(m_data_ptr);
      for (auto var_i = array.begin();
                var_i != array.end();
              ++var_i)
      {
        os << (first ? "" : a_separator.data()) << *var_i;
        first = false;
      }
    }/*}}}*/
    break;
  case type_list:
    {/*{{{*/
      bool first = true;
      auto &list = *static_cast<list_t *>(m_data_ptr);
      for (auto var_i = list.begin();
                var_i != list.end();
              ++var_i)
      {
        os << (first ? "" : a_separator.data()) << *var_i;
        first = false;
      }
    }/*}}}*/
    break;
  case type_set:
    {/*{{{*/
      bool first = true;
      auto &set = *static_cast<set_t *>(m_data_ptr);
      for (auto var_i = set.begin();
                var_i != set.end();
              ++var_i)
      {
        os << (first ? "" : a_separator.data()) << *var_i;
        first = false;
      }
    }/*}}}*/
    break;
  case type_dict:
    {/*{{{*/
      bool first = true;
      auto &dict = *static_cast<dict_t *>(m_data_ptr);
      for (auto pair_i = dict.begin();
                pair_i != dict.end();
              ++pair_i)
      {
        os << (first ? "" : a_separator.data()) << pair_i->first << ':' << pair_i->second;
        first = false;
      }
    }/*}}}*/
    break;

  // - ERROR -
  default:
    cclthrow(error_INVALID_OPERATION_ON_VARIABLE);
  }

  return strbuf.str();
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
  case type_set:
    {/*{{{*/
      bool first = true;
      auto &set = *static_cast<set_t *>(a_loc.m_data_ptr);
      if (set.empty())
      {
        a_os << "[]";
        break;
      }

      for (auto var_i = set.begin();
                var_i != set.end();
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

