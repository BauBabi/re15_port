undefined4 FUN_80042bac(int param_1,uint param_2,uint param_3)

{
  byte bVar1;
  bool bVar2;
  short sVar3;
  short sVar4;
  uint uVar5;
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
  FUN_8004f008((int)*(short *)(param_1 + 0x6a),local_68,local_68);
  uVar11 = 0;
  local_60[0] = (int)local_68[0] + *(int *)(param_1 + 0x34);
  uVar12 = (uint)DAT_800afbb4;
  local_58 = (int)local_64 + *(int *)(param_1 + 0x3c);
  DAT_800ac784 = param_1;
  if (uVar12 != 0) {
    puVar13 = &DAT_800ac9b0;
    sVar10 = 0;
    do {
      pbVar9 = (byte *)*puVar13;
      puVar13 = puVar13 + 1;
      DAT_800bbd9c = pbVar9;
      if (pbVar9 == (byte *)0x0) {
LAB_80043018:
        bVar2 = uVar11 < uVar12;
      }
      else {
        _DAT_800bbde8 = (int)DAT_800b0fd0;
        _DAT_800bbeec = (int)DAT_800b0fd2;
        uVar11 = uVar11 + 1;
        if ((pbVar9[1] & local_38) == 0) goto LAB_80043018;
        bVar2 = uVar11 < uVar12;
        if (((pbVar9[1] & 0x10) == param_3) &&
           (((pbVar9[2] & 0x80) != 0 ||
            (bVar2 = uVar11 < uVar12, *(byte *)(param_1 + 0x82) == pbVar9[2])))) {
          bVar1 = pbVar9[3];
          if ((bVar1 & 0x80) == 0) {
            local_50 = 0;
            sStack_4e = 0;
          }
          else if ((bVar1 & 0x40) == 0) {
            local_50 = *(short *)(&DAT_800b3fcc + (bVar1 & 0x3f) * 0x94);
            sStack_4e = *(short *)(&DAT_800b3fd4 + (pbVar9[3] & 0x3f) * 0x94);
          }
          else {
            local_50 = (&DAT_800acc60)[(bVar1 & 0x3f) * 0xfa];
            sStack_4e = (&DAT_800acc68)[(pbVar9[3] & 0x3f) * 0xfa];
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
code_r0x80042ef8:
            bVar2 = uVar11 < uVar12;
            if ((bVar1 & 0x20) != 0) {
              if ((bVar1 & 0x80) == 0) {
                uVar5 = FUN_80042b64(local_60,&local_4c);
                uVar5 = uVar5 & 0xffff;
              }
              else {
                uVar5 = FUN_80014368(local_60,&local_50);
              }
              bVar2 = uVar11 < uVar12;
              sVar3 = sVar10;
              sVar4 = DAT_800b0fd2;
              if (uVar5 != 0) goto LAB_80042f40;
            }
          }
          else {
            if ((bVar1 & 0x80) == 0) {
              uVar5 = FUN_80042b64(param_1 + 0x34,&local_4c);
              uVar5 = uVar5 & 0xffff;
            }
            else {
              uVar5 = FUN_80014368(param_1 + 0x34,&local_50);
            }
            sVar3 = DAT_800b0fd0;
            sVar4 = sVar10;
            if (uVar5 == 0) {
              bVar1 = pbVar9[1];
              goto code_r0x80042ef8;
            }
LAB_80042f40:
            DAT_800b0fd2 = sVar4;
            DAT_800b0fd0 = sVar3;
            if (param_3 == 0) {
              *(char *)(param_1 + 0xb) = (char)sVar10;
              pbVar8 = pbVar9 + 0x14;
              if ((pbVar9[1] & 0x80) == 0) {
                pcVar7 = (code *)(&PTR_LAB_8007469c)[*pbVar9];
                pbVar8 = pbVar9 + 0xc;
              }
              else {
                pcVar7 = (code *)(&PTR_LAB_8007469c)[*pbVar9];
              }
              (*pcVar7)(pbVar8);
              goto LAB_80043018;
            }
            bVar2 = uVar11 < uVar12;
            if (*pbVar9 != 0) {
              *(char *)(param_1 + 0xb) = (char)sVar10;
              if ((pbVar9[1] & 0x80) != 0) {
                uVar6 = (*(code *)(&PTR_LAB_8007469c)[*pbVar9])(pbVar9 + 0x14);
                return uVar6;
              }
              uVar6 = (*(code *)(&PTR_LAB_8007469c)[*pbVar9])(pbVar9 + 0xc);
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
