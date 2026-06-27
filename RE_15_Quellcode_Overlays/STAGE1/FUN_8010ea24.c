/* FUN_8010ea24 @ 0x8010ea24  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ea24(void)

{
  char in_v0;
  char cVar1;
  int iVar2;
  uint uVar3;
  
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + in_v0;
  if (0xc < *(byte *)(_DAT_800ac784 + 0x95)) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 6;
    func_0x800245d8(0);
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0xdc;
    if ((DAT_800acae7 == 0) && (iVar2 = func_0x8001a804(2000,0x180,&DAT_800aca88), iVar2 < 0)) {
      _DAT_800acaee = _DAT_800acaee - 10;
      if ((*(short *)(_DAT_800ac784 + 0x1e4) == 0) && (-1 < (int)((uint)_DAT_800acaee << 0x10))) {
        func_0x800453d0(3);
        *(undefined1 *)(_DAT_800ac784 + 6) = 5;
        DAT_800aca58 = 2;
        cVar1 = func_0x8001a780(&DAT_800aca54);
        DAT_800aca59 = cVar1 + 2;
        DAT_800aca5a = 0;
        DAT_800acae7 = DAT_800acae7 | 1;
        uVar3 = func_0x8001af20();
        func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fedeff0 + (uVar3 & 1)),0x32);
        FUN_8010f7f0();
        return;
      }
      cVar1 = func_0x8001a780(&DAT_800aca54);
      *(char *)(_DAT_800ac784 + 5) = cVar1 + '\t';
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
      FUN_8010f7f0();
      return;
    }
  }
  return;
}


