/* FUN_8011696c @ 0x8011696c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011696c(int param_1)

{
  int iVar1;
  uint in_v1;
  uint uVar2;
  int iStack00000010;
  undefined4 uStack00000014;
  
  *(short *)(param_1 + 0x166) = (short)((((in_v1 & 0xf) + 10) * 0x1000) / 10);
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  uVar2 = (uint)*(ushort *)(*(int *)(_DAT_800ac784 + 0x78) + 6);
  iStack00000010 = _DAT_800ac784 + 0xb0;
  uStack00000014 = 0x808080;
  func_0x8001af5c(0,0,uVar2 + 100,uVar2 + 200);
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(int *)(iVar1 + 0xc84) = iVar1 + 0xec;
  *(int *)(iVar1 + 0xca8) = iVar1 + 0xac;
  *(undefined4 *)(iVar1 + 0xc44) = 0x66;
  *(undefined4 *)(iVar1 + 0xc48) = 0xfffffcd6;
  *(undefined4 *)(iVar1 + 0xc4c) = 0;
  *(undefined2 *)(iVar1 + 0xc78) = 0;
  *(undefined2 *)(iVar1 + 0xc7a) = 0;
  *(undefined2 *)(iVar1 + 0xc7c) = 0;
  func_0x80068098(iVar1 + 0xc78,iVar1 + 0xc30);
  return;
}


