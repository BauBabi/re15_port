/* FUN_8004c658 @ 0x8004c658  (Ghidra headless, raw MIPS overlay) */

int FUN_8004c658(uint *param_1,undefined4 param_2,uint param_3)

{
  byte bVar1;
  ushort uVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  short *psVar6;
  short *psVar7;
  int iVar9;
  int iVar10;
  int iVar11;
  short *psVar8;
  
  if ((*param_1 & 8) == 0) {
    iVar11 = param_1[0xe] + (int)(short)param_1[0x28];
    iVar5 = param_1[0x10] + (int)*(short *)((int)param_1 + 0xa2);
    psVar6 = *(short **)(DAT_800ce324 + 0x20);
    iVar4 = *(int *)(psVar6 + 2);
    uVar3 = FUN_8004c198(iVar11,iVar5,(int)*psVar6,(int)psVar6[1]);
    bVar1 = *(byte *)((int)param_1 + 0x106);
    DAT_800c3b78 = 0;
    iVar11 = iVar11 + (uint)(ushort)param_1[0x24];
    iVar5 = iVar5 + (uint)*(ushort *)((int)param_1 + 0x92);
    iVar10 = (uint)(ushort)param_1[0x24] * 2;
    iVar9 = (uint)*(ushort *)((int)param_1 + 0x92) * 2;
    psVar7 = psVar6;
    while (psVar8 = psVar7, psVar7 = psVar8 + 8, psVar7 != psVar6 + iVar4 * 8) {
      if ((((uVar3 & psVar8[0xd]) != 0) &&
          ((uint)(iVar11 - *psVar7) < (uint)(ushort)psVar8[10] + iVar10)) &&
         ((uint)(iVar5 - psVar8[9]) < (uint)(ushort)psVar8[0xb] + iVar9)) {
        uVar2 = psVar8[0xc];
        if ((((uVar2 & 0x4000) != 0) && ((uVar2 & param_3) == 0)) &&
           ((*(uint *)(psVar8 + 0xe) & 1 << (bVar1 & 0x1f)) != 0)) {
          if ((uVar2 & 0xf) == 10) {
            DAT_800c3b78 = DAT_800c3b78 | 4;
          }
          else {
            if (param_3 != 0) {
              iVar5 = iVar9 >> 1;
              FUN_8004c900(param_1,iVar5,psVar7);
              iVar11 = param_1[0xe] + (int)(short)param_1[0x28] + (iVar10 >> 1);
              iVar5 = param_1[0x10] + (int)*(short *)((int)param_1 + 0xa2) + iVar5;
            }
            DAT_800c3b78 = DAT_800c3b78 | 1;
          }
        }
      }
    }
    iVar4 = (int)(short)DAT_800c3b78;
  }
  else {
    iVar4 = 0;
  }
  return iVar4;
}


