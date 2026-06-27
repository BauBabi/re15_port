void * memset(uchar *param_1,uchar param_2,int param_3)

{
  void *pvVar1;
  
  pvVar1 = (void *)(param_3 + -1);
  if (param_3 != 0) {
    do {
      *param_1 = param_2;
      pvVar1 = (void *)((int)pvVar1 + -1);
      param_1 = param_1 + 1;
    } while (pvVar1 != (void *)0xffffffff);
  }
  return pvVar1;
}
