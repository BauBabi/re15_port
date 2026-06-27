/* FUN_8010bcb8 @ 0x8010bcb8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bcb8(void)

{
  uint uVar1;
  char cVar2;
  
  _DAT_800ac784[1] = 1;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 1) != 0) {
    _DAT_800ac784[1] = 0x101;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x6f) = _DAT_800aca88;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 2;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 1;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 0;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 2) != 0) {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 1;
  }
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 1;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 4) != 0) {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 0;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x79) = 0;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 8) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 0x79) = 4;
  }
  cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  if (cVar2 == '@') {
    _DAT_800ac784[1] = 4;
    cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  }
  if (cVar2 == 'A') {
    _DAT_800ac784[1] = 0x104;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee1ef8);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x14cc;
  _DAT_800ac784[0x1e] = *(uint *)(&LAB_8011f9dc + (uint)(byte)_DAT_800ac784[0x79] * 4);
  func_0x8001af5c(0,0,*(ushort *)(_DAT_800ac784[0x1e] + 6) + 100,
                  *(ushort *)(_DAT_800ac784[0x1e] + 6) + 200,_DAT_800ac784 + 0x2c,0x808080);
  uVar1 = _DAT_800ac784[0x62];
  *(uint *)(uVar1 + 0xc84) = uVar1 + 0xec;
  *(uint *)(uVar1 + 0xca8) = uVar1 + 0xac;
  *(undefined4 *)(uVar1 + 0xc44) = 0x66;
  *(undefined4 *)(uVar1 + 0xc48) = 0xfffffcd6;
  *(undefined4 *)(uVar1 + 0xc4c) = 0;
  *(undefined2 *)(uVar1 + 0xc78) = 0;
  *(undefined2 *)(uVar1 + 0xc7a) = 0;
  *(undefined2 *)(uVar1 + 0xc7c) = 0;
  func_0x80068098(uVar1 + 0xc78,uVar1 + 0xc30);
  return;
}


