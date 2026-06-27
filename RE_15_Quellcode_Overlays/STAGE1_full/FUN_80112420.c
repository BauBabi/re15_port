/* FUN_80112420 @ 0x80112420  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112420(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  
  uVar1 = func_0x8001bc08();
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d2) = uVar1;
  *(byte *)((int)_DAT_800ac784 + 0x1d2) = *(byte *)((int)_DAT_800ac784 + 0x1d2) & 1;
  uVar2 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x77) = uVar2;
  *(short *)(_DAT_800ac784 + 0x7b) = _DAT_800aca8c - (short)_DAT_800ac784[0xe];
  if ((_DAT_800aca50 & 0xff00) != 0) {
    FUN_80116068();
  }
  (*(code *)(&PTR_FUN_8012113c)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_LAB_80121184)[*(byte *)((int)_DAT_800ac784 + 5)])();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if ((short)_DAT_800ac784[0x7b] < 0xfa1) {
    if ((short)_DAT_800ac784[0x7b] < 800) {
      func_0x80012974(6000);
    }
    else {
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
    }
  }
  else {
    func_0x80012a0c(6000);
  }
  FUN_80115f70();
  if ((_DAT_800aca50 & 0x800) != 0) {
    if (*(char *)((int)_DAT_800ac784 + 0x1db) == '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xf0ff;
      *(undefined1 *)((int)_DAT_800ac784 + 0x1db) = 1;
    }
  }
  return;
}


