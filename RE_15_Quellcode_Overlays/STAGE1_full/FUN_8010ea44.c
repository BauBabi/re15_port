/* FUN_8010ea44 @ 0x8010ea44  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ea44(void)

{
  char cVar1;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0x3c;
    break;
  case 1:
  case 3:
  case 5:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x800453d0(2);
    _DAT_800aca50 = _DAT_800aca50 | 1;
    break;
  case 4:
    _DAT_800aca50 = _DAT_800aca50 & 0xfffe;
    if (*(short *)(_DAT_800ac784 + 0x9a) < 0) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 3;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x2f0,&DAT_80120f54);
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    break;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 5) = 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  default:
    goto switchD_8010ea7c_default;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_8010ea7c_default:
  return;
}


