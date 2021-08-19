
#include <stdint.h>
#include <stdio.h>
typedef enum
{
  PlusZero = 0x00,
  MinusZero = 0x01,
  PlusInf = 0xF0,
  MinusInf = 0xF1,
  PlusRegular = 0x10,
  MinusRegular = 0x11,
  PlusDenormal = 0x20,
  MinusDenormal = 0x21,
  SignalingNaN = 0x30,
  QuietNaN = 0x31
} float_class_t;

extern float_class_t
classify (double *value_ptr)
{

  void *a_ptr_as_void = value_ptr;
  uint64_t *a_ptr_as_uint = (uint64_t *)a_ptr_as_void;
  uint64_t b = *a_ptr_as_uint;
  uint64_t tmp_for_extra = b >> 52;
  char is_exponent_full = 1;
  uint64_t bool_true = 1;
  for (int i = 0; i < 11; i++)
    {
      if ((bool_true & (tmp_for_extra >> i)) == 0)
        {
          is_exponent_full = 0;
          break;
        }
    }
  char is_negative = 0;
  if ((b >> 63) == 1)
    {
      is_negative = 1;
    }
  char is_mantis_zero = 1;
  for (int i = 0; i < 52; i++)
    {
      if ((bool_true & (b >> i)) == 1)
        {
          is_mantis_zero = 0;
          break;
        }
    }
  if (is_exponent_full == 1)
    {
      if (is_mantis_zero == 1)
        {
          if (is_negative == 1)
            return MinusInf;
          else
            return PlusInf;
        }
      if ((1 & (b >> 51)) == 1)
        return QuietNaN;
      else
        return SignalingNaN;
    }
  char is_exponent_empty = 1;
  for (int i = 0; i < 11; i++)
    {
      if ((bool_true & (tmp_for_extra >> i)) == 1)
        {
          is_exponent_empty = 0;
          break;
        }
    }
  if ((is_exponent_empty == 1) && (is_mantis_zero == 1))
    {
      if (is_negative == 1)
        return MinusZero;
      else
        return PlusZero;
    }
  if ((is_exponent_empty == 1) && (is_mantis_zero == 0))
    {
      if (is_negative == 1)
        return MinusDenormal;
      else
        return PlusDenormal;
    }
  if (is_negative == 1)
    return MinusRegular;
  else
    return PlusRegular;
}

// int
// main ()
// {
//   double a = 0.0 / 0.0;
//   // scanf("%lf", &a);
//   printf ("%x", classify (&a));
//   return 0;
// }