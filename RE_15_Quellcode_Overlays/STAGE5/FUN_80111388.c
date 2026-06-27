/* FUN_80111388 @ 0x80111388  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111388(int param_1)

{
  int in_v1;
  
  if (*(int *)(param_1 + 0x38) < in_v1) {
    *(undefined2 *)(param_1 + 0xb4) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0xb6) = 0;
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}


