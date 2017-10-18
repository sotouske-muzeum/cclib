
#include "cclcrypto.h"

namespace cclcrypto {

std::string digest(const std::string a_data,const EVP_MD *a_md)
{/*{{{*/
  EVP_MD_CTX *md_ctx;

  if ((md_ctx = EVP_MD_CTX_create()) == NULL)
  {
    cclthrow(error_CRYPTO_MESSAGE_DIGEST_ERROR);
  }

  if (1 != EVP_DigestInit_ex(md_ctx,a_md,NULL))
  {
    EVP_MD_CTX_destroy(md_ctx);
    cclthrow(error_CRYPTO_MESSAGE_DIGEST_ERROR);
  }

  if (1 != EVP_DigestUpdate(md_ctx,a_data.data(),a_data.length()))
  {
    EVP_MD_CTX_destroy(md_ctx);
    cclthrow(error_CRYPTO_MESSAGE_DIGEST_ERROR);
  }

  unsigned char *dg_data;
  unsigned int dg_length;
  if ((dg_data = (unsigned char *)OPENSSL_malloc(EVP_MD_size(a_md))) == NULL)
  {
    EVP_MD_CTX_destroy(md_ctx);
    cclthrow(error_CRYPTO_MESSAGE_DIGEST_ERROR);
  }

  if (1 != EVP_DigestFinal_ex(md_ctx,dg_data,&dg_length))
  {
    OPENSSL_free(dg_data);
    EVP_MD_CTX_destroy(md_ctx);
    cclthrow(error_CRYPTO_MESSAGE_DIGEST_ERROR);
  }

  // - create digest string -
  std::string digest_str(reinterpret_cast<char *>(dg_data),dg_length);

  OPENSSL_free(dg_data);
  EVP_MD_CTX_destroy(md_ctx);

  return digest_str;
}/*}}}*/

std::string bin_to_hex(const std::string a_data)
{/*{{{*/
  std::string result;
  result.reserve(a_data.length() << 1);

  for (auto char_i = a_data.begin();
            char_i != a_data.end();
          ++char_i)
  {
    unsigned char ch = static_cast<unsigned char>(*char_i);
    result += c_hexa_map[ch >> 4];
    result += c_hexa_map[ch & 0x0f];
  }

  return result;
}/*}}}*/

std::string hex_to_bin(const std::string a_data)
{/*{{{*/

  // - ERROR -
  if (a_data.length() & 0x01)
  {
    cclthrow(error_CRYPTO_INVALID_HEXA_DATA_SIZE);
  }

  std::string result;
  result.reserve(a_data.length() >> 1);

  for (auto char_i = a_data.begin();
            char_i != a_data.end();
            char_i += 2)
  {
    unsigned char ch;

    char first = char_i[0];
    if (first >= '0' && first <= '9')
    {
      ch = first - '0';
    }
    else if (first >= 'a' && first <= 'f')
    {
      ch = 10 + (first - 'a');
    }
    else if (first >= 'A' && first <= 'F')
    {
      ch = 10 + (first - 'A');
    }

    // - ERROR -
    else
    {
      cclthrow(error_CRYPTO_INVALID_HEXA_DATA);
    }

    ch <<= 4;

    char second = char_i[1];
    if (second >= '0' && second <= '9')
    {
      ch += second - '0';
    }
    else if (second >= 'a' && second <= 'f')
    {
      ch += 10 + (second - 'a');
    }
    else if (second >= 'A' && second <= 'F')
    {
      ch += 10 + (second - 'A');
    }

    // - ERROR -
    else
    {
      cclthrow(error_CRYPTO_INVALID_HEXA_DATA);
    }

    result += ch;
  }

  return result;
}/*}}}*/

cclcrypto_c::cclcrypto_c() throw()
{/*{{{*/
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
  OPENSSL_config(NULL);
}/*}}}*/

cclcrypto_c::~cclcrypto_c()
{/*{{{*/
  EVP_cleanup();
  CRYPTO_cleanup_all_ex_data();
  ERR_free_strings();
}/*}}}*/

// - crypto global init object -
cclcrypto_c g_cclcrypto;

} // namespace cclcrypto

