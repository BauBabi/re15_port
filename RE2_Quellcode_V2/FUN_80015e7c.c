/* FUN_80015e7c @ 0x80015e7c  (Ghidra headless, raw MIPS overlay) */

void FUN_80015e7c(int param_1,int param_2,int param_3,int param_4)

{
  short sVar1;
  short sVar2;
  short sVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  short sVar9;
  
  uVar4 = *(uint *)(param_3 + (uint)*(byte *)(param_1 + 0x14c) * 4);
  if (param_4 == 0) {
    uVar5 = (uint)*(byte *)(param_1 + 0x14d);
  }
  else {
    uVar5 = ((uVar4 & 0xffff) - (uint)*(byte *)(param_1 + 0x14d)) - 1;
  }
  uVar7 = *(uint *)(param_2 + 4) >> 0x12;
  iVar8 = param_2 + (uint)*(ushort *)(param_2 + 2) +
          uVar7 * (*(uint *)(param_3 + (uVar4 >> 0x10) + uVar5 * 4) & 0xfff) * 4;
  iVar6 = *(int *)(iVar8 + 8);
  sVar1 = *(short *)(iVar8 + 6);
  sVar9 = (short)((uint)iVar6 >> 0x10);
  if (*(char *)(param_1 + 0x14d) == '\0') {
    *(undefined4 *)(param_1 + 0x20c) = 0;
    *(undefined2 *)(param_1 + 0x210) = 0;
    if (param_4 != 0) {
      *(short *)(param_1 + 0x20c) = sVar1;
      *(short *)(param_1 + 0x20e) = (short)iVar6;
      *(short *)(param_1 + 0x210) = sVar9;
      iVar8 = param_2 + (uint)*(ushort *)(param_2 + 2) +
              uVar7 * (*(uint *)(param_3 +
                                (uint)*(ushort *)
                                       (param_3 + (uint)*(byte *)(param_1 + 0x14c) * 4 + 2)) & 0xfff
                      ) * 4;
      *(short *)(param_1 + 0x20c) = *(short *)(param_1 + 0x20c) + *(short *)(iVar8 + 6);
      *(short *)(param_1 + 0x20e) = *(short *)(param_1 + 0x20e) + (short)*(undefined4 *)(iVar8 + 8);
      *(short *)(param_1 + 0x210) = *(short *)(param_1 + 0x210) + *(short *)(iVar8 + 10);
    }
  }
  sVar2 = *(short *)(param_1 + 0x20c);
  sVar3 = *(short *)(param_1 + 0x210);
  *(int *)(param_1 + 0x20c) = (int)sVar1 | iVar6 << 0x10;
  *(short *)(param_1 + 0x210) = sVar9;
  *(short *)(param_1 + 0x144) = sVar1 - sVar2;
  *(short *)(param_1 + 0x146) = (short)iVar6 - *(short *)(param_1 + 0x20e);
  *(short *)(param_1 + 0x148) = sVar9 - sVar3;
  return;
}


