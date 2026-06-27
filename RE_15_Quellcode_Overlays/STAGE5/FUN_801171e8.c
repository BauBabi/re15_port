/* FUN_801171e8 @ 0x801171e8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801171e8(void)

{
  bool bVar1;
  char cVar2;
  short sVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int unaff_s0;
  undefined2 unaff_s1;
  int iVar7;
  short in_lo;
  undefined1 auStackX_0 [16];
  
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 6);
  uVar4 = (uint)(uVar6 < 2);
  if (uVar6 != 1) {
    if (uVar4 == 0) {
      uVar4 = 2;
      if (uVar6 != 2) goto LAB_80117be0;
      func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),
                      0x20);
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
      if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
        *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 10;
      }
      cVar2 = *(char *)(_DAT_800ac784 + 0x9e);
      *(char *)(_DAT_800ac784 + 0x9e) = cVar2 + -1;
      if (cVar2 == '\0') {
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      }
      goto LAB_801173e0;
    }
    bVar1 = uVar6 != 0;
    uVar6 = 1;
    if (bVar1) goto LAB_80117be0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x20
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  FUN_8011bdd8(1,1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0x14;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 10;
  }
  iVar5 = func_0x8001a9cc(&DAT_800aca88,0x400);
  if ((iVar5 != 0) && (*(char *)(_DAT_800ac784 + 0x8f) == '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    uVar6 = _DAT_800ac784;
    uVar4 = 0x5a;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x5a;
LAB_80117be0:
    if (((uVar4 - 0x23 < 8) &&
        (iVar5 = func_0x8001a5e0(*(int *)(uVar6 + 0x188) + 0xa14,&stack0xfffffff8,auStackX_0,
                                 &DAT_800aca54), iVar5 != 0)) && (DAT_800acae7 == '\0')) {
      _DAT_800aca58 = 0x205;
      if (0x25 < *(byte *)(_DAT_800ac784 + 0x95)) {
        _DAT_800aca58 = 0x202;
      }
      _DAT_800acbfc = _DAT_800ac784;
      _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
      _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
      func_0x800453d0(7);
      _DAT_800acaee = _DAT_800acaee - 10;
      if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
        _DAT_800acaee = 1;
      }
    }
    uVar6 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
    if (uVar6 == 0x25) {
      func_0x800453d0(4);
      *(short *)(unaff_s0 + 0x9e6) = *(short *)(unaff_s0 + 0x9e6) + in_lo * 2;
      func_0x8003a95c();
      *(undefined2 *)(unaff_s0 + 0x9e6) = unaff_s1;
      uVar6 = _DAT_800ac784;
    }
    else if (0x31 < *(byte *)(_DAT_800ac784 + 0x95)) {
      FUN_8011bdd8(0,0);
      return;
    }
    if (*(byte *)(uVar6 + 0x95) - 0x47 < 0x1e) {
      iVar5 = *(int *)(uVar6 + 0x188);
      func_0x8003a95c(iVar5 + 0x4b4,1);
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar7 + 0x5de) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar7 + 0x560,1);
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar7 + 0x68a) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar7 + 0x60c,1);
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar7 + 0x93a) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar7 + 0x8bc,1);
      iVar7 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar7 + 0x9e6) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar7 + 0x968,1);
      *(short *)(iVar5 + 0x532) = *(short *)(iVar5 + 0x532) + -0x88;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x532) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x4b4,1);
      *(undefined2 *)(iVar5 + 0x532) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x5de) =
           *(short *)(iVar5 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x560,1);
      *(undefined2 *)(iVar5 + 0x5de) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x68a) =
           *(short *)(iVar5 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x60c,1);
      *(undefined2 *)(iVar5 + 0x68a) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x93a) =
           *(short *)(iVar5 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x8bc,1);
      *(undefined2 *)(iVar5 + 0x93a) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x9e6) =
           *(short *)(iVar5 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x968,1);
      *(undefined2 *)(iVar5 + 0x9e6) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    FUN_801190dc();
    return;
  }
LAB_801173e0:
  func_0x800245d8(0);
  return;
}


