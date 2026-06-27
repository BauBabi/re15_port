/* FUN_8011971c @ 0x8011971c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011971c(void)

{
  int iVar1;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
LAB_80119870:
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    }
    break;
  case 2:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x18') {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x17;
    }
    iVar1 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffb0,
                            (int)*(short *)(_DAT_800ac784 + 0x1ba));
    if (iVar1 == 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    goto LAB_80119870;
  case 3:
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x18;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    func_0x800453d0(2);
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80121368)[*(byte *)(_DAT_800ac784 + 0x1e2)];
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  case 4:
    iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar1 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    break;
  default:
    break;
  }
  return;
}


