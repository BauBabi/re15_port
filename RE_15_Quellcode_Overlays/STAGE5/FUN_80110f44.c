/* FUN_80110f44 @ 0x80110f44  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110f44(void)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  int unaff_s0;
  undefined4 uVar4;
  
  func_0x8001ad68(unaff_s0 + -0x95,_DAT_800acbcc,_DAT_800acbd0);
  uVar4 = 0x80110f78;
  iVar2 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (iVar2 != 0) {
    DAT_800aca5a = 6;
    uVar3 = *(uint *)(unaff_s0 + -0x95) & 0xffffefff;
    *(uint *)(unaff_s0 + -0x95) = uVar3;
    if (*(char *)(*(int *)(uVar3 - 0x387c) + 6) == '\0') {
      sVar1 = func_0x8001af20();
      *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + 0x3b;
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    }
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200,uVar4);
    if (((int)*(short *)(_DAT_800ac784 + 0x9c) % 0xc) * 0x10000 >> 0x10 == 0xb) {
      func_0x800453d0(6);
    }
    return;
  }
  return;
}


