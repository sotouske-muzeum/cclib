
#include "cclcrypto.h"

namespace cclcrypto {

std::string digest(const std::string a_data,const EVP_MD *a_md)
{/*{{{*/
  EVP_MD_CTX *md_ctx;

  if ((md_ctx = EVP_MD_CTX_create()) == nullptr)
  {
    cclthrow(error_CRYPTO_MESSAGE_DIGEST_ERROR);
  }

  if (1 != EVP_DigestInit_ex(md_ctx,a_md,nullptr))
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
  if ((dg_data = reinterpret_cast<unsigned char *>(OPENSSL_malloc(EVP_MD_size(a_md)))) == nullptr)
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

std::string random(size_t a_size)
{/*{{{*/
  unsigned char buffer[a_size];
  RAND_bytes(buffer,a_size);

  return std::string(reinterpret_cast<char *>(buffer),a_size);
}/*}}}*/

std::string passwd_create(const std::string a_passwd,const EVP_MD *a_md)
{/*{{{*/
  std::string salt = random(EVP_MD_size(a_md));
  return salt + digest(salt + a_passwd,a_md);
}/*}}}*/

std::string passwd_create_base16(const std::string a_passwd,const EVP_MD *a_md)
{/*{{{*/
  return base16_encode(passwd_create(a_passwd,a_md));
}/*}}}*/

bool passwd_verify(const std::string a_passwd,const std::string a_hash,const EVP_MD *a_md)
{/*{{{*/
  std::string salt = a_hash.substr(0,EVP_MD_size(a_md));
  return a_hash.compare(EVP_MD_size(a_md),EVP_MD_size(a_md),
      digest(salt + a_passwd,a_md)) == 0;
}/*}}}*/

bool passwd_verify_base16(const std::string a_passwd,const std::string a_hash,const EVP_MD *a_md)
{/*{{{*/
  return passwd_verify(a_passwd,base16_decode(a_hash),a_md);
}/*}}}*/

std::string base16_encode(const std::string a_data)
{/*{{{*/
  std::string result;
  result.reserve(a_data.length() << 1);

  for (auto char_i = a_data.begin();
            char_i != a_data.end();
          ++char_i)
  {
    unsigned char ch = static_cast<unsigned char>(*char_i);
    result += c_base16_map[ch >> 4];
    result += c_base16_map[ch & 0x0f];
  }

  return result;
}/*}}}*/

std::string base16_decode(const std::string a_data)
{/*{{{*/

  // - ERROR -
  if ((a_data.length() & 0x01) != 0)
  {
    cclthrow(error_CRYPTO_INVALID_BASE16_DATA_SIZE);
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
      cclthrow(error_CRYPTO_INVALID_BASE16_DATA);
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
      cclthrow(error_CRYPTO_INVALID_BASE16_DATA);
    }

    result += ch;
  }

  return result;
}/*}}}*/

std::string base64_encode(const std::string a_data)
{/*{{{*/
  std::string result;
  result.resize(((a_data.length()/3 + 1) << 2) + 1);

  unsigned char *trg_buffer = 
    reinterpret_cast<unsigned char *>(
        const_cast<char *>(result.data()));

  const unsigned char *src_buffer = 
    reinterpret_cast<const unsigned char *>(a_data.data());

  int length = EVP_EncodeBlock(trg_buffer,src_buffer,a_data.length());
  result.resize(length);

  return result;
}/*}}}*/

std::string base64_decode(const std::string a_data)
{/*{{{*/
  std::string result;
  result.resize(((a_data.length() >> 2) * 3) + 1);

  unsigned char *trg_buffer = 
    reinterpret_cast<unsigned char *>(
        const_cast<char *>(result.data()));

  const unsigned char *src_buffer = 
    reinterpret_cast<const unsigned char *>(a_data.data());

  int length = EVP_DecodeBlock(trg_buffer,src_buffer,a_data.length());

  // - ERROR -
  if (length == -1)
  {
    cclthrow(error_CRYPTO_INVALID_BASE64_DATA);
  }

  result.resize(length);

  return result;
}/*}}}*/

cclcrypto_c::cclcrypto_c() throw()
{/*{{{*/
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
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

