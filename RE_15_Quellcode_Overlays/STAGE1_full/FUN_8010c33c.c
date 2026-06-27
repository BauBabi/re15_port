/* FUN_8010c33c @ 0x8010c33c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c33c(void)

{
  byte bVar1;
  ushort uVar2;
  uint uVar3;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x14;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_80120934;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 1;
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x74) = (uVar2 & 0x1f) + 0x1e;
  bVar1 = func_0x8001af20();
  *(byte *)((int)_DAT_800ac784 + 0x1d2) = (bVar1 & 3) + 1;
  uVar3 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar3 + 0x6a4) = 0x60;
  *(undefined2 *)(uVar3 + 0x6a6) = 0x30;
  *(undefined2 *)(uVar3 + 0x6a8) = 0x390;
  *(undefined2 *)(uVar3 + 0x6a0) = 0;
  *(undefined2 *)(uVar3 + 0x6a2) = 0;
  *(undefined2 *)(uVar3 + 0x6aa) = 0x138;
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0x2c,0x808080);
  return;
}


