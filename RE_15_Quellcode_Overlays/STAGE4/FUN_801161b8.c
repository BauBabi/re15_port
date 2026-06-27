/* FUN_801161b8 @ 0x801161b8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801161b8(int param_1)

{
  int iVar1;
  uint uVar2;
  uint extraout_v1;
  
  uVar2 = (uint)*(byte *)(param_1 + 5);
  if (uVar2 != 1) {
    if (uVar2 < 2) {
      if (uVar2 == 0) {
        iVar1 = func_0x8004efe4(0x800b1028,0x1f);
        if (iVar1 == 0) {
          return;
        }
        *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
        func_0x8004efb8(0x800b1028,0x1f);
        uVar2 = extraout_v1;
      }
    }
    else if (uVar2 == 2) goto LAB_80116258;
    *(undefined1 *)(*(int *)(uVar2 - 0x387c) + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0xb0,0x808080);
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 4;
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
    }
    return;
  }
  *(undefined1 *)(param_1 + 0x94) = 0;
  *(undefined1 *)(param_1 + 0x95) = 0;
  *(undefined1 *)(param_1 + 0x8f) = 0;
  *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
LAB_80116258:
  iVar1 = *(int *)(param_1 + 0x188);
  if (*(byte *)(param_1 + 0x95) < 10) {
    *(int *)(iVar1 + 0xdc) = *(int *)(iVar1 + 0xdc) + 0xb;
  }
  if (*(byte *)(param_1 + 0x95) - 10 < 0xd) {
    *(int *)(iVar1 + 0xdc) = *(int *)(iVar1 + 0xdc) + 0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x17 < 7) {
    *(int *)(iVar1 + 0xdc) = *(int *)(iVar1 + 0xdc) + 0xb;
  }
  if (*(char *)(param_1 + 0x95) == '(') {
    *(int *)(iVar1 + 0x188) = *(int *)(iVar1 + 0x188) + -0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x29 < 7) {
    *(int *)(iVar1 + 0x188) = *(int *)(iVar1 + 0x188) + -0xd;
  }
  if (*(byte *)(param_1 + 0x95) - 0x2f < 7) {
    *(int *)(iVar1 + 0x2e0) = *(int *)(iVar1 + 0x2e0) + 0xc;
    *(int *)(iVar1 + 0x438) = *(int *)(iVar1 + 0x438) + 0xc;
    *(int *)(iVar1 + 0x590) = *(int *)(iVar1 + 0x590) + 0xc;
    *(int *)(iVar1 + 0x6e8) = *(int *)(iVar1 + 0x6e8) + 0xc;
  }
  if (*(byte *)(param_1 + 0x95) - 0x36 < 6) {
    *(int *)(iVar1 + 0x2e0) = *(int *)(iVar1 + 0x2e0) + 0xb;
    *(int *)(iVar1 + 0x438) = *(int *)(iVar1 + 0x438) + 0xb;
    *(int *)(iVar1 + 0x590) = *(int *)(iVar1 + 0x590) + 0xb;
    *(int *)(iVar1 + 0x6e8) = *(int *)(iVar1 + 0x6e8) + 0xb;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(param_1 + 0x84),*(undefined4 *)(param_1 + 0x16c),0,0x200);
  if (iVar1 != 0) {
    *(char *)(param_1 + 5) = *(char *)(param_1 + 5) + '\x01';
  }
  return;
}


