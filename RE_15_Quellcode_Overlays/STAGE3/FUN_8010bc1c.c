/* FUN_8010bc1c @ 0x8010bc1c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bc1c(void)

{
  uint uVar1;
  
  _DAT_800ac784[1] = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 3;
  uVar1 = _DAT_800ac784[0x62];
  *(undefined2 *)(uVar1 + 0x29c) = 0x48;
  *(undefined2 *)(uVar1 + 0x29e) = 0x30;
  *(undefined2 *)(uVar1 + 0x2a0) = 0x390;
  *(undefined2 *)(uVar1 + 0x298) = 0;
  *(undefined2 *)(uVar1 + 0x29a) = 0;
  *(undefined2 *)(uVar1 + 0x2a2) = 0x138;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x70) = 0;
  _DAT_800ac784[0x1e] = DAT_8011ea88;
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,1000,500,_DAT_800ac784 + 0x2c,0x808080);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee2e38);
  *(undefined2 *)((int)_DAT_800ac784 + 0x1d6) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1e6) = 0;
  _DAT_800aca50 = 0;
  if (*(char *)((int)_DAT_800ac784 + 9) == '@') {
    _DAT_800ac784[1] = 4;
  }
  if (*(char *)((int)_DAT_800ac784 + 9) == 'A') {
    _DAT_800ac784[1] = 0x104;
  }
  return;
}


