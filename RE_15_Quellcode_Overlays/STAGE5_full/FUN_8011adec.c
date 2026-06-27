/* FUN_8011adec @ 0x8011adec  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011adec(void)

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
    goto LAB_8011af50;
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
LAB_8011af50:
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
  FUN_8011bdd8(0,0);
  return;
}


