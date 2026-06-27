/* FUN_8010edbc @ 0x8010edbc  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010edbc(void)

{
  short sVar1;
  int iVar2;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar1 = func_0x8001aa68((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) << 4,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = sVar1 + *(short *)(_DAT_800ac784 + 0x6a);
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) << 4;
  case 3:
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x12') {
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0xc1c;
      *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + -0x708;
    }
    iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar2 != 0) {
      *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x01';
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x708;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xc1c;
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
      func_0x800245d8(0);
      *(undefined2 *)(_DAT_800ac784 + 0x40) = *(undefined2 *)(_DAT_800ac784 + 0x34);
      *(undefined2 *)(_DAT_800ac784 + 0x44) = *(undefined2 *)(_DAT_800ac784 + 0x3c);
      *(undefined2 *)(_DAT_800ac784 + 0x42) = *(undefined2 *)(_DAT_800ac784 + 0x38);
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


