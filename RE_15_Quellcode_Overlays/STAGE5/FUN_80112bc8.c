/* FUN_80112bc8 @ 0x80112bc8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112bc8(void)

{
  int in_v0;
  int iVar1;
  uint uVar2;
  int iVar3;
  
  iVar1 = func_0x8001f314(*(undefined4 *)(in_v0 + 0x84),*(undefined4 *)(in_v0 + 0x16c),0,0x200);
  iVar3 = (uint)*(byte *)(_DAT_800ac784 + 6) + iVar1;
  *(char *)(_DAT_800ac784 + 6) = (char)iVar3;
  *(char *)(iVar3 + 5) = (char)iVar1;
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  if (*(char *)(_DAT_800ac784 + 9) == '\t') {
    uVar2 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x3c + (uVar2 & 0xf);
    if (-0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}


