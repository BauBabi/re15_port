/* FUN_8010ed88 @ 0x8010ed88  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ed88(void)

{
  char cVar1;
  char cVar2;
  undefined4 uVar3;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    FUN_801112dc(0,0);
    if (*(char *)(_DAT_800ac784 + 0x95) == '2') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xd;
LAB_8010efbc:
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x32;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 3:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    cVar1 = *(char *)(_DAT_800ac784 + 6);
    uVar3 = 1;
LAB_8010ef98:
    *(char *)(_DAT_800ac784 + 6) = cVar1 + cVar2;
    FUN_801112dc(0,uVar3);
    break;
  case 4:
  case 7:
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    goto LAB_8010efbc;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 6) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x19;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 6:
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    cVar1 = *(char *)(_DAT_800ac784 + 6);
    uVar3 = 0;
    goto LAB_8010ef98;
  default:
    break;
  }
  return;
}


