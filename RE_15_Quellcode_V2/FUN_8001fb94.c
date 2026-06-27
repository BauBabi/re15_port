void FUN_8001fb94(int param_1,uint *param_2,uint *param_3,undefined4 param_4)

{
  uint uVar1;
  uint *puVar2;
  uint *puVar3;
  uint local_30;
  ushort local_2c;
  
  while( true ) {
    local_30 = (ushort)*param_3 & 0xfff | (*param_3 & 0xfff000) << 4;
    puVar2 = param_3 + 1;
    local_2c = (ushort)*(byte *)((int)param_3 + 3) | ((ushort)*puVar2 & 0xf) << 8;
    FUN_80020510(param_2 + 0x1e,&local_30,(SVECTOR *)(param_2 + 0x18),param_4);
    if ((*param_2 & 0x200) == 0) {
      RotMatrix((SVECTOR *)(param_2 + 0x18),(MATRIX *)(param_2 + 6));
    }
    if (param_1 == 1) {
      return;
    }
    local_30 = (*puVar2 & 0xfff0) >> 4 | *puVar2 & 0xfff0000;
    local_2c = (ushort)(*puVar2 >> 0x1c) | (ushort)(byte)param_3[2] << 4;
    FUN_80020510(param_2 + 0x49,&local_30,(SVECTOR *)(param_2 + 0x43),param_4);
    if ((param_2[0x2b] & 0x200) == 0) {
      RotMatrix((SVECTOR *)(param_2 + 0x43),(MATRIX *)(param_2 + 0x31));
    }
    if (param_1 == 2) {
      return;
    }
    uVar1 = param_3[2];
    local_30 = (uVar1 & 0xfff00) >> 8 | (uVar1 & 0xfff00000) >> 4;
    local_2c = (ushort)param_3[3] & 0xfff;
    FUN_80020510(param_2 + 0x74,&local_30,(SVECTOR *)(param_2 + 0x6e),param_4);
    if ((param_2[0x56] & 0x200) == 0) {
      RotMatrix((SVECTOR *)(param_2 + 0x6e),(MATRIX *)(param_2 + 0x5c));
    }
    if (param_1 == 3) {
      return;
    }
    uVar1 = param_3[3];
    puVar2 = param_3 + 4;
    local_30 = (uVar1 & 0xfff000) >> 0xc | (uVar1 & 0xff000000) >> 8 | (*puVar2 & 0xf) << 0x18;
    local_2c = (ushort)((*puVar2 & 0xfff0) >> 4);
    FUN_80020510(param_2 + 0x9f,&local_30,(SVECTOR *)(param_2 + 0x99),param_4);
    if ((param_2[0x81] & 0x200) == 0) {
      RotMatrix((SVECTOR *)(param_2 + 0x99),(MATRIX *)(param_2 + 0x87));
    }
    if (param_1 == 4) {
      return;
    }
    puVar3 = param_3 + 5;
    local_30 = (int)(*puVar2 & 0xfff0000) >> 0x10 |
               (*puVar2 & 0xf0000000) >> 0xc | (uint)(byte)*puVar3 << 0x14;
    local_2c = (ushort)(*puVar3 >> 8) & 0xfff;
    FUN_80020510(param_2 + 0xca,&local_30,(SVECTOR *)(param_2 + 0xc4),param_4);
    if ((param_2[0xac] & 0x200) == 0) {
      RotMatrix((SVECTOR *)(param_2 + 0xc4),(MATRIX *)(param_2 + 0xb2));
    }
    if (param_1 == 5) break;
    local_30 = *puVar3 >> 0x14 | (param_3[6] & 0xfff) << 0x10;
    local_2c = (ushort)((param_3[6] & 0xfff000) >> 0xc);
    FUN_80020510(param_2 + 0xf5,&local_30,(SVECTOR *)(param_2 + 0xef),param_4);
    if ((param_2[0xd7] & 0x200) == 0) {
      RotMatrix((SVECTOR *)(param_2 + 0xef),(MATRIX *)(param_2 + 0xdd));
    }
    if (param_1 == 6) {
      return;
    }
    puVar2 = param_3 + 7;
    local_30 = (uint)*(byte *)((int)param_3 + 0x1b) | ((ushort)*puVar2 & 0xf) << 8 |
               (*puVar2 & 0xfff0) << 0xc;
    local_2c = (ushort)(*puVar2 >> 0x10) & 0xfff;
    FUN_80020510(param_2 + 0x120,&local_30,(SVECTOR *)(param_2 + 0x11a),param_4);
    if ((param_2[0x102] & 0x200) == 0) {
      RotMatrix((SVECTOR *)(param_2 + 0x11a),(MATRIX *)(param_2 + 0x108));
    }
    if (param_1 == 7) {
      return;
    }
    puVar3 = param_3 + 8;
    local_30 = *puVar2 >> 0x1c | (uint)(byte)*puVar3 << 4 | (*puVar3 & 0xfff00) << 8;
    local_2c = (ushort)(*puVar3 >> 0x14);
    FUN_80020510(param_2 + 0x14b,&local_30,(SVECTOR *)(param_2 + 0x145),param_4);
    param_3 = param_3 + 9;
    if ((param_2[0x12d] & 0x200) == 0) {
      RotMatrix((SVECTOR *)(param_2 + 0x145),(MATRIX *)(param_2 + 0x133));
    }
    param_1 = param_1 + -8;
    param_2 = param_2 + 0x158;
    if (param_1 == 0) {
      return;
    }
  }
  return;
}
