uint FUN_8001945c(byte *param_1,int *param_2,int param_3,uint param_4)

{
  byte bVar1;
  int iVar2;
  uint *puVar3;
  int iVar4;
  uint uVar5;
  
  uVar5 = 0;
  do {
    bVar1 = *param_1;
    (&DAT_800bbdf0)[uVar5 + (param_4 & 0xff)] = bVar1;
    if (bVar1 == 0xff) break;
    iVar2 = *param_2;
    param_2 = param_2 + -1;
    param_1 = param_1 + 1;
    iVar4 = (uint)bVar1 * 4;
    uVar5 = uVar5 + 1;
    puVar3 = (uint *)(iVar2 + param_3);
    *(uint **)(&DAT_800b2248 + iVar4) = puVar3;
    *(uint **)(&DAT_800b22d4 + iVar4) = puVar3 + (*puVar3 & 0xffff) * 2 + (*puVar3 >> 0x10) + 2;
  } while (uVar5 < 8);
  return uVar5 & 0xff;
}
