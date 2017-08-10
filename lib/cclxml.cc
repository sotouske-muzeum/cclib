
#include "cclxml.h"

namespace cclxml {

std::ostream &operator << (std::ostream &a_out,const xml_attr_c &a_this)
{/*{{{*/
  a_out << a_this.m_name << "=\"" << a_this.m_value << '"';
  return a_out;
}/*}}}*/

void xml_node_c::attr(const std::string a_name,const std::string a_value)
{/*{{{*/
  m_attrs.emplace_back(a_name,a_value);
}/*}}}*/

xml_node_c &xml_node_c::node(
  const std::string a_name,const std::string a_text,bool a_self_close)
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

} // namespace cclxml

