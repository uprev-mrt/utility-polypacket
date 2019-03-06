#include "poly_test.h"
#include <stdio.h>

uint8_t buffer[1024];
char printBuf[1024];



int main()
{
  printf("test\n");
  poly_test_init();

  PolyTest obj;

  obj.TestString("this is my test");
  obj.TestInt(455);

  printf("%s", obj.toJSON());

  return 0;
}
