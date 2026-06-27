/* FUN_8011b5c4 @ 0x8011b5c4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b5c4(int param_1,int param_2)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined1 auStack_38 [20];
  uint local_24;
  uint local_1c;
  
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80068098(_DAT_800ac784 + 0x68,_DAT_800ac784 + 0x20);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar3 + 0x18,auStack_38);
  iVar3 = iVar3 + param_2 * 0x204 + 0x204;
  func_0x80022da0(auStack_38,iVar3 + -0x140,auStack_38);
  func_0x80022da0(auStack_38,iVar3 + -0x94,auStack_38);
  func_0x80022da0(auStack_38,iVar3 + 0x18,auStack_38);
  if (param_1 == 0) {
    *(uint *)(_DAT_800ac784 + 0x34) =
         *(int *)(_DAT_800ac784 + 0x34) - (local_24 - *(int *)(iVar3 + 0x54));
    *(uint *)(_DAT_800ac784 + 0x3c) =
         *(int *)(_DAT_800ac784 + 0x3c) - (local_1c - *(int *)(iVar3 + 0x5c));
  }
  else {
    iVar2 = (int)(((local_24 & 0xffff) - (uint)*(ushort *)(iVar3 + 0x54)) * 0x10000) >> 0x10;
    iVar3 = (int)(((local_1c & 0xffff) - (uint)*(ushort *)(iVar3 + 0x5c)) * 0x10000) >> 0x10;
    uVar1 = func_0x80065f60(iVar2 * iVar2 + iVar3 * iVar3);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar1;
  }
  return;
}


