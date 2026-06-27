/* FUN_801181ec @ 0x801181ec  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801181ec(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
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
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
        func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      }
      goto LAB_801184f4;
    }
    if (bVar1 != 0) goto LAB_801184f4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
    }
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
  }
  iVar3 = func_0x8001a9cc(&DAT_800aca88,800);
  if (((iVar3 != 0) || (0xdac < *(uint *)(_DAT_800ac784 + 0x1d0))) &&
     (*(byte *)(_DAT_800ac784 + 0x95) < 0xf)) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
    return;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  if (((*(byte *)(_DAT_800ac784 + 0x95) - 0x23 < 8) &&
      (iVar3 = func_0x8001a5e0(*(int *)(_DAT_800ac784 + 0x188) + 0xa14,&local_18,&local_10,
                               &DAT_800aca54), iVar3 != 0)) && (DAT_800acae7 == '\0')) {
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
  if (*(char *)(_DAT_800ac784 + 0x95) == '%') {
    func_0x800453d0(4);
  }
LAB_801184f4:
  FUN_8011bdd8(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x32);
  return;
}


