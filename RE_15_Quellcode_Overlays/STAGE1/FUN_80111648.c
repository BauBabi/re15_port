/* FUN_80111648 @ 0x80111648  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111648(undefined4 param_1,int param_2,undefined4 param_3)

{
  char cVar1;
  
  cVar1 = func_0x8001f314(param_1,*(undefined4 *)(param_2 + -0x3430),param_3,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  if (((_DAT_800aca52 & 1) != 0) && (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1518)) {
    FUN_80115d74(4);
  }
  if ((_DAT_800aca50 & 0x1000) != 0) {
    FUN_80115d74((uint)*(byte *)(_DAT_800ac784 + 0x1d4) % 3 + 1);
  }
  return;
}


