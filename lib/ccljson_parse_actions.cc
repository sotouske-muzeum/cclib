#include "ccljson.h"

#include <iostream>

namespace ccljson {

/*
 * constants and definitions
 */

// - callers of parse action functions -
const unsigned c_json_parse_action_cnt = 24;
bool(*parser_c::json_pa_callers[c_json_parse_action_cnt])(parser_c &_this) =
{/*{{{*/
  parser_c::pa_json_null,
  parser_c::pa_json_null,
  parser_c::pa_json_null,
  parser_c::pa_json_null,
  parser_c::pa_json_null,
  parser_c::pa_json_object_begin,
  parser_c::pa_json_null,
  parser_c::pa_json_null,
  parser_c::pa_json_object_pair,
  parser_c::pa_json_null,
  parser_c::pa_json_null,
  parser_c::pa_json_null,
  parser_c::pa_json_array_begin,
  parser_c::pa_json_array_value,
  parser_c::pa_json_array_value,
  parser_c::pa_json_val_string,
  parser_c::pa_json_val_integer,
  parser_c::pa_json_val_float,
  parser_c::pa_json_val_object,
  parser_c::pa_json_val_array,
  parser_c::pa_json_val_true,
  parser_c::pa_json_val_false,
  parser_c::pa_json_val_null,
  parser_c::pa_json_string,
};/*}}}*/

/*
 * parse action macros
 */

/*
 * functions implementing parse actions of json parser
 */

bool parser_c::pa_json_null(parser_c &_this)
{/*{{{*/
  (void)_this;
  assert(false);

  return true;
}/*}}}*/

bool parser_c::pa_json_object_begin(parser_c &_this)
{/*{{{*/
  _this.m_objects.push_back(dict_t{});
  return true;
}/*}}}*/

bool parser_c::pa_json_object_pair(parser_c &_this)
{/*{{{*/
  _this.m_objects.back().to_dict().insert({
    _this.m_strings.back(),_this.m_values.back()});

  _this.m_strings.pop_back();
  _this.m_values.pop_back();

  return true;
}/*}}}*/

bool parser_c::pa_json_array_begin(parser_c &_this)
{/*{{{*/
  _this.m_arrays.push_back(array_t{});
  return true;
}/*}}}*/

bool parser_c::pa_json_array_value(parser_c &_this)
{/*{{{*/
  _this.m_arrays.back().to_array().push_back(_this.m_values.back());
  _this.m_values.pop_back();

  return true;
}/*}}}*/

bool parser_c::pa_json_val_string(parser_c &_this)
{/*{{{*/
  _this.m_values.push_back(_this.m_strings.back());
  _this.m_strings.pop_back();

  return true;
}/*}}}*/

bool parser_c::pa_json_val_integer(parser_c &_this)
{/*{{{*/
  stack_elem_c &lse = _this.m_lalr_stack.back();
  const char *int_num_data = _this.m_source.data() + lse.m_term_start;

  // - retrieve number from string -
  int64_t const_int = strtoll(int_num_data,nullptr,10);

  auto map_iter = _this.m_integer_map.find(const_int);
  if (map_iter == _this.m_integer_map.end())
  {
    map_iter = _this.m_integer_map.insert({const_int,const_int}).first;
  }

  _this.m_values.push_back(map_iter->second);

  return true;
}/*}}}*/

bool parser_c::pa_json_val_float(parser_c &_this)
{/*{{{*/
  stack_elem_c &lse = _this.m_lalr_stack.back();
  const char *float_num_data = _this.m_source.data() + lse.m_term_start;

  // - retrieve number from string -
  double const_float = strtod(float_num_data,nullptr);

  auto map_iter = _this.m_double_map.find(const_float);
  if (map_iter == _this.m_double_map.end())
  {
    map_iter = _this.m_double_map.insert({const_float,const_float}).first;
  }

  _this.m_values.push_back(map_iter->second);

  return true;
}/*}}}*/

bool parser_c::pa_json_val_object(parser_c &_this)
{/*{{{*/
  _this.m_values.push_back(_this.m_objects.back());
  _this.m_objects.pop_back();

  return true;
}/*}}}*/

bool parser_c::pa_json_val_array(parser_c &_this)
{/*{{{*/
  _this.m_values.push_back(_this.m_arrays.back());
  _this.m_arrays.pop_back();

  return true;
}/*}}}*/

bool parser_c::pa_json_val_true(parser_c &_this)
{/*{{{*/
  _this.m_values.push_back(_this.m_var_true);
  return true;
}/*}}}*/

bool parser_c::pa_json_val_false(parser_c &_this)
{/*{{{*/
  _this.m_values.push_back(_this.m_var_false);
  return true;
}/*}}}*/

bool parser_c::pa_json_val_null(parser_c &_this)
{/*{{{*/
  _this.m_values.push_back(_this.m_var_null);
  return true;
}/*}}}*/

bool parser_c::pa_json_string(parser_c &_this)
{/*{{{*/
  stack_elem_c &lse = _this.m_lalr_stack.back();

  auto iter = _this.m_source.data() + lse.m_term_start + 1;
  auto iter_end = _this.m_source.data() + lse.m_term_end - 1;

  int buff_size = iter_end - iter;
  char buff[buff_size];

  std::stringbuf char_buffer;
  char_buffer.pubsetbuf(buff,buff_size);

  if (iter < iter_end)
  {
    do
    {
      if (*iter == '\\')
      {
        iter++;

        // - process character represented by unicode number -
        if (*iter == 'u')
        {
          iter++;

          unsigned value = 0;

          // - retrieve character value -
          auto iter_end = iter + 4;
          do {
            value <<= 4;

            if (*iter >= '0' && *iter <= '9')
            {
              value += *iter - '0';
            }
            else if (*iter >= 'a' && *iter <= 'f')
            {
              value += 10 + (*iter - 'a');
            }
            else if (*iter >= 'A' && *iter <= 'F')
            {
              value += 10 + (*iter - 'A');
            }
            else
            {
              assert(false);
            }

          } while(++iter < iter_end);

          // - convert utf16/32 value to utf8 character string -
          if (value <= 0x7f)
          {
            char_buffer.sputc(value);
          }
          else if (value <= 0x7ff)
          {
            char_buffer.sputc(0xc0 | value >> 6);
            char_buffer.sputc(0x80 | (value & 0x3f));
          }
          else if (value <= 0xffff)
          {
            char_buffer.sputc(0xe0 |   value >> 12);
            char_buffer.sputc(0x80 | ((value >>  6) & 0x3f));
            char_buffer.sputc(0x80 |  (value        & 0x3f));
          }
        }
        else
        {
          switch (*iter++)
          {
          case 'b':
            char_buffer.sputc('\b');
            break;
          case 'f':
            char_buffer.sputc('\f');
            break;
          case 'n':
            char_buffer.sputc('\n');
            break;
          case 'r':
            char_buffer.sputc('\r');
            break;
          case 't':
            char_buffer.sputc('\t');
            break;
          case '\\':
            char_buffer.sputc('\\');
            break;
          case '"':
            char_buffer.sputc('"');
            break;
          default:
            assert(false);
          }
        }
      }
      else
      {
        char_buffer.sputc(*iter++);
      }
    }
    while(iter != iter_end);
  }

  std::string const_str(char_buffer.str());

  auto map_iter = _this.m_string_map.find(const_str);
  if (map_iter == _this.m_string_map.end())
  {
    map_iter = _this.m_string_map.insert({const_str,const_str}).first;
  }

  _this.m_strings.push_back(map_iter->second);

  return true;
}/*}}}*/

} // namespace ccljson

