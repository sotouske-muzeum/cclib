
#include <cassert>
#include <unordered_map>

#include "cclstr.h"
#include "cclxml.h"

void struct_error(void *ctx,_xmlError *err)
{/*{{{*/
  (void)ctx;(void)err;
}/*}}}*/

void error(void *ctx,const char *msg,...) // NOLINT
{/*{{{*/
  (void)ctx;(void)msg;
}/*}}}*/

void print_element_names(xmlNode *a_node)
{/*{{{*/
  xmlNode *cur_node = a_node;
  for (; cur_node != nullptr; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      //printf("node type: Element, name: %s\n", cur_node->name);
    }

    print_element_names(cur_node->children);
  }
}/*}}}*/

void test_cclxml_parse()
{/*{{{*/
  xmlSetGenericErrorFunc(nullptr,error);
  xmlSetStructuredErrorFunc(nullptr,struct_error);

  xmlDocPtr doc = xmlParseFile("../tests/data/cclxml/track.gpx");
  xmlNode *root_element = xmlDocGetRootElement(doc);

  print_element_names(root_element);

  xmlFreeDoc(doc);
  xmlCleanupParser();
}/*}}}*/

void test_cclxml_generate()
{/*{{{*/
  cclxml::xml_node_c root{"root"};
  root.node("node","Alone node text");

  int idx = 0;
  do {
    auto &node = root.node("node","Some data as text");

    node.attr_("attr0","Attribute value 0");
    node.attr_("attr1","Attribute value 1");

  } while(++idx < 20);

  // - generate xml to string -
  std::stringbuf buffer;
  std::ostream os(&buffer);
  os << root;

  // - read reference xml from file -
  std::string ref_xml;
  assert(cclstr::read_file("../tests/data/cclxml/generated.xml",ref_xml));

  // - compare generated and reference xml -
  assert(buffer.str() == ref_xml);
}/*}}}*/

void test_cclxml_parse_val()
{/*{{{*/
  std::string data = cclstr::read_file("../tests/data/cclxml/test.xml");
  cclxml::xml_node_c node = cclxml::xml_parser_c::parse(data);

  // - generate xml to string -
  std::stringbuf buffer;
  std::ostream os(&buffer);
  os << node;

  // - compare generated and original xml -
  assert(buffer.str() == data);
}/*}}}*/

void test_cclxml_all()
{/*{{{*/
  test_cclxml_parse();
  test_cclxml_generate();
  test_cclxml_parse_val();
}/*}}}*/

int main(int argc,char **argv)
{/*{{{*/
  if (argc > 1)
  {
    int arg_idx = 1;
    do {
      if (std::string("parse") == argv[arg_idx])
      {
        test_cclxml_parse();
      }
      else if (std::string("generate") == argv[arg_idx])
      {
        test_cclxml_generate();
      }
      else if (std::string("parse_val") == argv[arg_idx])
      {
        try { test_cclxml_parse_val(); } catch (...) {}
      }
      else if (std::string("all") == argv[arg_idx])
      {
        test_cclxml_all();
      }
      else
      {
        // - ERROR -
        throw std::runtime_error{"unrecognized test argument"};
      }
    } while(++arg_idx < argc);
  }

  return 0;
}/*}}}*/

