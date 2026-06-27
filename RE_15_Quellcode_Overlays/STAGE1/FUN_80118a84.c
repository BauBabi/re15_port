/* FUN_80118a84 @ 0x80118a84  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118a84(void)

{
  byte bVar1;
  undefined2 uVar2;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  if (1 < *(byte *)(_DAT_800ac784 + 6)) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x93);
    if ((bVar1 & 2) != 0) {
      if ((bVar1 & 0x40) != 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 2;
        *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        uVar2 = func_0x8001a9cc();
        *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar2;
        *(short *)(_DAT_800ac784 + 0x6a) =
             *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1da);
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        if (*(short *)(_DAT_800ac784 + 0x1da) == 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          *(undefined1 *)(_DAT_800ac784 + 7) = 0;
          if (((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) ||
             (5000 < *(ushort *)(_DAT_800ac784 + 0x1d4))) {
            *(undefined1 *)(_DAT_800ac784 + 5) = 10;
          }
        }
        return;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = bVar1 & 0xfd;
      local_28 = _DAT_801213a8;
      local_24 = _DAT_801213ac;
      local_20 = _DAT_801213b0;
      local_1c = _DAT_801213b4;
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
    }
  }
  return;
}


