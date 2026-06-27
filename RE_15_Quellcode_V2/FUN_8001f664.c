void FUN_8001f664(int param_1,int param_2,uint *param_3)

{
  uint uVar1;
  ushort *puVar2;
  uint *puVar3;
  uint *puVar4;
  
  puVar2 = (ushort *)(param_2 + 0x7c);
  do {
    *(uint *)(puVar2 + -2) = (ushort)*param_3 & 0xfff | (*param_3 & 0xfff000) << 4;
    puVar3 = param_3 + 1;
    *puVar2 = (ushort)*(byte *)((int)param_3 + 3) | ((ushort)*puVar3 & 0xf) << 8;
    if (param_1 == 1) {
      return;
    }
    *(uint *)(puVar2 + 0x54) = (*puVar3 & 0xfff0) >> 4 | *puVar3 & 0xfff0000;
    puVar2[0x56] = (ushort)(*puVar3 >> 0x1c) | (ushort)(byte)param_3[2] << 4;
    if (param_1 == 2) {
      return;
    }
    uVar1 = param_3[2];
    *(uint *)(puVar2 + 0xaa) = (uVar1 & 0xfff00) >> 8 | (uVar1 & 0xfff00000) >> 4;
    puVar2[0xac] = (ushort)param_3[3] & 0xfff;
    if (param_1 == 3) {
      return;
    }
    uVar1 = param_3[3];
    puVar3 = param_3 + 4;
    *(uint *)(puVar2 + 0x100) =
         (uVar1 & 0xfff000) >> 0xc | (uVar1 & 0xff000000) >> 8 | (*puVar3 & 0xf) << 0x18;
    puVar2[0x102] = (ushort)((*puVar3 & 0xfff0) >> 4);
    if (param_1 == 4) {
      return;
    }
    puVar4 = param_3 + 5;
    *(uint *)(puVar2 + 0x156) =
         (int)(*puVar3 & 0xfff0000) >> 0x10 |
         (*puVar3 & 0xf0000000) >> 0xc | (uint)(byte)*puVar4 << 0x14;
    puVar2[0x158] = (ushort)(*puVar4 >> 8) & 0xfff;
    if (param_1 == 5) {
      return;
    }
    *(uint *)(puVar2 + 0x1ac) = *puVar4 >> 0x14 | (param_3[6] & 0xfff) << 0x10;
    puVar2[0x1ae] = (ushort)((param_3[6] & 0xfff000) >> 0xc);
    if (param_1 == 6) {
      return;
    }
    puVar3 = param_3 + 7;
    *(uint *)(puVar2 + 0x202) =
         (uint)*(byte *)((int)param_3 + 0x1b) | ((ushort)*puVar3 & 0xf) << 8 |
         (*puVar3 & 0xfff0) << 0xc;
    puVar2[0x204] = (ushort)(*puVar3 >> 0x10) & 0xfff;
    if (param_1 == 7) {
      return;
    }
    puVar4 = param_3 + 8;
    param_1 = param_1 + -8;
    *(uint *)(puVar2 + 600) = *puVar3 >> 0x1c | (uint)(byte)*puVar4 << 4 | (*puVar4 & 0xfff00) << 8;
    param_3 = param_3 + 9;
    puVar2[0x25a] = (ushort)(*puVar4 >> 0x14);
    puVar2 = puVar2 + 0x2b0;
  } while (param_1 != 0);
  return;
}
