/* FUN_801179ec @ 0x801179ec  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801179ec(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  uint uVar4;
  uint extraout_v1;
  undefined1 *unaff_s0;
  int iVar5;
  undefined2 unaff_s1;
  int iVar6;
  short in_lo;
  undefined8 uVar7;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = DAT_80100404;
  local_14 = DAT_80100408;
  local_10 = DAT_8010040c;
  local_c = DAT_80100410;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
LAB_80117b18:
    unaff_s0 = &DAT_800aca88;
    iVar5 = func_0x8001a9cc(&DAT_800aca88,800);
    if (((iVar5 != 0) || (0xdac < *(uint *)(_DAT_800ac784 + 0x1d0))) &&
       (*(byte *)(_DAT_800ac784 + 0x95) < 0xf)) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      uVar7 = func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      goto code_r0x8011852c;
    }
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    if (((*(byte *)(_DAT_800ac784 + 0x95) - 0x23 < 8) &&
        (iVar5 = func_0x8001a5e0(*(int *)(_DAT_800ac784 + 0x188) + 0xa14,&local_18,&local_10,
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
    uVar4 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
    if (uVar4 == 0x25) {
      func_0x800453d0(4);
      goto LAB_801184f4;
    }
LAB_80117cf4:
    if (0x31 < *(byte *)(_DAT_800ac784 + 0x95)) {
      FUN_8011bdd8(0,0);
      return;
    }
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
        *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
        if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
          *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
        }
        func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
        goto LAB_80117b18;
      }
    }
    else if (bVar1 == 2) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      uVar4 = extraout_v1;
      goto LAB_80117cf4;
    }
LAB_801184f4:
    *(short *)(unaff_s0 + 0x9e6) = *(short *)(unaff_s0 + 0x9e6) + in_lo * 2;
    func_0x8003a95c();
    *(undefined2 *)(unaff_s0 + 0x9e6) = unaff_s1;
    uVar4 = _DAT_800ac784;
  }
  uVar7 = CONCAT44(uVar4,(uint)*(byte *)(uVar4 + 0x95));
code_r0x8011852c:
  if ((int)uVar7 - 0x47U < 0x1e) {
    iVar5 = *(int *)((int)((ulonglong)uVar7 >> 0x20) + 0x188);
    func_0x8003a95c(iVar5 + 0x4b4,1);
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar6 + 0x5de) =
         *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar6 + 0x560,1);
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar6 + 0x68a) =
         *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar6 + 0x60c,1);
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar6 + 0x93a) =
         *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar6 + 0x8bc,1);
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar6 + 0x9e6) =
         *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar6 + 0x968,1);
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
    *(short *)(iVar5 + 0x5de) = *(short *)(iVar5 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
    func_0x8003a95c(iVar5 + 0x560,1);
    *(undefined2 *)(iVar5 + 0x5de) = 0;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x68a) = *(short *)(iVar5 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
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
    *(short *)(iVar5 + 0x9e6) = *(short *)(iVar5 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
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


