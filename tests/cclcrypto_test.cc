
#include <cassert>

#include "cclcrypto.h"

void test_cclcrypto_random()
{/*{{{*/
  int idx = 0;
  do {
    std::cout << "random: " << cclcrypto::bin_to_hex(cclcrypto::random(32)) << std::endl;
  } while(++idx < 5);
}/*}}}*/

void test_cclcrypto_digest(const EVP_MD *a_md)
{/*{{{*/
  std::string encrypted = cclcrypto::digest("heslo",a_md);
  std::string encrypted_hex = cclcrypto::bin_to_hex(encrypted);

  // - test output -
  std::cout << "encrypted.length(): " << encrypted.length() << std::endl;
  std::cout << "encrypted: " << encrypted_hex << std::endl;

  std::string encrypted_bin = cclcrypto::hex_to_bin(cclcrypto::bin_to_hex(encrypted));
  assert(encrypted == encrypted_bin);
  assert(encrypted_hex == cclcrypto::bin_to_hex(encrypted_bin));
}/*}}}*/

void test_cclcrypto_passwd(const EVP_MD *a_md)
{/*{{{*/
  std::string passwd = "heslo";

  int idx = 0;
  do {
    std::string passwd_hash = cclcrypto::passwd_create(passwd,a_md);
    std::cout << "passwd_hash: " << cclcrypto::bin_to_hex(passwd_hash) << std::endl;
    assert(cclcrypto::passwd_verify(passwd,passwd_hash,a_md));
  } while(++idx < 5);
}/*}}}*/

void test_cclcrypto_passwd_hex(const EVP_MD *a_md)
{/*{{{*/
  std::string passwd = "heslo";

  int idx = 0;
  do {
    std::string passwd_hash = cclcrypto::passwd_create_hex(passwd,a_md);
    std::cout << "passwd_hash: " << passwd_hash << std::endl;
    assert(cclcrypto::passwd_verify_hex(passwd,passwd_hash,a_md));
  } while(++idx < 5);
}/*}}}*/

void test_cclcrypto_all()
{/*{{{*/
  test_cclcrypto_random();
  test_cclcrypto_digest(EVP_sha256());
  test_cclcrypto_digest(EVP_sha512());
  test_cclcrypto_passwd(EVP_sha256());
  test_cclcrypto_passwd(EVP_sha512());
  test_cclcrypto_passwd_hex(EVP_sha256());
  test_cclcrypto_passwd_hex(EVP_sha512());
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
      else if (std::string("passwd_hex_sha256") == argv[arg_idx])
      {
        test_cclcrypto_passwd_hex(EVP_sha256());
      }
      else if (std::string("passwd_hex_sha512") == argv[arg_idx])
      {
        test_cclcrypto_passwd_hex(EVP_sha512());
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

