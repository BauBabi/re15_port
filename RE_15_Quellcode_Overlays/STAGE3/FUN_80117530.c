/* FUN_80117530 @ 0x80117530  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117530(void)

{
  char cVar1;
  short sVar2;
  uint uVar3;
  uint extraout_v1;
  int unaff_s0;
  int iVar4;
  int unaff_s2;
  undefined4 local_18;
  undefined4 local_14;
  uint local_10;
  undefined4 local_c;
  
  local_18 = DAT_801003cc;
  local_14 = DAT_801003d0;
  local_10 = DAT_801003d4;
  local_c = DAT_801003d8;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar3 != 1) {
    if (1 < uVar3) {
      sVar2 = 0x101;
      if (uVar3 != 2) goto code_r0x80117f64;
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      goto LAB_80117760;
    }
    sVar2 = 1;
    if (uVar3 != 0) goto code_r0x80117f64;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    func_0x800453d0(3);
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (*(char *)(_DAT_800ac784 + 0x95) == ',') {
    iVar4 = func_0x8001a5e0(*(int *)(_DAT_800ac784 + 0x188) + 0xa14,&local_18,&local_10,
                            &DAT_800aca54);
    if (iVar4 != 0) {
      _DAT_800acbfc = _DAT_800ac784;
      _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
      _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
      _DAT_800aca58 = 6;
      _DAT_800acc1e = (undefined2)local_c;
      _DAT_800acc1c = (undefined2)local_10;
      *(short *)(unaff_s0 + 0x5de) =
           *(short *)(unaff_s0 + 0x5de) +
           *(char *)(*(int *)((local_10 & 0xffff) - 0x387c) + 0x9e) * -6;
      func_0x8003a95c();
      *(undefined2 *)(unaff_s0 + 0x5de) = 0;
      unaff_s0 = *(int *)(_DAT_800ac784 + 0x188);
      unaff_s2 = unaff_s0 + 0x60c;
      sVar2 = func_0x8001af20();
      uVar3 = _DAT_800ac784;
      goto code_r0x80117f64;
    }
    func_0x800453d0(6);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) != '%') {
LAB_80117760:
    FUN_8011b5c4(0,1);
    return;
  }
  sVar2 = func_0x800453d0(4);
  uVar3 = extraout_v1;
code_r0x80117f64:
  *(short *)(unaff_s0 + 0x68a) = *(short *)(unaff_s0 + 0x68a) + *(char *)(uVar3 + 0x9e) * sVar2;
  func_0x8003a95c(unaff_s2,1);
  *(undefined2 *)(unaff_s0 + 0x68a) = 0;
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  sVar2 = func_0x8001af20();
  *(short *)(iVar4 + 0x93a) =
       *(short *)(iVar4 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
  func_0x8003a95c(iVar4 + 0x8bc,1);
  *(undefined2 *)(iVar4 + 0x93a) = 0;
  iVar4 = *(int *)(_DAT_800ac784 + 0x188);
  sVar2 = func_0x8001af20();
  *(short *)(iVar4 + 0x9e6) = *(short *)(iVar4 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
  func_0x8003a95c(iVar4 + 0x968,1);
  *(undefined2 *)(iVar4 + 0x9e6) = 0;
  *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_801188c8();
  return;
}


