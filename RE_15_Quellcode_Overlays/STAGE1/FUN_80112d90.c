/* FUN_80112d90 @ 0x80112d90  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112d90(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        func_0x8001a8f8(&DAT_800aca88,0x32);
        func_0x800245d8(0);
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        iVar5 = func_0x8001a804(9000,100,&DAT_800aca88);
        if (iVar5 < 0) {
          FUN_80115d74(0xb);
        }
        return;
      }
      goto LAB_80113758;
    }
    if (bVar1 != 0) goto LAB_80113758;
    FUN_80115d94(6);
    func_0x800453d0(1);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,0x32);
  if (*(short *)(_DAT_800ac784 + 0x1ea) + -0xe10 < *(int *)(_DAT_800ac784 + 0x38)) {
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    FUN_80113694();
    return;
  }
  if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ea) + -0x1518) {
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    iVar5 = _DAT_800ac784;
    iVar4 = (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
    *(int *)(_DAT_800ac784 + 0x38) = iVar4;
    *(char *)(iVar5 + 0x1d6) = (char)iVar4 + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 100;
    FUN_801161e8();
    if (_DAT_800acaee < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    }
  }
  else {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x32;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
  }
LAB_80113758:
  func_0x80019700();
  func_0x800453d0(2);
  *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar5 != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
  }
  cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    func_0x800453d0(2);
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
  }
  if ((uint)*(byte *)(_DAT_800ac784 + 0x1d5) % 10 == 0) {
    func_0x800453d0(0);
  }
  sVar3 = func_0x80037024();
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -1 + sVar3 * -3;
  if (*(short *)(_DAT_800ac784 + 0x9c) < 0) {
    _DAT_800aca5a = CONCAT11(DAT_800aca5b,DAT_800aca5a + '\x01');
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x4000;
    FUN_80115d74(0xe);
  }
  return;
}


