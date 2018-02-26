
#include "cclxml.h"

namespace cclxml {

std::ostream &operator << (std::ostream &a_out,const xml_attr_c &a_this)
{/*{{{*/
  a_out << a_this.m_name << "=\"" << a_this.m_value << '"';
  return a_out;
}/*}}}*/

void xml_node_c::attr(const std::string &a_name,const std::string &a_value)
{/*{{{*/
  m_attrs.emplace_back(a_name,a_value);
}/*}}}*/

void xml_node_c::text(const std::string &a_text)
{/*{{{*/
  m_text += a_text;
}/*}}}*/

xml_node_c &xml_node_c::node(
  const std::string &a_name,const std::string &a_text,bool a_self_close)
{/*{{{*/
  m_nodes.emplace_back(a_name,a_text,a_self_close);
  return m_nodes.back();
}/*}}}*/

xml_node_c &xml_node_c::move_node(xml_node_c &&a_node) throw()
{/*{{{*/
  m_nodes.push_back(std::move(a_node));
  return m_nodes.back();
}/*}}}*/

std::ostream &operator << (std::ostream &a_out,const xml_node_c &a_this)
{/*{{{*/
  a_out << '<' << a_this.m_name;

  // - output node attributes -
  for (auto attr_i = a_this.m_attrs.begin();
            attr_i != a_this.m_attrs.end();
          ++attr_i)
  {
    a_out << ' ' << *attr_i;
  }

  if (a_this.m_self_close && a_this.m_text.empty() && a_this.m_nodes.empty())
  {
    a_out << "/>" << std::endl;
  }
  else
  {
    a_out << '>';

    // - output node text -
    if (!a_this.m_text.empty())
    {
      a_out << a_this.m_text;
    }

    // - output node subnodes -
    if (!a_this.m_nodes.empty())
    {
      a_out << std::endl;

      for (auto node_i = a_this.m_nodes.begin();
                node_i != a_this.m_nodes.end();
              ++node_i)
      {
        a_out << *node_i;
      }
    }

    a_out << "</" << a_this.m_name << '>' << std::endl;
  }

  return a_out;
}/*}}}*/

void xml_parser_c::xml_start_document(void *user)
{/*{{{*/
  (void)user;
}/*}}}*/

void xml_parser_c::xml_end_document(void *user)
{/*{{{*/
  (void)user;
}/*}}}*/

void xml_parser_c::xml_start_element(void *user,const xmlChar *name,const xmlChar **attrs)
{/*{{{*/
  auto xml_parser = static_cast<xml_parser_c *>(user);
  node_stack_t &ns = xml_parser->node_stack;

  xml_node_c *node;

  if (ns.empty())
  {
    ns.push_back(node = new xml_node_c(reinterpret_cast<const char *>(name)));
  }
  else
  {
    node = &(ns.back()->node(reinterpret_cast<const char *>(name)));
    ns.push_back(node);
  }

  if (attrs != nullptr)
  {
    auto attr_ptr = reinterpret_cast<const char **>(attrs);
    while (*attr_ptr != nullptr)
    {
      node->attr_(attr_ptr[0],attr_ptr[1]);

      attr_ptr += 2;
    }
  }
}/*}}}*/

void xml_parser_c::xml_end_element(void *user,const xmlChar *name)
{/*{{{*/
  (void)name;

  auto xml_parser = static_cast<xml_parser_c *>(user);
  node_stack_t &ns = xml_parser->node_stack;

  // - if not root element -
  if (ns.size() > 1)
  {
    ns.pop_back();
  }
}/*}}}*/

void xml_parser_c::xml_characters(void *user,const xmlChar *ch,int len)
{/*{{{*/
  auto xml_parser = static_cast<xml_parser_c *>(user);
  node_stack_t &ns = xml_parser->node_stack;

  // - skip end of lines -
  if ((len == 1 && (*ch == '\n' || *ch == '\r')) ||
      (len == 2 && ch[0] == '\r' && ch[1] == '\n'))
  {
    return;
  }

  ns.back()->text_(std::string(reinterpret_cast<const char *>(ch),static_cast<size_t>(len)));
}/*}}}*/

void xml_parser_c::xml_warning(void *user,const char *msg,...) // NOLINT
{/*{{{*/
  (void)user;
  (void)msg;
}/*}}}*/

void xml_parser_c::xml_error(void *user,const char *msg,...) // NOLINT
{/*{{{*/
  (void)user;
  (void)msg;
}/*}}}*/

void xml_parser_c::xml_fatal_error(void *user,const char *msg,...) // NOLINT
{/*{{{*/
  (void)user;
  (void)msg;
}/*}}}*/

xml_parser_c::~xml_parser_c()
{/*{{{*/
  if (!node_stack.empty())
  {
    delete node_stack[0];
  }
}/*}}}*/

xml_node_c xml_parser_c::parse(const std::string &a_data)
{/*{{{*/

  // - create sax handler -
  xmlSAXHandler sax = {};

  // - set sax callbacks -
  sax.startDocument = xml_parser_c::xml_start_document;
  sax.endDocument = xml_parser_c::xml_end_document;
  sax.startElement = xml_parser_c::xml_start_element;
  sax.endElement = xml_parser_c::xml_end_element;
  sax.characters = xml_parser_c::xml_characters;

  sax.warning = xml_parser_c::xml_warning;
  sax.error = xml_parser_c::xml_error;
  sax.fatalError = xml_parser_c::xml_fatal_error;

  xml_parser_c xml_parser;

  // - parse xml data -
  int res = xmlSAXUserParseMemory(&sax,&xml_parser,a_data.data(),a_data.size());

  // - ERROR -
  if (res != 0)
  {
    cclthrow(error_XML_DATA_PARSE_ERROR);
  }

  return std::move(*xml_parser.node_stack.back());
}/*}}}*/

cclxml_c::cclxml_c() throw()
{/*{{{*/
  xmlInitParser();
}/*}}}*/

cclxml_c::~cclxml_c()
{/*{{{*/
  xmlCleanupParser();
}/*}}}*/

// - curl global init object -
cclxml_c g_cclcurl;

} // namespace cclxml

