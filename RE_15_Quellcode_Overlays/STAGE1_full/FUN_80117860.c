/* FUN_80117860 @ 0x80117860  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117860(void)

{
  byte bVar1;
  char cVar2;
  undefined1 uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      bVar1 = *(byte *)(_DAT_800ac784 + 7);
      if (bVar1 == 1) {
        uVar3 = 4;
      }
      else {
        if (1 < bVar1) {
          if (bVar1 != 2) {
            return;
          }
          *(undefined1 *)(_DAT_800ac784 + 5) = 3;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          *(undefined1 *)(_DAT_800ac784 + 7) = 1;
          return;
        }
        uVar3 = 3;
        if (bVar1 != 0) {
          return;
        }
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    bVar1 = *(byte *)(_DAT_800ac784 + 7);
    if (bVar1 == 1) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    }
    else if (bVar1 < 2) {
      if (bVar1 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
      }
    }
    else if (bVar1 == 2) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    }
  }
  if ((*(char *)(_DAT_800ac784 + 7) == '\0') && (*(char *)(_DAT_800ac784 + 0x95) == '\x1d')) {
    func_0x800453d0(4);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_8011bf50(0,0);
  return;
}


