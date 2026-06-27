/* FUN_80051088 @ 0x80051088  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80051088(int param_1,uint param_2,uint param_3)

{
  byte bVar1;
  bool bVar2;
  short sVar3;
  short sVar4;
  int iVar5;
  undefined4 uVar6;
  code *pcVar7;
  byte *pbVar8;
  byte *pbVar9;
  short sVar10;
  uint uVar11;
  uint uVar12;
  undefined4 *puVar13;
  short local_68 [2];
  short local_64;
  int local_60 [2];
  int local_58;
  short local_50;
  short sStack_4e;
  short local_4c;
  short local_4a;
  short local_48;
  short local_46;
  short local_44;
  short local_42;
  short local_40;
  short local_3e;
  uint local_38;
  
  local_68[0] = 0x26c;
  local_64 = 0;
  local_38 = param_2;
  FUN_80077384((int)*(short *)(param_1 + 0x76),local_68,local_68);
  uVar11 = 0;
  local_60[0] = (int)local_68[0] + *(int *)(param_1 + 0x38);
  uVar12 = (uint)DAT_800d4230;
  local_58 = (int)local_64 + *(int *)(param_1 + 0x40);
  DAT_800ce330 = param_1;
  if (uVar12 != 0) {
    puVar13 = &DAT_800ce558;
    sVar10 = 0;
    do {
      pbVar9 = (byte *)*puVar13;
      puVar13 = puVar13 + 1;
      DAT_800ead94 = pbVar9;
      if (pbVar9 == (byte *)0x0) {
LAB_800514a4:
        bVar2 = uVar11 < uVar12;
      }
      else {
        DAT_800eade0 = (int)DAT_800d47ec;
        DAT_800eae30 = (int)DAT_800d47ee;
        uVar11 = uVar11 + 1;
        if ((pbVar9[1] & local_38) == 0) goto LAB_800514a4;
        bVar2 = uVar11 < uVar12;
        if (((pbVar9[1] & 0x10) == param_3) &&
           (((pbVar9[2] & 0x80) != 0 ||
            (bVar2 = uVar11 < uVar12, *(byte *)(param_1 + 0x106) == pbVar9[2])))) {
          bVar1 = pbVar9[3];
          if ((bVar1 & 0x80) == 0) {
            local_50 = 0;
            sStack_4e = 0;
          }
          else if ((bVar1 & 0x40) == 0) {
            local_50 = (short)(&DAT_800d035c)[(bVar1 & 0x3f) * 0x7e];
            sStack_4e = (short)(&DAT_800d0364)[(pbVar9[3] & 0x3f) * 0x7e];
          }
          else {
            local_50 = (short)*(undefined4 *)((&DAT_800cfe14)[bVar1 & 0x3f] + 0x38);
            sStack_4e = (short)*(undefined4 *)((&DAT_800cfe14)[pbVar9[3] & 0x3f] + 0x40);
          }
          if ((pbVar9[1] & 0x80) == 0) {
            local_4c = *(short *)(pbVar9 + 4);
            local_4a = *(short *)(pbVar9 + 6);
            local_48 = *(short *)(pbVar9 + 8);
            local_46 = *(short *)(pbVar9 + 10);
          }
          else {
            local_4c = *(short *)(pbVar9 + 4);
            local_4a = *(short *)(pbVar9 + 6);
            local_48 = local_50 + *(short *)(pbVar9 + 8);
            local_46 = sStack_4e + *(short *)(pbVar9 + 10);
            local_44 = local_50 + *(short *)(pbVar9 + 0xc);
            local_42 = sStack_4e + *(short *)(pbVar9 + 0xe);
            local_40 = local_50 + *(short *)(pbVar9 + 0x10);
            local_3e = sStack_4e + *(short *)(pbVar9 + 0x12);
          }
          local_4a = sStack_4e + local_4a;
          local_4c = local_50 + local_4c;
          bVar1 = pbVar9[1];
          if ((bVar1 & 0x40) == 0) {
code_r0x80051390:
            bVar2 = uVar11 < uVar12;
            if ((bVar1 & 0x20) != 0) {
              if ((bVar1 & 0x80) == 0) {
                iVar5 = FUN_8002c904(local_60,&local_4c);
              }
              else {
                iVar5 = FUN_8002c820(local_60,&local_50);
              }
              bVar2 = uVar11 < uVar12;
              sVar3 = sVar10;
              sVar4 = DAT_800d47ee;
              if (iVar5 != 0) goto LAB_800513d0;
            }
          }
          else {
            if ((bVar1 & 0x80) == 0) {
              iVar5 = FUN_8002c904(param_1 + 0x38,&local_4c);
            }
            else {
              iVar5 = FUN_8002c820(param_1 + 0x38,&local_50);
            }
            sVar3 = DAT_800d47ec;
            sVar4 = sVar10;
            if (iVar5 == 0) {
              bVar1 = pbVar9[1];
              goto code_r0x80051390;
            }
LAB_800513d0:
            DAT_800d47ee = sVar4;
            DAT_800d47ec = sVar3;
            if (param_3 == 0) {
              *(char *)(param_1 + 0xb) = (char)sVar10;
              pbVar8 = pbVar9 + 0x14;
              if ((pbVar9[1] & 0x80) == 0) {
                pcVar7 = (code *)(&PTR_LAB_800a73c4)[*pbVar9];
                pbVar8 = pbVar9 + 0xc;
              }
              else {
                pcVar7 = (code *)(&PTR_LAB_800a73c4)[*pbVar9];
              }
              (*pcVar7)(pbVar8);
              goto LAB_800514a4;
            }
            bVar2 = uVar11 < uVar12;
            if (*pbVar9 != 0) {
              *(char *)(param_1 + 0xb) = (char)sVar10;
              if ((pbVar9[1] & 0x80) != 0) {
                uVar6 = (*(code *)(&PTR_LAB_800a73c4)[*pbVar9])(pbVar9 + 0x14);
                return uVar6;
              }
              uVar6 = (*(code *)(&PTR_LAB_800a73c4)[*pbVar9])(pbVar9 + 0xc);
              return uVar6;
            }
          }
        }
      }
      sVar10 = sVar10 + 1;
    } while (bVar2);
  }
  return 0;
}


