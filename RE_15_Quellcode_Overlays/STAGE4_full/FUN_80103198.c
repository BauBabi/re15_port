/* FUN_80103198 @ 0x80103198  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80103198(void)

{
  char cVar1;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x17;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 1;
    break;
  case 1:
  case 5:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  case 3:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    return;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x19;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    func_0x800453d0(0);
    break;
  case 6:
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 3;
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) & 0x7f;
  default:
    goto switchD_801031d0_default;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_801031d0_default:
  return;
}


