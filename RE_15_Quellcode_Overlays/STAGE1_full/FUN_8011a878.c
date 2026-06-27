/* FUN_8011a878 @ 0x8011a878  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a878(void)

{
  byte bVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar1 & 2) != 0) {
    if ((bVar1 & 0x40) == 0) {
      *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
      local_28 = DAT_801213a8;
      local_24 = DAT_801213ac;
      local_20 = DAT_801213b0;
      local_1c = DAT_801213b4;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
    else {
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}


