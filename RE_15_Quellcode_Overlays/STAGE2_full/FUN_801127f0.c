/* FUN_801127f0 @ 0x801127f0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801127f0(void)

{
  int *piVar1;
  short sVar2;
  int iVar3;
  byte bVar4;
  char cVar5;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 1:
    iVar3 = _DAT_800ac784;
    sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
    piVar1 = (int *)(_DAT_800ac784 + 0x38);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + 1;
    *(int *)(iVar3 + 0x38) = (int)sVar2 * (int)sVar2 * 8 + *piVar1;
    *(short *)(_DAT_800ac784 + 0x6c) = *(short *)(_DAT_800ac784 + 0x6c) + 0x155;
    if (-1 < *(short *)(_DAT_800ac784 + 0x6c)) {
      *(undefined2 *)(_DAT_800ac784 + 0x6c) = 0;
    }
    if ((int)*(short *)(_DAT_800ac784 + 0x1ba) < *(int *)(_DAT_800ac784 + 0x38)) {
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      *(undefined2 *)(_DAT_800ac784 + 0x6c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
      sVar2 = *(short *)(_DAT_800ac784 + 0x1ba);
      *(char *)(_DAT_800ac784 + 0x82) =
           (char)(sVar2 >> 0xf) - ((char)((int)sVar2 / 0x708) + (char)(sVar2 >> 7));
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if ((*(short *)(_DAT_800ac784 + 0x1d2) != 0) && (DAT_800acae7 == 0)) {
      func_0x800453d0(2);
      DAT_800aca58 = 2;
      cVar5 = func_0x8001a780(&DAT_800aca54);
      DAT_800aca59 = cVar5 + '\x02';
      DAT_800aca5a = 0;
      DAT_800acae7 = DAT_800acae7 | 1;
      if (_DAT_800acaee < 0) {
        _DAT_800acaee = 1;
      }
      func_0x80037edc(0,10);
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    func_0x800453d0(6);
    bVar4 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 9) = bVar4 & 1;
  case 3:
    cVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar5;
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


