/* FUN_8010cdd0 @ 0x8010cdd0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cdd0(void)

{
  int in_v1;
  
  *(undefined1 *)(in_v1 + 6) = 3;
  *(char *)(_DAT_800ac784 + 0x94) = *(char *)(_DAT_800ac784 + 0x94) + '\x01';
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  func_0x800453d0(2);
  _DAT_800aca50 = _DAT_800aca50 | 1;
  FUN_8010d720();
  return;
}


