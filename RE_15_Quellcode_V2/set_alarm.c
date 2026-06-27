void set_alarm(void)

{
  int iVar1;
  
  iVar1 = FUN_80061fc0(0xffffffff);
  DAT_8007e468 = iVar1 + 0xf0;
  DAT_8007e46c = 0;
  return;
}
