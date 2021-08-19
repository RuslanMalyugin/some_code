#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

volatile int BLOCK = 32;
volatile int KEY_SIZE = 32;
volatile int SALT_SIZE = 16;

void
decrypt_input (char *password)
{
  char salt[SALT_SIZE];
  char result[BLOCK];
  char buffer[BLOCK];
  unsigned char ckey[KEY_SIZE];
  unsigned char ivec[BLOCK];
  int encoded_msg_len = 0;

  read (0, salt, SALT_SIZE);

  EVP_BytesToKey (EVP_aes_256_cbc (), EVP_sha256 (), salt + 8, password,
                  strlen (password), 1, ckey, ivec);
  EVP_CIPHER_CTX *ctx;
  ctx = EVP_CIPHER_CTX_new ();
  EVP_DecryptInit_ex (ctx, EVP_aes_256_cbc (), NULL, ckey, ivec);

  while (read (0, buffer, BLOCK))
    {
      EVP_DecryptUpdate (ctx, result, &encoded_msg_len, buffer, BLOCK);
      write (1, result, encoded_msg_len);
      
    }

  EVP_DecryptFinal_ex (ctx, result, &encoded_msg_len);
  write (1, result, encoded_msg_len);
  EVP_CIPHER_CTX_free (ctx);
}

int
main (int argc, char *argv[])
{
  char *password = argv[1];
  decrypt_input (password);
  return 0;
}
