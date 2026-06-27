/* FUN_800357d8 @ 0x800357d8  (Ghidra headless, raw MIPS overlay) */

void FUN_800357d8(int param_1,int param_2)

{
  u_short uVar1;
  int iVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  ushort unaff_s1;
  
  iVar6 = *(int *)(param_2 + 0x48);
  uVar1 = GetClut(0x130,param_1 + 0x1e0);
  iVar2 = *(int *)(*(int *)(iVar6 + 0x14) + 0x14);
  if (iVar2 != 0) {
    unaff_s1 = (ushort)((uint)*(undefined4 *)(*(int *)(iVar6 + 0x18) + 0xc) >> 0x16);
  }
  uVar5 = iVar2 << 1;
  uVar4 = 0;
  if (uVar5 != 0) {
    puVar3 = (undefined4 *)(*(int *)(iVar6 + 0x18) + 0xc);
    do {
      uVar4 = uVar4 + 1;
      *puVar3 = CONCAT22(uVar1,*(undefined2 *)puVar3);
      puVar3 = puVar3 + 10;
    } while (uVar4 < uVar5);
  }
  iVar2 = *(int *)(*(int *)(iVar6 + 0x1c) + 0x14);
  uVar4 = iVar2 << 1;
  if (iVar2 != 0) {
    unaff_s1 = (ushort)((uint)*(undefined4 *)(*(int *)(iVar6 + 0x20) + 0xc) >> 0x16);
  }
  uVar5 = 0;
  if (uVar4 != 0) {
    puVar3 = (undefined4 *)(*(int *)(iVar6 + 0x20) + 0xc);
    do {
      uVar5 = uVar5 + 1;
      *puVar3 = CONCAT22(uVar1,*(undefined2 *)puVar3);
      puVar3 = puVar3 + 0xd;
    } while (uVar5 < uVar4);
  }
  uVar4 = 0;
  iVar2 = 0x38;
  do {
    iVar6 = param_2 + iVar2;
    uVar4 = uVar4 + 1;
    *(ushort *)(iVar6 + 2) = unaff_s1;
    *(undefined2 *)(iVar6 + 4) = 0x10;
    *(undefined2 *)(iVar6 + 6) = 1;
    iVar2 = iVar2 + 8;
  } while (uVar4 < 2);
  return;
}


