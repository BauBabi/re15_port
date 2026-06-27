/* FUN_8011036c @ 0x8011036c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011036c(void)

{
  uint uVar1;
  char cVar2;
  
  _DAT_800ac784[1] = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x10;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0;
  if (*(byte *)((int)_DAT_800ac784 + 9) < 2) {
    *(undefined2 *)(_DAT_800ac784 + 0x1a) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
    _DAT_800ac784[0xe] = (uint)*(byte *)((int)_DAT_800ac784 + 0x82) * -0x708;
  }
  if (*(byte *)((int)_DAT_800ac784 + 9) - 2 < 0x1e) {
    *(undefined2 *)(_DAT_800ac784 + 0x1a) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x800;
    _DAT_800ac784[0xe] = 0xffffd5d0;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 2;
  }
  cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  if (cVar2 == '@') {
    _DAT_800ac784[1] = 4;
    cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  }
  if (cVar2 == 'A') {
    _DAT_800ac784[1] = 0x104;
  }
  _DAT_800ac784[0x1e] = _DAT_80118e04;
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  func_0x8001af5c(0,0,0x514,0x578,_DAT_800ac784 + 0x2c,0x808080);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee8e84);
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0xccc;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1d6) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}


