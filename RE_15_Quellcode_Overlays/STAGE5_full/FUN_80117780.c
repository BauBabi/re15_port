/* FUN_80117780 @ 0x80117780  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117780(void)

{
  short sVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  uint uVar5;
  
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x338);
  sVar2 = func_0x8001a9cc(&DAT_800aca88,0x10);
  sVar3 = func_0x8001a9cc(&DAT_800aca88,500);
  sVar4 = func_0x8001a9cc(&DAT_800aca88,0x464);
  if ((*_DAT_800ac784 & 0x20) == 0) {
    if ((((*(char *)((int)_DAT_800ac784 + 0x1df) == '\0') && (_DAT_800ac784[0x74] < 0xc80)) &&
        (DAT_800aca58 != '\x05')) && (sVar1 == 0)) {
      _DAT_800ac784[1] = 0x301;
    }
    if (((_DAT_800ac784[0x74] < 0x9c4) && (DAT_800aca58 != '\x05')) && (sVar3 == 0)) {
      _DAT_800ac784[1] = 0x401;
    }
    if ((_DAT_800ac784[0x74] < 0xed8) && (sVar2 != 0)) {
      if (DAT_800aca58 == '\x05') goto LAB_80117940;
      if (((sVar4 == 0) && (100 < _DAT_800acae0)) && (*(char *)((int)_DAT_800ac784 + 0x1df) == '\0')
         ) {
        uVar5 = 0x701;
LAB_80117928:
        _DAT_800ac784[1] = uVar5;
      }
    }
  }
  else if (_DAT_800ac784[0x74] < 0x9c4) {
    if (DAT_800aca58 == '\x05') goto LAB_80117940;
    if (sVar3 == 0) {
      uVar5 = 0x401;
      goto LAB_80117928;
    }
  }
  if (DAT_800aca58 != '\x05') {
    return;
  }
LAB_80117940:
  if ((_DAT_800ac784[0x74] < 3000) && (sVar2 == 0)) {
    _DAT_800ac784[1] = 0x501;
  }
  if (_DAT_800ac784[0x74] < 2000) {
    _DAT_800ac784[1] = 0x801;
  }
  if (((*(byte *)((int)_DAT_800ac784 + 9) & 0x10) == 0) &&
     ((*(byte *)((int)_DAT_800ac784 + 0x1dd) & 4) != 0)) {
    _DAT_800ac784[1] = 0x601;
  }
  return;
}


