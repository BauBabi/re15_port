/* FUN_801112dc @ 0x801112dc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801112dc(void)

{
  uint uVar1;
  char cVar2;
  int in_v1;
  
  *(undefined2 *)(in_v1 + 0x1be) = _DAT_800aca90;
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0x19;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x9f) = 0;
  cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  if (cVar2 == '@') {
    _DAT_800ac784[1] = 4;
    cVar2 = *(char *)((int)_DAT_800ac784 + 9);
  }
  if (cVar2 == 'A') {
    _DAT_800ac784[1] = 0x104;
  }
  if (*(char *)((int)_DAT_800ac784 + 9) == 'C') {
    _DAT_800ac784[1] = 0x204;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  uVar1 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar1 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee7a60);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  _DAT_800ac784[0x1e] = _DAT_8011a0ac;
  func_0x8001af5c(0,0,1000,0x44c);
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}


