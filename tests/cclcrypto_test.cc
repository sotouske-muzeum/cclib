
#include <cassert>

#include "cclcrypto.h"

void test_cclcrypto_random()
{/*{{{*/
  int idx = 0;
  do {
    std::cout << "random: " << cclcrypto::base16_encode(cclcrypto::random(32)) << std::endl;
  } while(++idx < 5);
}/*}}}*/

void test_cclcrypto_digest(const EVP_MD *a_md)
{/*{{{*/
  std::string encrypted = cclcrypto::digest("heslo",a_md);
  std::string encrypted_base16 = cclcrypto::base16_encode(encrypted);

  // - test output -
  std::cout << "encrypted.length(): " << encrypted.length() << std::endl;
  std::cout << "encrypted: " << encrypted_base16 << std::endl;

  std::string encrypted_bin = cclcrypto::base16_decode(cclcrypto::base16_encode(encrypted));
  assert(encrypted == encrypted_bin);
  assert(encrypted_base16 == cclcrypto::base16_encode(encrypted_bin));
}/*}}}*/

void test_cclcrypto_passwd(const EVP_MD *a_md)
{/*{{{*/
  std::string passwd = "heslo";

  int idx = 0;
  do {
    std::string passwd_hash = cclcrypto::passwd_create(passwd,a_md);
    std::cout << "passwd_hash: " << cclcrypto::base16_encode(passwd_hash) << std::endl;
    assert(cclcrypto::passwd_verify(passwd,passwd_hash,a_md));
  } while(++idx < 5);
}/*}}}*/

void test_cclcrypto_passwd_base16(const EVP_MD *a_md)
{/*{{{*/
  std::string passwd = "heslo";

  int idx = 0;
  do {
    std::string passwd_hash = cclcrypto::passwd_create_base16(passwd,a_md);
    std::cout << "passwd_hash: " << passwd_hash << std::endl;
    assert(cclcrypto::passwd_verify_base16(passwd,passwd_hash,a_md));
  } while(++idx < 5);
}/*}}}*/

void test_cclcrypto_base64()
{/*{{{*/
  std::string data = "Hello world!";
  std::string base64 = cclcrypto::base64_encode(data);

  std::cout << data << std::endl;
  std::cout << base64 << std::endl;

  std::string decoded = cclcrypto::base64_decode(base64);
  assert(data == decoded);

  std::cout << decoded << std::endl;

  int length = 0;
  do {
    std::string data = cclcrypto::random(length);
    std::string decoded = cclcrypto::base64_decode(cclcrypto::base64_encode(data));
    assert(data == decoded);
  } while((length += 3) < 64);
}/*}}}*/

void test_cclcrypto_all()
{/*{{{*/
  test_cclcrypto_random();
  test_cclcrypto_digest(EVP_sha256());
  test_cclcrypto_digest(EVP_sha512());
  test_cclcrypto_passwd(EVP_sha256());
  test_cclcrypto_passwd(EVP_sha512());
  test_cclcrypto_passwd_base16(EVP_sha256());
  test_cclcrypto_passwd_base16(EVP_sha512());
  test_cclcrypto_base64();
}/*}}}*/

int main(int argc,char **argv)
{/*{{{*/
  if (argc > 1)
  {
    int arg_idx = 1;
    do {
      if (std::string("random") == argv[arg_idx])
      {
        test_cclcrypto_random();
      }
      else if (std::string("digest_sha256") == argv[arg_idx])
      {
        test_cclcrypto_digest(EVP_sha256());
      }
      else if (std::string("digest_sha512") == argv[arg_idx])
      {
        test_cclcrypto_digest(EVP_sha512());
      }
      else if (std::string("passwd_sha256") == argv[arg_idx])
      {
        test_cclcrypto_passwd(EVP_sha256());
      }
      else if (std::string("passwd_sha512") == argv[arg_idx])
      {
        test_cclcrypto_passwd(EVP_sha512());
      }
      else if (std::string("passwd_base16_sha256") == argv[arg_idx])
      {
        test_cclcrypto_passwd_base16(EVP_sha256());
      }
      else if (std::string("passwd_base16_sha512") == argv[arg_idx])
      {
        test_cclcrypto_passwd_base16(EVP_sha512());
      }
      else if (std::string("base64") == argv[arg_idx])
      {
        test_cclcrypto_base64();
      }
      else if (std::string("all") == argv[arg_idx])
      {
        test_cclcrypto_all();
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

