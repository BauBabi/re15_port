/* FUN_8010d734 @ 0x8010d734  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d734(void)

{
  char cVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  iVar3 = _DAT_800ac784;
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    FUN_8010ee40((int)*(short *)(_DAT_800ac784 + 0x9c),2);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    *(undefined1 *)(_DAT_ffffc785 + 7) = 0;
    uVar2 = func_0x8001a9cc(iVar3,0x20);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar2;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = *(undefined2 *)(_DAT_800ac784 + 0x1da);
    return;
  }
  iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  if (iVar3 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + 0x800;
    if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x3c;
    }
  }
  if (DAT_800acae7 == 0) {
    local_20 = _DAT_80072d60;
    local_1c = _DAT_80072d64;
    local_18 = _DAT_80072d68;
    local_14 = _DAT_80072d6c;
    uVar2 = func_0x8001bff8(*(int *)(_DAT_800ac784 + 0x188) + 0x64c,&local_20,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar2;
    if ((*(byte *)(_DAT_800ac784 + 0x95) - 7 < 9) && (*(short *)(_DAT_800ac784 + 0x1d8) != 0)) {
      *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x78;
      DAT_800aca58 = 2;
      cVar1 = func_0x8001a780(&DAT_800aca54);
      DAT_800aca59 = cVar1 + '\x02';
      DAT_800aca5a = 0;
      if (_DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        DAT_800aca59 = '\0';
      }
      DAT_800acae7 = DAT_800acae7 | 1;
    }
    if (DAT_800acae7 == 0) {
      return;
    }
  }
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x64c,&local_20);
  return;
}


