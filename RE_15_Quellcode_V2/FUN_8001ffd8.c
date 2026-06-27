void FUN_8001ffd8(int param_1,uint *param_2,uint *param_3,undefined4 param_4,short param_5)

{
  uint uVar1;
  SVECTOR *pSVar2;
  uint *puVar3;
  uint *puVar4;
  int iVar5;
  uint local_48;
  ushort local_44;
  undefined1 auStack_40 [8];
  short local_38;
  byte local_30;
  
  local_38 = param_5;
  local_30 = *(byte *)(DAT_800ac784 + 0x8f);
  while( true ) {
    local_48 = (ushort)*param_3 & 0xfff | (*param_3 & 0xfff000) << 4;
    puVar3 = param_3 + 1;
    local_44 = (ushort)*(byte *)((int)param_3 + 3) | ((ushort)*puVar3 & 0xf) << 8;
    FUN_80020510(param_2 + 0x1e,&local_48,auStack_40,param_4);
    pSVar2 = (SVECTOR *)(param_2 + 0x18);
    iVar5 = 0x1000 - (uint)local_30 * (int)local_38;
    FUN_80020510(pSVar2,auStack_40,pSVar2,iVar5);
    if ((*param_2 & 0x200) == 0) {
      RotMatrix(pSVar2,(MATRIX *)(param_2 + 6));
    }
    if (param_1 == 1) {
      return;
    }
    local_48 = (*puVar3 & 0xfff0) >> 4 | *puVar3 & 0xfff0000;
    local_44 = (ushort)(*puVar3 >> 0x1c) | (ushort)(byte)param_3[2] << 4;
    FUN_80020510(param_2 + 0x49,&local_48,auStack_40,param_4);
    pSVar2 = (SVECTOR *)(param_2 + 0x43);
    FUN_80020510(pSVar2,auStack_40,pSVar2,iVar5);
    if ((param_2[0x2b] & 0x200) == 0) {
      RotMatrix(pSVar2,(MATRIX *)(param_2 + 0x31));
    }
    if (param_1 == 2) {
      return;
    }
    uVar1 = param_3[2];
    local_48 = (uVar1 & 0xfff00) >> 8 | (uVar1 & 0xfff00000) >> 4;
    local_44 = (ushort)param_3[3] & 0xfff;
    FUN_80020510(param_2 + 0x74,&local_48,auStack_40,param_4);
    pSVar2 = (SVECTOR *)(param_2 + 0x6e);
    FUN_80020510(pSVar2,auStack_40,pSVar2,iVar5);
    if ((param_2[0x56] & 0x200) == 0) {
      RotMatrix(pSVar2,(MATRIX *)(param_2 + 0x5c));
    }
    if (param_1 == 3) {
      return;
    }
    uVar1 = param_3[3];
    puVar3 = param_3 + 4;
    local_48 = (uVar1 & 0xfff000) >> 0xc | (uVar1 & 0xff000000) >> 8 | (*puVar3 & 0xf) << 0x18;
    local_44 = (ushort)((*puVar3 & 0xfff0) >> 4);
    FUN_80020510(param_2 + 0x9f,&local_48,auStack_40,param_4);
    pSVar2 = (SVECTOR *)(param_2 + 0x99);
    FUN_80020510(pSVar2,auStack_40,pSVar2,iVar5);
    if ((param_2[0x81] & 0x200) == 0) {
      RotMatrix(pSVar2,(MATRIX *)(param_2 + 0x87));
    }
    if (param_1 == 4) {
      return;
    }
    puVar4 = param_3 + 5;
    local_48 = (int)(*puVar3 & 0xfff0000) >> 0x10 |
               (*puVar3 & 0xf0000000) >> 0xc | (uint)(byte)*puVar4 << 0x14;
    local_44 = (ushort)(*puVar4 >> 8) & 0xfff;
    FUN_80020510(param_2 + 0xca,&local_48,auStack_40,param_4);
    pSVar2 = (SVECTOR *)(param_2 + 0xc4);
    FUN_80020510(pSVar2,auStack_40,pSVar2,iVar5);
    if ((param_2[0xac] & 0x200) == 0) {
      RotMatrix(pSVar2,(MATRIX *)(param_2 + 0xb2));
    }
    if (param_1 == 5) break;
    local_48 = *puVar4 >> 0x14 | (param_3[6] & 0xfff) << 0x10;
    local_44 = (ushort)((param_3[6] & 0xfff000) >> 0xc);
    FUN_80020510(param_2 + 0xf5,&local_48,auStack_40,param_4);
    pSVar2 = (SVECTOR *)(param_2 + 0xef);
    FUN_80020510(pSVar2,auStack_40,pSVar2,iVar5);
    if ((param_2[0xd7] & 0x200) == 0) {
      RotMatrix(pSVar2,(MATRIX *)(param_2 + 0xdd));
    }
    if (param_1 == 6) {
      return;
    }
    puVar3 = param_3 + 7;
    local_48 = (uint)*(byte *)((int)param_3 + 0x1b) | ((ushort)*puVar3 & 0xf) << 8 |
               (*puVar3 & 0xfff0) << 0xc;
    local_44 = (ushort)(*puVar3 >> 0x10) & 0xfff;
    FUN_80020510(param_2 + 0x120,&local_48,auStack_40,param_4);
    pSVar2 = (SVECTOR *)(param_2 + 0x11a);
    FUN_80020510(pSVar2,auStack_40,pSVar2,iVar5);
    if ((param_2[0x102] & 0x200) == 0) {
      RotMatrix(pSVar2,(MATRIX *)(param_2 + 0x108));
    }
    if (param_1 == 7) {
      return;
    }
    puVar4 = param_3 + 8;
    local_48 = *puVar3 >> 0x1c | (uint)(byte)*puVar4 << 4 | (*puVar4 & 0xfff00) << 8;
    local_44 = (ushort)(*puVar4 >> 0x14);
    FUN_80020510(param_2 + 0x14b,&local_48,auStack_40,param_4);
    pSVar2 = (SVECTOR *)(param_2 + 0x145);
    FUN_80020510(pSVar2,auStack_40,pSVar2,iVar5);
    param_3 = param_3 + 9;
    if ((param_2[0x12d] & 0x200) == 0) {
      RotMatrix(pSVar2,(MATRIX *)(param_2 + 0x133));
    }
    param_1 = param_1 + -8;
    param_2 = param_2 + 0x158;
    if (param_1 == 0) {
      return;
    }
  }
  return;
}
