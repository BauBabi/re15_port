/* FUN_801166e0 @ 0x801166e0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801166e0(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 1) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x14;
  _DAT_800ac784[0x1e] = (uint)PTR_DAT_8011ee7c;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 9;
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x6a4) = 0x60;
  *(undefined2 *)(uVar1 + 0x6a6) = 0x30;
  *(undefined2 *)(uVar1 + 0x6a0) = 0;
  *(undefined2 *)(uVar1 + 0x6a2) = 0;
  *(undefined2 *)(uVar1 + 0x6a8) = 0x200;
  *(undefined2 *)(uVar1 + 0x6aa) = 100;
  _DAT_800ac784[0x76] = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1dd) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x77) = 9;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1de) = 0;
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0x2c,0x808080);
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) == 3) {
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x10;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 9;
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) == 1) {
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 10;
    *_DAT_800ac784 = *_DAT_800ac784 | 8;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x400);
  func_0x8001ee68(_DAT_800ac784);
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 300;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) == 4) {
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x5de) = 0xfff;
    func_0x8003a95c(uVar1 + 0x560,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x68a) = 0xfff;
    func_0x8003a95c(uVar1 + 0x60c,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x93a) = 0xfff;
    func_0x8003a95c(uVar1 + 0x8bc,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x9e6) = 0xfff;
    func_0x8003a95c(uVar1 + 0x968,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0xa92) = 0xfff;
    func_0x8003a95c(uVar1 + 0xa14,1);
    return;
  }
  *(undefined4 *)(_DAT_800ac784[0x62] + 0xac0) = 0;
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0xa92) = 0xfff;
  func_0x8003a95c(uVar1 + 0xa14,1);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x10) != 0) {
    uVar1 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar1 + 0xa1c) = *(undefined4 *)(uVar1 + 0xac8);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa24) = *(undefined4 *)(uVar1 + 0xad0);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa20) = *(undefined4 *)(uVar1 + 0xacc);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa28) = *(undefined4 *)(uVar1 + 0xad4);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x88e) = 0;
    func_0x8003a95c(uVar1 + 0x810,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x68a) = 0;
    func_0x8003a95c(uVar1 + 0x60c,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x736) = 0;
    func_0x8003a95c(uVar1 + 0x6b8,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x7e2) = 0;
    func_0x8003a95c(uVar1 + 0x764,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x532) = 0;
    func_0x8003a95c(uVar1 + 0x4b4,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x5de) = 0;
    func_0x8003a95c(uVar1 + 0x560,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x93a) = 0;
    func_0x8003a95c(uVar1 + 0x8bc,1);
    uVar1 = _DAT_800ac784[0x62];
    *(undefined2 *)(uVar1 + 0x9e6) = 0;
    func_0x8003a95c(uVar1 + 0x968,1);
    if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x10) != 0) goto LAB_80116c08;
  }
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x88e) = 0xfff;
  func_0x8003a95c(uVar1 + 0x810,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x68a) = 0xfff;
  func_0x8003a95c(uVar1 + 0x60c,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x736) = 0xfff;
  func_0x8003a95c(uVar1 + 0x6b8,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x7e2) = 0xfff;
  func_0x8003a95c(uVar1 + 0x764,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x532) = 0xfff;
  func_0x8003a95c(uVar1 + 0x4b4,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x5de) = 0xfff;
  func_0x8003a95c(uVar1 + 0x560,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x93a) = 0xfff;
  func_0x8003a95c(uVar1 + 0x8bc,1);
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x9e6) = 0xfff;
  func_0x8003a95c(uVar1 + 0x968,1);
LAB_80116c08:
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) == 0) {
    uVar1 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar1 + 0xa1c) = *(undefined4 *)(uVar1 + 0xac8);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa24) = *(undefined4 *)(uVar1 + 0xad0);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa20) = *(undefined4 *)(uVar1 + 0xacc);
    *(undefined4 *)(_DAT_800ac784[0x62] + 0xa28) = *(undefined4 *)(uVar1 + 0xad4);
  }
  return;
}


