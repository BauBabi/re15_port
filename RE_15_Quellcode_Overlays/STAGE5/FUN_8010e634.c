/* FUN_8010e634 @ 0x8010e634  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e634(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar1 & 2) != 0) {
    if ((bVar1 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      FUN_8010eef8();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
    local_28 = DAT_8011fa1c;
    local_24 = DAT_8011fa20;
    local_20 = DAT_8011fa24;
    local_1c = DAT_8011fa28;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}


