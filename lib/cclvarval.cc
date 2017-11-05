
#include "cclvarval.h"

typedef cclvar::dict_pair_t dp_t;

namespace cclvarval {

string_map_t validator_c::c_type_map
{/*{{{*/
  {"blank"  ,cclvar::type_blank},
  {"bool"   ,cclvar::type_bool},
  {"integer",cclvar::type_integer},
  {"float"  ,cclvar::type_float},
  {"string" ,cclvar::type_string},
  {"array"  ,cclvar::type_array},
  {"list"   ,cclvar::type_list},
  {"dict"   ,cclvar::type_dict},
};/*}}}*/

string_map_t validator_c::c_prop_map
{/*{{{*/
  {"type"     ,prop_type},
  {"=="       ,prop_equal},
  {"!="       ,prop_not_equal},
  {"<"        ,prop_lesser},
  {">"        ,prop_greater},
  {"<="       ,prop_lesser_equal},
  {">="       ,prop_greater_equal},
  {"size =="  ,prop_size_equal},
  {"size !="  ,prop_size_not_equal},
  {"size <"   ,prop_size_lesser},
  {"size >"   ,prop_size_greater},
  {"size <="  ,prop_size_lesser_equal},
  {"size >="  ,prop_size_greater_equal},
  {"items"    ,prop_items},
  {"opt-items",prop_opt_items},
  {"all-keys" ,prop_all_keys},
  {"all-items",prop_all_items},
};/*}}}*/

#define VALIDATE_PAIR_CALL(VALUE,PROPS_REF,INVALID_CODE) \
{/*{{{*/\
  if ((PROPS_REF).type() == cclvar::type_string)\
  {\
    cclvar::var_c ref;\
    if (m_schema.has_key(PROPS_REF,ref))\
    {\
      try { validate_pair(VALUE,ref); }\
      catch (...)\
      {\
        m_props_stack.push_back(PROPS_REF);\
        m_props_stack.push_back("ref");\
        \
        INVALID_CODE;\
        std::rethrow_exception(std::current_exception());\
      }\
    }\
    else\
    {\
      INVALID_CODE;\
      cclthrow(error_VARVAL_INVALID_SCHEMA_REFERENCE);\
    }\
  }\
  else\
  {\
    try { validate_pair(VALUE,PROPS_REF); }\
    catch (...)\
    {\
      INVALID_CODE;\
      std::rethrow_exception(std::current_exception());\
    }\
  }\
}/*}}}*/

void validator_c::validate_pair(cclvar::var_c a_value,cclvar::var_c a_props)
{/*{{{*/
  cclvar::dict_t &props_dict = a_props.to_dict();

  // - process all properties -
  for (auto prop_i = props_dict.begin();
            prop_i != props_dict.end();
          ++prop_i)
  {
    int prop_id = c_prop_map[prop_i->first.to_str()];

    switch (prop_id)
    {
    case prop_type:
      if (a_value.type() != c_type_map[prop_i->second.to_str()])
      {
        cclthrow(error_VARVAL_INVALID_TYPE);
      }
      break;
    case prop_equal:
      if (a_value != prop_i->second)
      {
        cclthrow(error_VARVAL_INVALID_VALUE);
      }
      break;
    case prop_not_equal:
      if (a_value == prop_i->second)
      {
        cclthrow(error_VARVAL_INVALID_VALUE);
      }
      break;
    case prop_lesser:
      if (a_value.compare(prop_i->second) >= 0)
      {
        cclthrow(error_VARVAL_INVALID_VALUE);
      }
      break;
    case prop_greater:
      if (a_value.compare(prop_i->second) <= 0)
      {
        cclthrow(error_VARVAL_INVALID_VALUE);
      }
      break;
    case prop_lesser_equal:
      if (a_value.compare(prop_i->second) > 0)
      {
        cclthrow(error_VARVAL_INVALID_VALUE);
      }
      break;
    case prop_greater_equal:
      if (a_value.compare(prop_i->second) < 0)
      {
        cclthrow(error_VARVAL_INVALID_VALUE);
      }
      break;
    case prop_size_equal:
    case prop_size_not_equal:
    case prop_size_lesser:
    case prop_size_greater:
    case prop_size_lesser_equal:
    case prop_size_greater_equal:
    {
      size_t value_size;
      switch (a_value.type())
      {
      case cclvar::type_string:
        value_size = a_value.to_str().size();
        break;
      case cclvar::type_array:
        value_size = a_value.to_array().size();
        break;
      case cclvar::type_list:
        value_size = a_value.to_list().size();
        break;
      case cclvar::type_dict:
        value_size = a_value.to_dict().size();
        break;

      // - ERROR -
      default:
        cclthrow(error_VARVAL_INVALID_TYPE);
      }

      int64_t prop_size = prop_i->second.to_int();

      switch (prop_id)
      {
        case prop_size_equal:
          if (value_size != prop_size)
          {
            cclthrow(error_VARVAL_VALUE_INVALID_SIZE);
          }
          break;
        case prop_size_not_equal:
          if (value_size == prop_size)
          {
            cclthrow(error_VARVAL_VALUE_INVALID_SIZE);
          }
          break;
        case prop_size_lesser:
          if (value_size >= prop_size)
          {
            cclthrow(error_VARVAL_VALUE_INVALID_SIZE);
          }
          break;
        case prop_size_greater:
          if (value_size <= prop_size)
          {
            cclthrow(error_VARVAL_VALUE_INVALID_SIZE);
          }
          break;
        case prop_size_lesser_equal:
          if (value_size > prop_size)
          {
            cclthrow(error_VARVAL_VALUE_INVALID_SIZE);
          }
          break;
        case prop_size_greater_equal:
          if (value_size < prop_size)
          {
            cclthrow(error_VARVAL_VALUE_INVALID_SIZE);
          }
          break;
      }
    }
    break;
    case prop_items:
      switch (a_value.type())
      {
      case cclvar::type_array:
      {/*{{{*/
        cclvar::dict_t &items_dict = prop_i->second.to_dict();
        cclvar::array_t &value_arr = a_value.to_array();

        for (auto index_i = items_dict.begin();
                  index_i != items_dict.end();
                ++index_i)
        {
          int64_t index = index_i->first.to_int();
          
          // - ERROR -
          if (index < 0 || index >= value_arr.size())
          {
            m_value_stack.push_back(index_i->first);
            m_props_stack.push_back(index_i->first);
            m_props_stack.push_back("items");

            cclthrow(error_VARVAL_ARRAY_INVALID_SIZE);
          }

          VALIDATE_PAIR_CALL(value_arr[index],index_i->second,
            m_value_stack.push_back(index_i->first);
            m_props_stack.push_back(index_i->first);
            m_props_stack.push_back("items");
          );
        }
      }/*}}}*/
      break;
      case cclvar::type_dict:
      {/*{{{*/
        cclvar::dict_t &items_dict = prop_i->second.to_dict();

        for (auto key_i = items_dict.begin();
                  key_i != items_dict.end();
                ++key_i)
        {
          // - ERROR -
          cclvar::var_c key_value;
          if (!a_value.has_key(key_i->first,key_value))
          {
            m_value_stack.push_back(key_i->first);
            m_props_stack.push_back(key_i->first);
            m_props_stack.push_back("items");

            cclthrow(error_VARVAL_DICT_MISSING_KEY);
          }

          VALIDATE_PAIR_CALL(key_value,key_i->second,
            m_value_stack.push_back(key_i->first);
            m_props_stack.push_back(key_i->first);
            m_props_stack.push_back("items");
          );
        }
      }/*}}}*/
      break;

      // - ERROR -
      default:
        cclthrow(error_VARVAL_INVALID_TYPE);
      }
      break;
    case prop_opt_items:
      switch (a_value.type())
      {
      case cclvar::type_dict:
      {/*{{{*/
        cclvar::dict_t &opt_items_dict = prop_i->second.to_dict();

        for (auto key_i = opt_items_dict.begin();
                  key_i != opt_items_dict.end();
                ++key_i)
        {
          // - ERROR -
          cclvar::var_c key_value;
          if (a_value.has_key(key_i->first,key_value))
          {
            VALIDATE_PAIR_CALL(key_value,key_i->second,
              m_value_stack.push_back(key_i->first);
              m_props_stack.push_back(key_i->first);
              m_props_stack.push_back("opt-items");
            );
          }
        }
      }/*}}}*/
      break;

      // - ERROR -
      default:
        cclthrow(error_VARVAL_INVALID_TYPE);
      }
      break;
    case prop_all_keys:
      switch (a_value.type())
      {
      case cclvar::type_dict:
      {/*{{{*/
        cclvar::dict_t &value_dict = a_value.to_dict();

        for (auto pair_i = value_dict.begin();
                  pair_i != value_dict.end();
                ++pair_i)
        {
          VALIDATE_PAIR_CALL(pair_i->first,prop_i->second,
            m_value_stack.push_back(pair_i->first);
            m_props_stack.push_back(pair_i->first);
            m_props_stack.push_back("all-keys");
          );
        }
      }/*}}}*/
      break;

      // - ERROR -
      default:
        cclthrow(error_VARVAL_INVALID_TYPE);
      }
      break;
    case prop_all_items:
      switch (a_value.type())
      {
      case cclvar::type_array:
      {/*{{{*/
        cclvar::array_t &value_arr = a_value.to_array();

        for (auto item_i = value_arr.begin();
                  item_i != value_arr.end();
                  ++item_i)
        {
          VALIDATE_PAIR_CALL(*item_i,prop_i->second,
            m_value_stack.push_back(item_i - value_arr.begin());
            m_props_stack.push_back(item_i - value_arr.begin());
            m_props_stack.push_back("all-items");
          );
        }
      }/*}}}*/
      break;
      case cclvar::type_dict:
      {/*{{{*/
        cclvar::dict_t &value_dict = a_value.to_dict();

        for (auto pair_i = value_dict.begin();
                  pair_i != value_dict.end();
                  ++pair_i)
        {
          VALIDATE_PAIR_CALL(pair_i->second,prop_i->second,
            m_value_stack.push_back(pair_i->first);
            m_props_stack.push_back(pair_i->first);
            m_props_stack.push_back("all-items");
          );
        }
      }/*}}}*/
      break;
      // - ERROR -
      default:
        cclthrow(error_VARVAL_INVALID_TYPE);
      }
      break;

    // - ERROR -
    default:
      cclthrow(error_VARVAL_INVALID_SCHEMA_PROPERTY);
      break;
    }
  }
}/*}}}*/

void validator_c::validate(cclvar::var_c a_value)
{/*{{{*/
  m_value_stack = cclvar::array_t{};
  m_props_stack = cclvar::array_t{};

  try
  {
    // - validate root node -
    validate_pair(a_value,m_schema["root"]);
  }
  catch (std::exception &e)
  {
    m_props_stack.push_back("root");

    // - reverse values and props stacks -
    std::reverse(m_value_stack.begin(),m_value_stack.end());
    std::reverse(m_props_stack.begin(),m_props_stack.end());

    std::rethrow_exception(std::current_exception());
  }
}/*}}}*/

} // namespace cclvarval

