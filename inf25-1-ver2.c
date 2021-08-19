
//  Программе на стандартный поток ввода передается последовательность байт.
//
//  Необходимо вычислить контрольную сумму SHA-512 и вывести это значение в
//  hex-виде c префиксом 0x.
//
//  Используйте API OpenSSL/LibreSSL. Запуск сторонних команд через fork+exec
//  запрещен.
//
//  Отправляйте только исходный файл Си-программы с решением.

#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <openssl/evp.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef enum
{
  BUFFER_SIZE = 64,
  KEY_SIZE = 32,
  IV_SIZE = 16,
  SALT_SIZE = 8
} constants;

int
main (int argc, char **argv)
{
  char *password = argv[1];

  // The first 8-byte of encrypted data is 'Salted__',
  // which meas the data was encrypted using salt.
  // The next 8-byte is the salt, which is exactly the same as openssl -p
  // output.
  unsigned char salt_with_name[16];
  read (0, salt_with_name, sizeof (salt_with_name));
  unsigned char salt[SALT_SIZE];
  // clean salt
  memcpy (salt, salt_with_name + 8, sizeof (salt));

  // подготовка ключа и вектора для инициализации
  unsigned char key[KEY_SIZE];
  for (int i = 0; i < KEY_SIZE; ++i)
    {
      key[i] = '\0';
    }
  unsigned char iv[IV_SIZE];
  for (int i = 0; i < IV_SIZE; ++i)
    {
      iv[i] = '\0';
    }

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new ();
  EVP_BytesToKey (EVP_aes_256_cbc (), EVP_sha256 (), salt, password,
                  strlen (password), 1, key, iv);
  EVP_DecryptInit_ex (ctx, EVP_aes_256_cbc (), NULL, key, iv);
  // порционная расшифровка
  int block_size = EVP_CIPHER_CTX_block_size (ctx);
  unsigned char encoded[block_size];
  unsigned char decoded[block_size];

  uint64_t encoded_block_size;
  int decoded_block_size;
  while ((encoded_block_size = read (0, encoded, sizeof (encoded))) > 0)
    {
      EVP_DecryptUpdate (ctx, decoded, &decoded_block_size, encoded,
                         encoded_block_size);
      printf ("%.*s", decoded_block_size, decoded);
    }
  EVP_DecryptFinal_ex (ctx, decoded, &decoded_block_size);
  printf ("%.*s", decoded_block_size, decoded);

  EVP_CIPHER_CTX_free (ctx);
}
