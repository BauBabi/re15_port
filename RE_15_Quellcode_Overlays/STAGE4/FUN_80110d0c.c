/* FUN_80110d0c @ 0x80110d0c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110d0c(void)

{
  short sVar1;
  int iVar2;
  undefined4 uVar3;
  
  func_0x8001ad68(&DAT_800aca54);
  uVar3 = 0x80110d38;
  iVar2 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (*(char *)(*(int *)(iVar2 + -0x387c) + 6) == '\0') {
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
                  0x200,uVar3);
  if (((int)*(short *)(_DAT_800ac784 + 0x9c) % 0xc) * 0x10000 >> 0x10 == 0xb) {
    func_0x800453d0(6);
  }
  return;
}


