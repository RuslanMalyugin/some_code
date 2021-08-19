#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

volatile int MAX_SIZE = 4096;

int
main ()
{
  unsigned char buffer[SHA512_DIGEST_LENGTH];
  SHA512_CTX ctx;
  SHA512_Init (&ctx);
  size_t size;
  while ((size = read (0, buffer, SHA512_DIGEST_LENGTH)) > 0)
    {
      SHA512_Update (&ctx, buffer, size);
    }
  SHA512_Final (buffer, &ctx);
  printf ("0x");
  for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i)
    {
      printf ("%02x", buffer[i]);
    }
  return 0;
}
