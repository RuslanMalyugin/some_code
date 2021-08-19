#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

volatile  int MAX_SIZE = 4096;
volatile  int SALT_SIZE = 16;
volatile  int KEY_SIZE = 32;
volatile  int IV_SIZE = 32;
volatile  int BLOCK_SIZE = 32;

int
main (int argc, char *argv[])
{
  char *password = argv[1];

  char salt[SALT_SIZE];
  read (0, salt, SALT_SIZE);
  unsigned char key[KEY_SIZE];
  unsigned char iv[IV_SIZE];
  EVP_BytesToKey (EVP_aes_256_cbc (), EVP_sha256 (), salt + 8, password,
                  strlen (password), 1, key, iv);

  EVP_CIPHER_CTX *ctx;
  ctx = EVP_CIPHER_CTX_new ();
  EVP_DecryptInit_ex (ctx, EVP_aes_256_cbc (), NULL, key, iv);
  char encrypted_data[BLOCK_SIZE];
  char decrypted_data[BLOCK_SIZE];
  int decoded_data_len = 0;
  int bytes;
  while (read (0, encrypted_data, BLOCK_SIZE))
    {
      EVP_DecryptUpdate (ctx, decrypted_data, &decoded_data_len,
                         encrypted_data, BLOCK_SIZE);
      write (1, decrypted_data, decoded_data_len);
      // EVP_DecryptUpdate (ctx, result, &encoded_msg_len, buffer, BLOCK);
      // write (1, result, encoded_msg_len);
      write (1, "\n", 1);
    }
  write (1, "\n", 1);
  EVP_DecryptFinal_ex (ctx, decrypted_data, &decoded_data_len);
  write (1, decrypted_data, decoded_data_len);

  EVP_CIPHER_CTX_free (ctx);
}