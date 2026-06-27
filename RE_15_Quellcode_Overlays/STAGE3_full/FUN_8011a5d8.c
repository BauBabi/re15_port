/* FUN_8011a5d8 @ 0x8011a5d8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a5d8(void)

{
  char cVar1;
  char cVar2;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    func_0x800453d0(1);
  case 1:
    cVar2 = *(char *)(_DAT_800ac784 + 0x95);
    cVar1 = 'P';
    goto LAB_8011a73c;
  case 2:
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 300;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    func_0x8004ef90(0x800b1028,0x1c);
  case 3:
    if ((*(byte *)(_DAT_800ac784 + 9) & 0xf) == 2) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 7) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  case 5:
    cVar2 = *(char *)(_DAT_800ac784 + 0x95);
    cVar1 = '<';
LAB_8011a73c:
    if (cVar2 == cVar1) {
      func_0x800453d0(10);
    }
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    break;
  case 6:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0xb01;
  }
  FUN_8011b5c4(0,0);
  return;
}


