/* FUN_80116288 @ 0x80116288  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116288(void)

{
  short sVar1;
  char cVar2;
  int iVar3;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) || ((*(byte *)((int)_DAT_800ac784 + 9) & 0x20) != 0)) {
    (*(code *)(&PTR_FUN_80121268)[(byte)_DAT_800ac784[1]])();
    func_0x8002b498(_DAT_800ac784);
    if ((byte)_DAT_800ac784[0x74] < 0xd) {
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    }
    else {
      *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
      *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
      iVar3 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
      if (iVar3 != 0) {
        if (DAT_800acae7 == '\0') {
          DAT_800aca58 = 2;
          cVar2 = func_0x8001a780(&DAT_800aca54);
          DAT_800aca59 = cVar2 + '\x02';
          _DAT_800aca5a = 0;
        }
        if (3 < _DAT_800acaee) {
          _DAT_800acaee = _DAT_800acaee + -2;
        }
      }
    }
    _DAT_800ac784[0xd] = (int)(short)_DAT_800ac784[0x76];
    _DAT_800ac784[0xf] = (int)*(short *)((int)_DAT_800ac784 + 0x1da);
    if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
      sVar1 = *(short *)((int)_DAT_800ac784 + 0x1d6);
      iVar3 = ((byte)_DAT_800ac784[0x74] - 1) * 0x14;
    }
    else {
      sVar1 = *(short *)((int)_DAT_800ac784 + 0x1d6);
      iVar3 = ((byte)_DAT_800ac784[0x74] - 1) * 0x28;
    }
    _DAT_800ac784[0xe] = sVar1 - iVar3;
  }
  return;
}


