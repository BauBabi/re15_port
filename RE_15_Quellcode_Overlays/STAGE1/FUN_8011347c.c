/* FUN_8011347c @ 0x8011347c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011347c(void)

{
  char cVar1;
  ushort uVar2;
  int iVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) != 0) {
      FUN_80115d94(3);
      uVar2 = *(ushort *)(_DAT_800ac784 + -0x3512) - 4;
      *(ushort *)(_DAT_800ac784 + -0x3512) = uVar2;
      if ((int)((uint)uVar2 << 0x10) < 0) {
        DAT_800aca58 = 3;
        _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
        DAT_800aca59 = 0;
        _DAT_800aca5a = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
      }
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      return;
    }
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    if (*(short *)(_DAT_800ac784 + 0x1ec) < 2000) {
      *(short *)(_DAT_800ac784 + 0x1e4) = -*(short *)(_DAT_800ac784 + 0x1e4);
    }
    iVar3 = _DAT_800ac784;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else {
    iVar3 = _DAT_800ac784;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar3 != 0) {
        *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + -1;
      }
      *(int *)(_DAT_800ac784 + 0x38) =
           (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
      func_0x8001a8f8(&DAT_800aca88,0x32);
      func_0x800245d8(0);
      if (*(char *)(_DAT_800ac784 + 0x1d0) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
        _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x8000;
        _DAT_800acbfc = _DAT_800ac784;
        _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
        _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
        _DAT_800aca5a = 0;
        DAT_800aca59 = 0;
        _DAT_800acaee = _DAT_800acaee + -8;
        DAT_800aca58 = 5;
        FUN_80115d74(0xd);
      }
      cVar1 = *(char *)(_DAT_800ac784 + 0x1d5);
      *(char *)(_DAT_800ac784 + 0x1d5) = cVar1 + -1;
      if (cVar1 == '\0') {
        FUN_80115d74(5);
      }
      return;
    }
  }
  iVar3 = *(int *)(iVar3 + -0x387c);
  if ((*(ushort *)(iVar3 + 0x1dc) < 1000) &&
     (*(short *)(iVar3 + 0x1ba) + -0x2ee < *(int *)(iVar3 + 0x38))) {
    FUN_80115d74(8);
  }
  return;
}


