//
// Created by xwj on 12/23/25.
//

#include <stdio.h>
extern volatile int g_keyFlag;
void test_key(void)
{
  printf("test key\n");

  while (1)
  {
    if (g_keyFlag == 1) {
      printf("key is ok\n");
    }
  }
}
