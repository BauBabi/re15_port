/* FUN_80118908 @ 0x80118908  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118908(void)

{
  byte bVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
          if (((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) &&
             (*(char *)(_DAT_800ac784 + 7) == '\0')) {
            *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
            *(undefined1 *)(_DAT_800ac784 + 4) = 2;
            *(undefined1 *)(_DAT_800ac784 + 5) = 9;
            *(undefined1 *)(_DAT_800ac784 + 6) = 3;
            goto LAB_80118dac;
          }
          *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
          local_28 = DAT_801213a8;
          local_24 = DAT_801213ac;
          local_20 = DAT_801213b0;
          local_1c = DAT_801213b4;
          func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                          *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
        }
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x1e2) * 0x1e + 600;
        if (*(char *)(_DAT_800ac784 + 7) == '\x03') {
          *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 0x2d0;
        }
        iVar4 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),
                                (int)*(short *)(_DAT_800ac784 + 0x1d8),0xffffffc4,
                                (int)*(short *)(_DAT_800ac784 + 0x1ba));
        if (iVar4 != 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
          *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
          func_0x800453d0(2);
          *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
          *(undefined1 *)(_DAT_800ac784 + 7) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
          return;
        }
        if (*(char *)(_DAT_800ac784 + 0x95) != '\x13') {
          return;
        }
        if (0x31 < _DAT_800acaee) {
          return;
        }
        if (DAT_800acae7 != '\0') {
          return;
        }
        iVar4 = func_0x8001a804(5000,0xc0,&DAT_800aca88);
        if (-1 < iVar4) {
          return;
        }
        if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 5) = 8;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        func_0x800453d0(7);
        return;
      }
      if (bVar1 != 3) {
        return;
      }
      iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar4 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
LAB_80118dac:
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        return;
      }
      goto LAB_80118dbc;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x1f) + 0xb4;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  }
  if (*(char *)(_DAT_800ac784 + 7) != '\0') {
    sVar3 = func_0x8001aa68(((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4,0x20);
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar3;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\n') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x1f) + 200;
    *(ushort *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) + (ushort)*(byte *)(_DAT_800ac784 + 0x1e2) * 10;
    if (*(char *)(_DAT_800ac784 + 7) == '\x03') {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x32a;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x9f) & 0xf0) << 4;
      *(undefined1 *)(_DAT_800ac784 + 0x82) = 1;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  }
LAB_80118dbc:
  func_0x800245d8(0);
  return;
}


