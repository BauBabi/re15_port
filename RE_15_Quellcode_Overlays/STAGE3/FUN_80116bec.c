/* FUN_80116bec @ 0x80116bec  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116bec(void)

{
  char cVar1;
  short sVar2;
  uint uVar3;
  int unaff_s0;
  int iVar4;
  undefined2 unaff_s1;
  int iVar5;
  short in_lo;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    _DAT_800acaee = 1;
    uVar3 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
    if (uVar3 == 0x25) {
      func_0x800453d0(4);
      *(short *)(unaff_s0 + 0x9e6) = *(short *)(unaff_s0 + 0x9e6) + in_lo * 2;
      func_0x8003a95c();
      *(undefined2 *)(unaff_s0 + 0x9e6) = unaff_s1;
      uVar3 = _DAT_800ac784;
    }
    else if (0x31 < *(byte *)(_DAT_800ac784 + 0x95)) {
      FUN_8011b5c4(0,0);
      return;
    }
    if (*(byte *)(uVar3 + 0x95) - 0x47 < 0x1e) {
      iVar4 = *(int *)(uVar3 + 0x188);
      func_0x8003a95c(iVar4 + 0x4b4,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar5 + 0x5de) =
           *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar5 + 0x560,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar5 + 0x68a) =
           *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar5 + 0x60c,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar5 + 0x93a) =
           *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar5 + 0x8bc,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar5 + 0x9e6) =
           *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar5 + 0x968,1);
      *(short *)(iVar4 + 0x532) = *(short *)(iVar4 + 0x532) + -0x88;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x532) =
           *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar4 + 0x4b4,1);
      *(undefined2 *)(iVar4 + 0x532) = 0;
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x5de) =
           *(short *)(iVar4 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar4 + 0x560,1);
      *(undefined2 *)(iVar4 + 0x5de) = 0;
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x68a) =
           *(short *)(iVar4 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar4 + 0x60c,1);
      *(undefined2 *)(iVar4 + 0x68a) = 0;
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x93a) =
           *(short *)(iVar4 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar4 + 0x8bc,1);
      *(undefined2 *)(iVar4 + 0x93a) = 0;
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x9e6) =
           *(short *)(iVar4 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar4 + 0x968,1);
      *(undefined2 *)(iVar4 + 0x9e6) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    FUN_801188c8();
    return;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x30
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  FUN_8011b5c4(1,1);
  iVar4 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar4 >> 0x10) - (iVar4 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}


