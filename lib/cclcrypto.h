
#ifndef CCLIB_CCLCRYPTO_H
#define CCLIB_CCLCRYPTO_H

#include "cclsys.h"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

namespace cclcrypto {

// - exceptions list -
const std::string error_CRYPTO_MESSAGE_DIGEST_ERROR = "error while computing message digest";
const std::string error_CRYPTO_INVALID_BASE16_DATA_SIZE = "invalid base16 data size";
const std::string error_CRYPTO_INVALID_BASE16_DATA = "invalid base16 data";
const std::string error_CRYPTO_INVALID_BASE64_DATA = "invalid base64 data to decode";

const char c_base16_map[] = "0123456789abcdef";

std::string digest(const std::string a_data,const EVP_MD *a_md);
std::string random(size_t a_size);
std::string passwd_create(const std::string a_passwd,const EVP_MD *a_md);
std::string passwd_create_base16(const std::string a_passwd,const EVP_MD *a_md);
bool passwd_verify(const std::string a_passwd,const std::string a_hash,const EVP_MD *a_md);
bool passwd_verify_base16(const std::string a_passwd,const std::string a_hash,const EVP_MD *a_md);
std::string base16_encode(const std::string a_data);
std::string base16_decode(const std::string a_data);
std::string base64_encode(const std::string a_data);
std::string base64_decode(const std::string a_data);

class cclcrypto_c
{
  public:
  cclcrypto_c() throw();
  ~cclcrypto_c();
};

} // namespace cclcrypto

#endif

