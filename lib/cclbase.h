
#ifndef CCLIB_CCLBASE_H
#define CCLIB_CCLBASE_H

#include <stdexcept>

#if __cplusplus < 201103L
#define nullptr NULL
#endif

#define cclthrow(MSG) \
{/*{{{*/\
  fprintf(stderr,"EXCEPTION: %s +%d fun: %s\n",__FILE__,__LINE__,__FUNCTION__);\
  throw std::runtime_error{MSG};\
}/*}}}*/

namespace cclbase {
} // namespace cclbase

#endif

