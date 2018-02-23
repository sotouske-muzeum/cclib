
#ifndef CCLIB_CCLXML_H
#define CCLIB_CCLXML_H

#include "cclbase.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <iostream>
#include <string>
#include <vector>

namespace cclxml {

// - exceptions list -
const std::string error_XML_DATA_PARSE_ERROR = "error while parsing xml data";

class xml_attr_c
{/*{{{*/
  private:
  std::string m_name;
  std::string m_value;

  public:
  xml_attr_c() = delete;
  xml_attr_c(std::string a_name,std::string a_value) : m_name(std::move(a_name)), m_value(std::move(a_value)) {}
  xml_attr_c(const xml_attr_c &) = delete;
  xml_attr_c(xml_attr_c &&a_xml_attr) throw() :
    m_name(std::move(a_xml_attr.m_name)),
    m_value(std::move(a_xml_attr.m_value)) {}
  void operator = (const xml_attr_c &) = delete;
  xml_attr_c &operator = (xml_attr_c &&a_xml_attr) throw()
  {/*{{{*/
    m_name = std::move(a_xml_attr.m_name);
    m_value = std::move(a_xml_attr.m_value);
    return *this;
  }/*}}}*/

  friend std::ostream &operator << (std::ostream &a_out,const xml_attr_c &a_this);
};/*}}}*/

class xml_node_c
{/*{{{*/
  private:
  bool m_self_close;
  std::string m_name;
  std::string m_text;
  std::vector<xml_attr_c> m_attrs;
  std::vector<xml_node_c> m_nodes;

  void attr(const std::string &a_name,const std::string &a_value);
  void text(const std::string &a_text);

  public:
  explicit xml_node_c(std::string a_name,std::string a_text = "",bool a_self_close = true) :
    m_self_close{a_self_close}, m_name(std::move(a_name)), m_text(std::move(a_text)) {}

  xml_node_c() = delete;
  xml_node_c(const xml_node_c &) = delete;
  xml_node_c(xml_node_c &&a_xml_node) throw() :
    m_self_close(a_xml_node.m_self_close),
    m_name(std::move(a_xml_node.m_name)),
    m_text(std::move(a_xml_node.m_text)),
    m_attrs(std::move(a_xml_node.m_attrs)),
    m_nodes(std::move(a_xml_node.m_nodes)) {}
  void operator = (const xml_node_c &) = delete;
  xml_node_c &operator = (xml_node_c &&a_xml_node) throw()
  {/*{{{*/
    m_self_close = a_xml_node.m_self_close;
    m_name = std::move(a_xml_node.m_name);
    m_text = std::move(a_xml_node.m_text);
    m_attrs = std::move(a_xml_node.m_attrs);
    m_nodes = std::move(a_xml_node.m_nodes);
    return *this;
  }/*}}}*/

  xml_node_c &node(const std::string &a_name,const std::string &a_text = "",bool a_self_close = true);
  xml_node_c &move_node(xml_node_c &&a_node) throw();
  xml_node_c &attr_(const std::string &a_name,const std::string &a_value)
  {/*{{{*/
    attr(a_name,a_value);
    return *this;
  }/*}}}*/
  xml_node_c &text_(const std::string &a_text)
  {/*{{{*/
    text(a_text);
    return *this;
  }/*}}}*/

  xml_node_c &tr() { return node("tr","",false); }
  xml_node_c &th() { return node("th","",false); }
  xml_node_c &td() { return node("td","",false); }
  xml_node_c &div() { return node("div","",false); }
  xml_node_c &span() { return node("span","",false); }

  friend std::ostream &operator << (std::ostream &a_out,const xml_node_c &a_this);
};/*}}}*/

class xml_parser_c
{/*{{{*/
  private:
  typedef std::vector<xml_node_c *> node_stack_t;

  node_stack_t node_stack;

  static void xml_start_document(void *user);
  static void xml_end_document(void *user);
  static void xml_start_element(void *user,const xmlChar *name,const xmlChar **attrs);
  static void xml_end_element(void *user,const xmlChar *name);
  static void xml_characters(void *user,const xmlChar *ch,int len);

  static void xml_warning(void *user,const char *msg,...);
  static void xml_error(void *user,const char *msg,...);
  static void xml_fatal_error(void *user,const char *msg,...);

  explicit xml_parser_c() = default;
  ~xml_parser_c();

  public:
  static xml_node_c parse(const std::string &a_data);
};/*}}}*/

class cclxml_c
{/*{{{*/
  public:
  cclxml_c() throw();
  ~cclxml_c();
};/*}}}*/

} // namespace cclxml

#endif

