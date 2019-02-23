#include "poly_test.h"
#include <stdio.h>


int main()
{
  printf("test\n");
  poly_test_init();
  test_struct_t* testobj = new_test_struct();

  char* test = "This is a test";

  memcpy(testobj->testString, test, strlen(test));

  testobj->testInt = 4;

  return 0;
}
