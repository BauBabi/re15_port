/* FUN_801169b8 @ 0x801169b8  (Ghidra headless, raw MIPS overlay) */

void FUN_801169b8(int param_1)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(param_1 + 5);
  if (bVar1 == 1) {
    *(undefined1 *)(param_1 + 0x94) = 0;
    *(undefined1 *)(param_1 + 0x95) = 0;
    *(undefined1 *)(param_1 + 0x8f) = 0;
    *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        return;
      }
      iVar2 = func_0x8004efe4(0x800b1028,0x1f);
      if (iVar2 == 0) {
        return;
      }
      *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
      func_0x8004efb8(0x800b1028,0x1f);
      return;
    }
    if (bVar1 != 2) {
      return;
    }
  }
  iVar2 = *(int *)(param_1 + 0x188);
  if (*(byte *)(param_1 + 0x95) < 10) {
    *(int *)(iVar2 + 0xdc) = *(int *)(iVar2 + 0xdc) + 0xb;
  }
  if (*(byte *)(param_1 + 0x95) - 10 < 0xd) {
    *(int *)(iVar2 + 0xdc) = *(int *)(iVar2 + 0xdc) + 0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x17 < 7) {
    *(int *)(iVar2 + 0xdc) = *(int *)(iVar2 + 0xdc) + 0xb;
  }
  if (*(char *)(param_1 + 0x95) == '(') {
    *(int *)(iVar2 + 0x188) = *(int *)(iVar2 + 0x188) + -0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x29 < 7) {
    *(int *)(iVar2 + 0x188) = *(int *)(iVar2 + 0x188) + -0xd;
  }
  if (*(byte *)(param_1 + 0x95) - 0x2f < 7) {
    *(int *)(iVar2 + 0x2e0) = *(int *)(iVar2 + 0x2e0) + 0xc;
    *(int *)(iVar2 + 0x438) = *(int *)(iVar2 + 0x438) + 0xc;
    *(int *)(iVar2 + 0x590) = *(int *)(iVar2 + 0x590) + 0xc;
    *(int *)(iVar2 + 0x6e8) = *(int *)(iVar2 + 0x6e8) + 0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x36 < 6) {
    *(int *)(iVar2 + 0x2e0) = *(int *)(iVar2 + 0x2e0) + 0xb;
    *(int *)(iVar2 + 0x438) = *(int *)(iVar2 + 0x438) + 0xb;
    *(int *)(iVar2 + 0x590) = *(int *)(iVar2 + 0x590) + 0xb;
    *(int *)(iVar2 + 0x6e8) = *(int *)(iVar2 + 0x6e8) + 0xb;
  }
  iVar2 = func_0x8001f314(*(undefined4 *)(param_1 + 0x84),*(undefined4 *)(param_1 + 0x16c),0,0x200);
  if (iVar2 != 0) {
    *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
  }
  return;
}


