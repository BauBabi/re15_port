/* FUN_80114850 @ 0x80114850  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114850(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
      if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 100;
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar3 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(&DAT_8011a168 + iVar3);
    local_1c = *(undefined4 *)(&DAT_8011a16c + iVar3);
    local_18 = *(undefined4 *)(&DAT_8011a170 + iVar3);
    local_14 = *(undefined4 *)(&DAT_8011a174 + iVar3);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(0);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_80115bec(0,1);
  return;
}


