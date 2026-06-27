void FUN_80015d84(void)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  undefined2 local_60 [2];
  short local_5c;
  undefined2 local_58;
  short local_54;
  undefined2 local_50;
  undefined2 local_4c;
  short local_48;
  short local_44;
  int local_40;
  int *local_38;
  int local_30;
  
  local_38 = (int *)local_60;
  iVar4 = 0x41;
  iVar9 = 0;
  local_40 = 0;
  do {
    uVar3 = *(int *)((int)&DAT_800b8af4 + iVar9) + 0x80U & 0xfff;
    *(uint *)((int)&DAT_800b8af4 + iVar9) = uVar3;
    if (0xf < DAT_800b522c) {
      if (DAT_800b522c < 0x41) {
        iVar2 = *(int *)((int)&DAT_800b8ae8 + iVar9) + *(int *)((int)&DAT_800b8af0 + iVar9);
        *(int *)((int)&DAT_800b8ae4 + iVar9) =
             *(int *)((int)&DAT_800b8ae4 + iVar9) + *(int *)((int)&DAT_800b8aec + iVar9);
        *(int *)((int)&DAT_800b8ae8 + iVar9) = iVar2;
        sVar1 = *(short *)((int)&DAT_800b8ae4 + iVar9 + 2);
        iVar6 = DAT_800b522c * 2 + -2;
        if (0x80 < iVar6) {
          iVar6 = 0x80;
        }
        uVar7 = 0;
        iVar8 = 0x80 - iVar6 >> 2;
        local_30 = iVar4;
        piVar5 = local_38;
        do {
          iVar4 = rcos(uVar3);
          *piVar5 = (int)sVar1 + (iVar8 * iVar4 >> 0xc);
          iVar4 = rsin(uVar3);
          piVar5[4] = (iVar2 >> 0x10) + (iVar8 * iVar4 >> 0xc);
          if ((uVar7 & 1) == 0) {
            *piVar5 = *piVar5 + (local_30 - DAT_800b522c) * -0x20;
          }
          else {
            *piVar5 = (local_30 - DAT_800b522c) * 0x20 + *piVar5;
          }
          if ((uVar7 & 2) == 0) {
            piVar5[4] = piVar5[4] + (local_30 - DAT_800b522c) * -8;
          }
          else {
            piVar5[4] = (local_30 - DAT_800b522c) * 8 + piVar5[4];
          }
          piVar5 = piVar5 + 1;
          uVar7 = uVar7 + 1;
          uVar3 = uVar3 + 0x80;
        } while ((int)uVar7 < 4);
        *(undefined2 *)((int)&DAT_800b8ab8 + iVar9) = local_60[0];
        *(undefined2 *)((int)&DAT_800b8aba + iVar9) = local_50;
        *(short *)((int)&DAT_800b8ac4 + iVar9) = local_5c + 0x100;
        *(undefined2 *)((int)&DAT_800b8ac6 + iVar9) = local_4c;
        *(undefined2 *)((int)&DAT_800b8ad0 + iVar9) = local_58;
        *(short *)((int)&DAT_800b8ad2 + iVar9) = local_48 + 0x40;
        *(short *)((int)&DAT_800b8adc + iVar9) = local_54 + 0x100;
        *(short *)((int)&DAT_800b8ade + iVar9) = local_44 + 0x40;
        iVar4 = local_30;
      }
      else {
        *(undefined2 *)((int)&DAT_800b8aba + iVar9) = 0x50;
        *(undefined2 *)((int)&DAT_800b8ac6 + iVar9) = 0x50;
        *(undefined2 *)((int)&DAT_800b8ab8 + iVar9) = 8;
        *(undefined2 *)((int)&DAT_800b8ac4 + iVar9) = 0x138;
        *(undefined2 *)((int)&DAT_800b8ad0 + iVar9) = 8;
        *(undefined2 *)((int)&DAT_800b8ad2 + iVar9) = 0xa0;
        *(undefined2 *)((int)&DAT_800b8adc + iVar9) = 0x138;
        *(undefined2 *)((int)&DAT_800b8ade + iVar9) = 0xa0;
        iVar6 = 0x80;
        if (0x4a < DAT_800b522c) {
          iVar6 = (DAT_800b522c + -0x4b) * -4 + 0x80;
        }
        if (iVar6 < 0) {
          iVar6 = 0;
        }
      }
      uVar3 = iVar6 << 1;
      if (0xff < (int)uVar3) {
        uVar3 = 0xff;
      }
      uVar3 = uVar3 | 0x3e000000;
      *(uint *)((int)&DAT_800b8ab4 + iVar9) = uVar3;
      *(uint *)((int)&DAT_800b8ac0 + iVar9) = uVar3;
      *(uint *)((int)&DAT_800b8acc + iVar9) = uVar3;
      *(uint *)((int)&DAT_800b8ad8 + iVar9) = uVar3;
      local_30 = iVar4;
      AddPrim(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20,&DAT_800b8ab0 + iVar9);
      iVar4 = local_30;
    }
    iVar9 = iVar9 + 0x48;
    local_40 = local_40 + 1;
  } while (local_40 < 4);
  FUN_800160c4();
  return;
}
