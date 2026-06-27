/* FUN_8010f67c @ 0x8010f67c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f67c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      cVar2 = *(char *)(_DAT_800ac784 + 0x9e);
      *(char *)(_DAT_800ac784 + 0x9e) = cVar2 + -1;
      if (cVar2 != '\0') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x96;
    *(undefined2 *)(_DAT_800ac784 + 0x1c0) = 0x8000;
    cVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = cVar2 + 0x2dU & 0x3f;
    func_0x800453d0(1);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8011ea6c);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -2;
  func_0x800245d8((1 - (*(byte *)(_DAT_800ac784 + 0x93) & 0x80)) * 0x800);
  if (*(char *)(_DAT_800ac784 + 7) == '\x02') {
    func_0x800453d0(7);
  }
  return;
}


