
#include "ccljson.h"

namespace ccljson {

const uint32_t c_idx_not_exist = std::numeric_limits<uint32_t>::max();

const uint32_t c_json_rule_cnt = 24;
const uint32_t json_rule_head_idxs[c_json_rule_cnt] = {14, 15, 16, 17, 17, 18, 19, 19, 20, 21, 22, 22, 23, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, };
const uint32_t json_rule_body_lengths[c_json_rule_cnt] = {1, 2, 2, 1, 2, 1, 3, 1, 3, 2, 1, 2, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, };

#define json_blank c_idx_not_exist
#define JSON_SHIFT(VALUE) VALUE
#define JSON_REDUCE(VALUE) (c_json_lalr_table_reduce_base + (VALUE))
#define JSON_GOTO(VALUE) VALUE

const uint32_t c_json_lalr_table_reduce_base = 0x80000000;
const uint32_t c_json_terminal_plus_nonterminal_cnt = 27;
const uint32_t json_lalr_state_cnt = 32;

const uint32_t json_lalr_table[json_lalr_state_cnt*c_json_terminal_plus_nonterminal_cnt] =
{/*{{{*/
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,    JSON_SHIFT(5),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,     JSON_GOTO(1),     JSON_GOTO(2),     JSON_GOTO(3),     JSON_GOTO(4),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,    JSON_SHIFT(6),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,    JSON_SHIFT(7),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
   JSON_SHIFT(11),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,   JSON_REDUCE(3),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,     JSON_GOTO(8),     JSON_GOTO(9),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,    JSON_GOTO(10),
   JSON_REDUCE(5),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,   JSON_REDUCE(5),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,   JSON_REDUCE(2),       json_blank,   JSON_REDUCE(2),   JSON_REDUCE(2),       json_blank,       json_blank,   JSON_REDUCE(2),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,   JSON_REDUCE(4),       json_blank,       json_blank,   JSON_SHIFT(12),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,   JSON_REDUCE(7),       json_blank,       json_blank,   JSON_REDUCE(7),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,   JSON_SHIFT(13),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(23),       json_blank,  JSON_REDUCE(23),  JSON_REDUCE(23),  JSON_REDUCE(23),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
   JSON_SHIFT(11),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,    JSON_GOTO(14),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,    JSON_GOTO(10),
   JSON_SHIFT(11),   JSON_SHIFT(17),   JSON_SHIFT(18),   JSON_SHIFT(21),   JSON_SHIFT(22),   JSON_SHIFT(23),    JSON_SHIFT(5),       json_blank,   JSON_SHIFT(26),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,    JSON_GOTO(19),     JSON_GOTO(3),     JSON_GOTO(4),       json_blank,       json_blank,    JSON_GOTO(20),    JSON_GOTO(24),    JSON_GOTO(25),       json_blank,    JSON_GOTO(15),    JSON_GOTO(16),
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,   JSON_REDUCE(6),       json_blank,       json_blank,   JSON_REDUCE(6),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,   JSON_REDUCE(8),       json_blank,       json_blank,   JSON_REDUCE(8),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(15),       json_blank,  JSON_REDUCE(15),  JSON_REDUCE(15),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(16),       json_blank,  JSON_REDUCE(16),  JSON_REDUCE(16),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(17),       json_blank,  JSON_REDUCE(17),  JSON_REDUCE(17),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(18),       json_blank,  JSON_REDUCE(18),  JSON_REDUCE(18),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(19),       json_blank,  JSON_REDUCE(19),  JSON_REDUCE(19),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(20),       json_blank,  JSON_REDUCE(20),  JSON_REDUCE(20),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(21),       json_blank,  JSON_REDUCE(21),  JSON_REDUCE(21),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(22),       json_blank,  JSON_REDUCE(22),  JSON_REDUCE(22),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,   JSON_SHIFT(27),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
   JSON_SHIFT(11),   JSON_SHIFT(17),   JSON_SHIFT(18),   JSON_SHIFT(21),   JSON_SHIFT(22),   JSON_SHIFT(23),    JSON_SHIFT(5),       json_blank,   JSON_SHIFT(26),  JSON_REDUCE(10),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,    JSON_GOTO(19),     JSON_GOTO(3),     JSON_GOTO(4),       json_blank,       json_blank,    JSON_GOTO(20),    JSON_GOTO(24),    JSON_GOTO(25),    JSON_GOTO(28),    JSON_GOTO(29),    JSON_GOTO(16),
  JSON_REDUCE(12),  JSON_REDUCE(12),  JSON_REDUCE(12),  JSON_REDUCE(12),  JSON_REDUCE(12),  JSON_REDUCE(12),  JSON_REDUCE(12),       json_blank,  JSON_REDUCE(12),  JSON_REDUCE(12),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,   JSON_REDUCE(9),       json_blank,   JSON_REDUCE(9),   JSON_REDUCE(9),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(11),   JSON_SHIFT(30),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(14),  JSON_REDUCE(14),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
   JSON_SHIFT(11),   JSON_SHIFT(17),   JSON_SHIFT(18),   JSON_SHIFT(21),   JSON_SHIFT(22),   JSON_SHIFT(23),    JSON_SHIFT(5),       json_blank,   JSON_SHIFT(26),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,    JSON_GOTO(19),     JSON_GOTO(3),     JSON_GOTO(4),       json_blank,       json_blank,    JSON_GOTO(20),    JSON_GOTO(24),    JSON_GOTO(25),       json_blank,    JSON_GOTO(31),    JSON_GOTO(16),
       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,  JSON_REDUCE(13),  JSON_REDUCE(13),       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,       json_blank,
};/*}}}*/

std::stringbuf &append_string(std::stringbuf &a_strbuf,const std::string &a_string)
{/*{{{*/
  if (!a_string.empty())
  {
    auto iter = a_string.begin();
    do {
      switch (*iter)
      {
        case '\b':
          a_strbuf.sputc('\\');
          a_strbuf.sputc('b');
          break;
        case '\f':
          a_strbuf.sputc('\\');
          a_strbuf.sputc('f');
          break;
        case '\n':
          a_strbuf.sputc('\\');
          a_strbuf.sputc('n');
          break;
        case '\r':
          a_strbuf.sputc('\\');
          a_strbuf.sputc('r');
          break;
        case '\t':
          a_strbuf.sputc('\\');
          a_strbuf.sputc('t');
          break;
        case '\\':
          a_strbuf.sputc('\\');
          a_strbuf.sputc('\\');
          break;
        case '"':
          a_strbuf.sputc('\\');
          a_strbuf.sputc('"');
          break;
        default:
          a_strbuf.sputc(*iter);
      }
    } while(++iter != a_string.end());
  }

  return a_strbuf;
}/*}}}*/

std::string create(var_c a_var)
{/*{{{*/
  std::stringbuf strbuf;
  std::ostream os(&strbuf);

  // - configure stream to show floating point -
  os.setf(std::ios::fixed,std::ios::floatfield);
  os.precision(6);

  std::vector<create_elem_c> create_stack;
  create_stack.emplace_back(a_var);

  do {
    create_elem_c &cs_elm = create_stack.back();

    switch (cs_elm.m_var.type())
    {
    case cclvar::type_blank:
      os << "null";
      create_stack.pop_back();
      break;
    case cclvar::type_bool:
      os << (cs_elm.m_var.to_bool() ? "true" : "false");
      create_stack.pop_back();
      break;
    case cclvar::type_integer:
      os << cs_elm.m_var.to_int();
      create_stack.pop_back();
      break;
    case cclvar::type_float:
      os << cs_elm.m_var.to_float();
      create_stack.pop_back();
      break;
    case cclvar::type_string:
      strbuf.sputc('"');
      append_string(strbuf,cs_elm.m_var.to_str());
      strbuf.sputc('"');
      create_stack.pop_back();
      break;
    case cclvar::type_array:
      {/*{{{*/
        array_t &array = cs_elm.m_var.to_array();

        if (cs_elm.m_init)
        {
          strbuf.sputc('[');

          cs_elm.array_i = array.begin();
          cs_elm.m_init = false;
        }
        else
        {
          if (cs_elm.array_i != array.end())
          {
            strbuf.sputc(',');
          }
        }

        if (cs_elm.array_i != array.end())
        {
          create_stack.emplace_back(*cs_elm.array_i++);
        }
        else
        {
          strbuf.sputc(']');
          create_stack.pop_back();
        }
      }/*}}}*/
      break;
    case cclvar::type_list:
      {/*{{{*/
        list_t &list = cs_elm.m_var.to_list();

        if (cs_elm.m_init)
        {
          strbuf.sputc('[');

          cs_elm.list_i = list.begin();
          cs_elm.m_init = false;
        }
        else
        {
          if (cs_elm.list_i != list.end())
          {
            strbuf.sputc(',');
          }
        }

        if (cs_elm.list_i != list.end())
        {
          create_stack.emplace_back(*cs_elm.list_i++);
        }
        else
        {
          strbuf.sputc(']');
          create_stack.pop_back();
        }
      }/*}}}*/
      break;
    case cclvar::type_dict:
      {/*{{{*/
        dict_t &dict = cs_elm.m_var.to_dict();

        if (cs_elm.m_init)
        {
          strbuf.sputc('{');

          cs_elm.dict_i = dict.begin();
          cs_elm.m_init = false;
        }
        else
        {
          if (cs_elm.dict_i != dict.end())
          {
            strbuf.sputc(',');
          }
        }

        if (cs_elm.dict_i != dict.end())
        {
          auto &pair = *(cs_elm.dict_i++);

          // - ERROR -
          if (pair.first.type() != cclvar::type_string)
          {
            cclthrow(error_JSON_CREATE_NO_STRING_DICT_KEY);
          }

          strbuf.sputc('"');
          append_string(strbuf,pair.first.to_str());
          strbuf.sputc('"');
          strbuf.sputc(':');

          create_stack.emplace_back(pair.second);
        }
        else
        {
          strbuf.sputc('}');
          create_stack.pop_back();
        }
      }/*}}}*/
      break;

    // - ERROR -
    default:

      cclthrow(error_JSON_CREATE_UNSUPPORTED_VARIABLE_TYPE);
    }

  } while(!create_stack.empty());

  return strbuf.str();
}/*}}}*/

std::string create_nice(var_c a_var,const std::string &a_tab_str)
{/*{{{*/
  std::stringbuf strbuf;
  std::ostream os(&strbuf);

  // - configure stream to show floating point -
  os.setf(std::ios::fixed,std::ios::floatfield);
  os.precision(6);

  std::vector<create_elem_c> create_stack;
  create_stack.emplace_back(a_var);

  std::string indent_buffer;
  std::string::size_type indent_size = 0;

  #define CCLJSON_CREATE_NICE_PUSH_TAB() \
  {/*{{{*/\
    if ((indent_size += a_tab_str.length()) > indent_buffer.length())\
    {\
      indent_buffer += a_tab_str;\
    }\
  };/*}}}*/

  #define CCLJSON_CREATE_NICE_POP_TAB() \
  {/*{{{*/\
    indent_size -= a_tab_str.length();\
  };/*}}}*/

  #define CCLJSON_CREATE_NICE_INDENT() \
  {/*{{{*/\
    strbuf.sputc('\n');\
    strbuf.sputn(indent_buffer.data(),indent_size);\
  };/*}}}*/

  do {
    create_elem_c &cs_elm = create_stack.back();

    switch (cs_elm.m_var.type())
    {
    case cclvar::type_blank:
      os << "null";
      create_stack.pop_back();
      break;
    case cclvar::type_bool:
      os << (cs_elm.m_var.to_bool() ? "true" : "false");
      create_stack.pop_back();
      break;
    case cclvar::type_integer:
      os << cs_elm.m_var.to_int();
      create_stack.pop_back();
      break;
    case cclvar::type_float:
      os << cs_elm.m_var.to_float();
      create_stack.pop_back();
      break;
    case cclvar::type_string:
      strbuf.sputc('"');
      append_string(strbuf,cs_elm.m_var.to_str());
      strbuf.sputc('"');
      create_stack.pop_back();
      break;
    case cclvar::type_array:
      {/*{{{*/
        array_t &array = cs_elm.m_var.to_array();

        if (cs_elm.m_init)
        {
          strbuf.sputc('[');

          if (!array.empty())
          {
            CCLJSON_CREATE_NICE_PUSH_TAB();
            CCLJSON_CREATE_NICE_INDENT();
          }

          cs_elm.array_i = array.begin();
          cs_elm.m_init = false;
        }
        else
        {
          if (cs_elm.array_i != array.end())
          {
            strbuf.sputc(',');
            CCLJSON_CREATE_NICE_INDENT();
          }
        }

        if (cs_elm.array_i != array.end())
        {
          create_stack.emplace_back(*cs_elm.array_i++);
        }
        else
        {
          if (!array.empty())
          {
            CCLJSON_CREATE_NICE_POP_TAB();
            CCLJSON_CREATE_NICE_INDENT();
          }

          strbuf.sputc(']');
          create_stack.pop_back();
        }
      }/*}}}*/
      break;
    case cclvar::type_list:
      {/*{{{*/
        list_t &list = cs_elm.m_var.to_list();

        if (cs_elm.m_init)
        {
          strbuf.sputc('[');

          if (!list.empty())
          {
            CCLJSON_CREATE_NICE_PUSH_TAB();
            CCLJSON_CREATE_NICE_INDENT();
          }

          cs_elm.list_i = list.begin();
          cs_elm.m_init = false;
        }
        else
        {
          if (cs_elm.list_i != list.end())
          {
            strbuf.sputc(',');
            CCLJSON_CREATE_NICE_INDENT();
          }
        }

        if (cs_elm.list_i != list.end())
        {
          create_stack.emplace_back(*cs_elm.list_i++);
        }
        else
        {
          if (!list.empty())
          {
            CCLJSON_CREATE_NICE_POP_TAB();
            CCLJSON_CREATE_NICE_INDENT();
          }

          strbuf.sputc(']');
          create_stack.pop_back();
        }
      }/*}}}*/
      break;
    case cclvar::type_dict:
      {/*{{{*/
        dict_t &dict = cs_elm.m_var.to_dict();

        if (cs_elm.m_init)
        {
          strbuf.sputc('{');

          if (!dict.empty())
          {
            CCLJSON_CREATE_NICE_PUSH_TAB();
            CCLJSON_CREATE_NICE_INDENT();
          }

          cs_elm.dict_i = dict.begin();
          cs_elm.m_init = false;
        }
        else
        {
          if (cs_elm.dict_i != dict.end())
          {
            strbuf.sputc(',');
            CCLJSON_CREATE_NICE_INDENT();
          }
        }

        if (cs_elm.dict_i != dict.end())
        {
          auto &pair = *(cs_elm.dict_i++);

          // - ERROR -
          if (pair.first.type() != cclvar::type_string)
          {
            cclthrow(error_JSON_CREATE_NO_STRING_DICT_KEY);
          }

          strbuf.sputc('"');
          append_string(strbuf,pair.first.to_str());
          strbuf.sputc('"');
          strbuf.sputc(':');

          create_stack.emplace_back(pair.second);
        }
        else
        {
          if (!dict.empty())
          {
            CCLJSON_CREATE_NICE_POP_TAB();
            CCLJSON_CREATE_NICE_INDENT();
          }

          strbuf.sputc('}');
          create_stack.pop_back();
        }
      }/*}}}*/
      break;

    // - ERROR -
    default:

      cclthrow(error_JSON_CREATE_UNSUPPORTED_VARIABLE_TYPE);
    }

  } while(!create_stack.empty());

  return strbuf.str();
}/*}}}*/

var_c parse(const std::string &a_source)
{/*{{{*/
  parser_c parser{a_source};
  return parser.parse_source();
}/*}}}*/

uint32_t parser_c::recognize_terminal(uint32_t &a_input_idx)
{/*{{{*/

#define JSON_GET_NEXT_CHAR() \
  if (a_input_idx < m_source->length()) {\
    in_char = static_cast<uint8_t>((*m_source)[a_input_idx]);\
  }\
  else {\
    in_char = '\0';\
  }

#define JSON_CLOSE_CHAR(RET_TERM_IDX) \
  if (in_char == '\0') {\
    return RET_TERM_IDX;\
  }\
  \
  a_input_idx++;

   uint16_t in_char;

// - STATE 0 -
   JSON_GET_NEXT_CHAR();

   if (in_char == 0) {
      goto state_1_label;
    }

   if (in_char >= 8 && in_char < 14) {
      goto state_15_label;
    }

   if (in_char == 32) {
      goto state_15_label;
    }

   if (in_char == 34) {
      goto state_2_label;
    }

   if (in_char == 44) {
      goto state_3_label;
    }

   if (in_char == 45) {
      goto state_4_label;
    }

   if (in_char == 48) {
      goto state_5_label;
    }

   if (in_char >= 49 && in_char < 58) {
      goto state_6_label;
    }

   if (in_char == 58) {
      goto state_7_label;
    }

   if (in_char == 91) {
      goto state_8_label;
    }

   if (in_char == 93) {
      goto state_9_label;
    }

   if (in_char == 102) {
      goto state_10_label;
    }

   if (in_char == 110) {
      goto state_11_label;
    }

   if (in_char == 116) {
      goto state_12_label;
    }

   if (in_char == 123) {
      goto state_13_label;
    }

   if (in_char == 125) {
      goto state_14_label;
    }

   return c_idx_not_exist;

// - STATE 1 -
state_1_label:
   JSON_CLOSE_CHAR(13)
   return 13;

// - STATE 2 -
state_2_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char < 34) {
      goto state_2_label;
    }

   if (in_char == 34) {
      goto state_16_label;
    }

   if (in_char >= 35 && in_char < 92) {
      goto state_2_label;
    }

   if (in_char == 92) {
      goto state_17_label;
    }

   if (in_char >= 93 && in_char < 256) {
      goto state_2_label;
    }

   return c_idx_not_exist;

// - STATE 3 -
state_3_label:
   JSON_CLOSE_CHAR(10)
   return 10;

// - STATE 4 -
state_4_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 48) {
      goto state_5_label;
    }

   if (in_char >= 49 && in_char < 58) {
      goto state_6_label;
    }

   return c_idx_not_exist;

// - STATE 5 -
state_5_label:
   JSON_CLOSE_CHAR(1)
   JSON_GET_NEXT_CHAR();

   if (in_char == 46) {
      goto state_18_label;
    }

   if (in_char == 69) {
      goto state_19_label;
    }

   if (in_char == 101) {
      goto state_19_label;
    }

   return 1;

// - STATE 6 -
state_6_label:
   JSON_CLOSE_CHAR(1)
   JSON_GET_NEXT_CHAR();

   if (in_char == 46) {
      goto state_18_label;
    }

   if (in_char >= 48 && in_char < 58) {
      goto state_6_label;
    }

   if (in_char == 69) {
      goto state_19_label;
    }

   if (in_char == 101) {
      goto state_19_label;
    }

   return 1;

// - STATE 7 -
state_7_label:
   JSON_CLOSE_CHAR(11)
   return 11;

// - STATE 8 -
state_8_label:
   JSON_CLOSE_CHAR(8)
   return 8;

// - STATE 9 -
state_9_label:
   JSON_CLOSE_CHAR(9)
   return 9;

// - STATE 10 -
state_10_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 97) {
      goto state_20_label;
    }

   return c_idx_not_exist;

// - STATE 11 -
state_11_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 117) {
      goto state_21_label;
    }

   return c_idx_not_exist;

// - STATE 12 -
state_12_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 114) {
      goto state_22_label;
    }

   return c_idx_not_exist;

// - STATE 13 -
state_13_label:
   JSON_CLOSE_CHAR(6)
   return 6;

// - STATE 14 -
state_14_label:
   JSON_CLOSE_CHAR(7)
   return 7;

// - STATE 15 -
state_15_label:
   JSON_CLOSE_CHAR(12)
   JSON_GET_NEXT_CHAR();

   if (in_char >= 8 && in_char < 14) {
      goto state_15_label;
    }

   if (in_char == 32) {
      goto state_15_label;
    }

   return 12;

// - STATE 16 -
state_16_label:
   JSON_CLOSE_CHAR(0)
   return 0;

// - STATE 17 -
state_17_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 34) {
      goto state_2_label;
    }

   if (in_char == 47) {
      goto state_2_label;
    }

   if (in_char == 92) {
      goto state_2_label;
    }

   if (in_char == 98) {
      goto state_2_label;
    }

   if (in_char == 102) {
      goto state_2_label;
    }

   if (in_char == 110) {
      goto state_2_label;
    }

   if (in_char == 114) {
      goto state_2_label;
    }

   if (in_char == 116) {
      goto state_2_label;
    }

   if (in_char == 117) {
      goto state_23_label;
    }

   return c_idx_not_exist;

// - STATE 18 -
state_18_label:
   JSON_CLOSE_CHAR(2)
   JSON_GET_NEXT_CHAR();

   if (in_char >= 48 && in_char < 58) {
      goto state_18_label;
    }

   if (in_char == 69) {
      goto state_19_label;
    }

   if (in_char == 101) {
      goto state_19_label;
    }

   return 2;

// - STATE 19 -
state_19_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 43) {
      goto state_24_label;
    }

   if (in_char == 45) {
      goto state_24_label;
    }

   if (in_char >= 48 && in_char < 58) {
      goto state_25_label;
    }

   return c_idx_not_exist;

// - STATE 20 -
state_20_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 108) {
      goto state_26_label;
    }

   return c_idx_not_exist;

// - STATE 21 -
state_21_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 108) {
      goto state_27_label;
    }

   return c_idx_not_exist;

// - STATE 22 -
state_22_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 117) {
      goto state_28_label;
    }

   return c_idx_not_exist;

// - STATE 23 -
state_23_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char >= 48 && in_char < 58) {
      goto state_29_label;
    }

   if (in_char >= 65 && in_char < 71) {
      goto state_29_label;
    }

   if (in_char >= 97 && in_char < 103) {
      goto state_29_label;
    }

   return c_idx_not_exist;

// - STATE 24 -
state_24_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char >= 48 && in_char < 58) {
      goto state_25_label;
    }

   return c_idx_not_exist;

// - STATE 25 -
state_25_label:
   JSON_CLOSE_CHAR(2)
   JSON_GET_NEXT_CHAR();

   if (in_char >= 48 && in_char < 58) {
      goto state_25_label;
    }

   return 2;

// - STATE 26 -
state_26_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 115) {
      goto state_30_label;
    }

   return c_idx_not_exist;

// - STATE 27 -
state_27_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 108) {
      goto state_31_label;
    }

   return c_idx_not_exist;

// - STATE 28 -
state_28_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 101) {
      goto state_32_label;
    }

   return c_idx_not_exist;

// - STATE 29 -
state_29_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char >= 48 && in_char < 58) {
      goto state_33_label;
    }

   if (in_char >= 65 && in_char < 71) {
      goto state_33_label;
    }

   if (in_char >= 97 && in_char < 103) {
      goto state_33_label;
    }

   return c_idx_not_exist;

// - STATE 30 -
state_30_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char == 101) {
      goto state_34_label;
    }

   return c_idx_not_exist;

// - STATE 31 -
state_31_label:
   JSON_CLOSE_CHAR(5)
   return 5;

// - STATE 32 -
state_32_label:
   JSON_CLOSE_CHAR(3)
   return 3;

// - STATE 33 -
state_33_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char >= 48 && in_char < 58) {
      goto state_35_label;
    }

   if (in_char >= 65 && in_char < 71) {
      goto state_35_label;
    }

   if (in_char >= 97 && in_char < 103) {
      goto state_35_label;
    }

   return c_idx_not_exist;

// - STATE 34 -
state_34_label:
   JSON_CLOSE_CHAR(4)
   return 4;

// - STATE 35 -
state_35_label:
   JSON_CLOSE_CHAR(c_idx_not_exist);
   JSON_GET_NEXT_CHAR();

   if (in_char >= 48 && in_char < 58) {
      goto state_2_label;
    }

   if (in_char >= 65 && in_char < 71) {
      goto state_2_label;
    }

   if (in_char >= 97 && in_char < 103) {
      goto state_2_label;
    }

   return c_idx_not_exist;

}/*}}}*/

var_c parser_c::parse_source()
{/*{{{*/
  m_old_input_idx = 0;

  m_lalr_stack.clear();
  m_lalr_stack.emplace_back(0);

  uint32_t input_idx = 0;
  uint32_t ret_term = c_idx_not_exist;

  do
  {
    // - retrieve next terminal symbol -
    while (ret_term == c_idx_not_exist)
    {
      m_old_input_idx = input_idx;
      ret_term = recognize_terminal(input_idx);

      // - PARSE ERROR unrecognized terminal -
      if (ret_term == c_idx_not_exist)
      {
        cclthrow(error_JSON_PARSE_ERROR_UNRECOGNIZED_TERMINAL);
      }

      // - skipping of _SKIP_ terminals -
      if (ret_term == 12)
      {
        ret_term = c_idx_not_exist;
      }
    }

    // - retrieve action from table of actions -
    uint32_t parse_action = json_lalr_table[m_lalr_stack.back().m_state*c_json_terminal_plus_nonterminal_cnt + ret_term];

    // - PARSE ERROR wrong syntax -
    if (parse_action == c_idx_not_exist)
    {
      cclthrow(error_JSON_PARSE_ERROR_INVALID_SYNTAX);
    }

    // - action JSON_SHIFT -
    if (parse_action < c_json_lalr_table_reduce_base)
    {
      if (ret_term == 13)
      {
        break;
      }

      // - insertion of state, and terminal position in source string -
      m_lalr_stack.emplace_back(parse_action,m_old_input_idx,input_idx);
      ret_term = c_idx_not_exist;
    }

    // - action JSON_REDUCE-
    else
    {
      parse_action -= c_json_lalr_table_reduce_base;

      // - call function assigned to reduction -
      if (json_pa_callers[parse_action] != pa_json_null)
      {
        if (!json_pa_callers[parse_action](*this))
        {
          cclthrow(error_JSON_PARSE_ERROR);
        }
      }

      // - remove rule body from m_lalr_stack -
      m_lalr_stack.resize(m_lalr_stack.size() - json_rule_body_lengths[parse_action]);

      // - insert new automat state to stack -
      uint32_t goto_val = json_lalr_table[m_lalr_stack.back().m_state*c_json_terminal_plus_nonterminal_cnt + json_rule_head_idxs[parse_action]];
      m_lalr_stack.emplace_back(goto_val);
    }
  }
  while(true);

  return m_objects.back();
}/*}}}*/

} // namespace ccljson

