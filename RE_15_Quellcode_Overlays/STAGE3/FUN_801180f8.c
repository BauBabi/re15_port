/* FUN_801180f8 @ 0x801180f8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801180f8(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = DAT_801003bc;
  local_14 = DAT_801003c0;
  local_10 = DAT_80100398;
  local_c = DAT_8010039c;
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80118a70();
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
      *(undefined1 *)(_DAT_800ac784 + 0x1df) = 10;
      if ((*(char *)(_DAT_800ac784 + 0x95) == '\x01') || (*(char *)(_DAT_800ac784 + 0x95) == '\x12')
         ) {
        func_0x800453d0(4);
      }
      if ((((*(byte *)(_DAT_800ac784 + 0x95) - 4 < 10) ||
           (*(byte *)(_DAT_800ac784 + 0x95) - 0x12 < 10)) &&
          (iVar3 = func_0x8001a5e0(iVar3 + 0xa14,&local_18,&local_10,&DAT_800aca54), iVar3 != 0)) &&
         (DAT_800acae7 == '\0')) {
        _DAT_800aca58 = 0x202;
        func_0x800453d0(7);
        _DAT_800acaee = _DAT_800acaee - 5;
        if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
          _DAT_800acaee = 1;
        }
      }
      return;
    }
    if (bVar1 != 0) {
      FUN_80118a70();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x8001aac4(_DAT_800aca88,_DAT_800aca90,0x50);
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_80118a70();
  return;
}


