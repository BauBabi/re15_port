/* FUN_800375c8 @ 0x800375c8  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_800375c8(int param_1)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  ushort uVar6;
  uint *puVar7;
  short local_38 [2];
  short local_34;
  int local_30;
  int local_2c;
  int local_28;
  
  uVar6 = DAT_800cfc6e + 0x200U & 0xfc00;
  if (DAT_800cfcfe < 0x1f) {
    local_38[0] = *(short *)(&DAT_800a2594 + param_1 * 2);
    local_34 = 0;
    FUN_80077384((int)(short)uVar6,local_38,local_38);
    local_30 = local_38[0] + DAT_800cfc30;
    local_2c = DAT_800cfc34 + -0x708;
    local_28 = local_34 + DAT_800cfc38;
    uVar2 = FUN_8004c1bc(&local_30,0,1 << (DAT_800cfcfe & 0x1f),0x8000);
    iVar3 = FUN_80037eac(&DAT_800cfbf8);
    if (((((uVar2 & 2) != 0) || (iVar3 != 0)) &&
        (sVar1 = FUN_8004fba0(&local_30,DAT_800cfc92,0x8000,0), sVar1 <= local_2c)) &&
       (((DAT_800dcbc8 & 1) == 0 && (iVar4 = FUN_800389e8(&local_30,&DAT_800cfbf8), iVar4 == 0)))) {
      DAT_800cfddc = (uint *)iVar3;
      if (iVar3 == 0) {
        DAT_800cfddc = (uint *)0x0;
      }
      DAT_800cfbfc._1_1_ = 8;
      DAT_800cfbfc._2_1_ = 0;
      DAT_800cfbfc._3_1_ = 0;
      return 1;
    }
  }
  local_38[0] = *(short *)(&DAT_800a25bc + param_1 * 2);
  local_34 = 0;
  FUN_80077384((int)(short)uVar6,local_38,local_38);
  local_30 = local_38[0] + DAT_800cfc30;
  local_28 = local_34 + DAT_800cfc38;
  local_2c = DAT_800cfc34;
  local_2c = FUN_8004fba0(&local_30,DAT_800cfc92,(uint)DAT_800cfc01 << 8,0);
  if (DAT_800dcc2c < local_2c) {
    local_2c = DAT_800dcc2c;
  }
  uVar2 = FUN_8004c1bc(&local_30,0,1 << (DAT_800cfcfe & 0x1f),(uint)DAT_800cfc01 << 8);
  if (DAT_800cfc34 < local_2c) {
    if (((uVar2 & 4) != 0) || (uVar5 = 0, DAT_800cfddc != (uint *)0x0)) {
      puVar7 = &DAT_800d0324;
      if (&DAT_800d0324 < DAT_800d4224) {
        do {
          if ((((*puVar7 & 1) != 0) && ((*puVar7 & 2) == 0)) && (puVar7 != DAT_800cfddc)) {
            iVar4 = FUN_80038950(&local_30,puVar7,DAT_800cfc92,1);
            iVar3 = local_2c;
            if (iVar4 != 0) goto LAB_80037914;
            local_2c = DAT_800cfc34;
            iVar4 = FUN_80038950(&local_30,puVar7,DAT_800cfc92,1);
            if (iVar4 != 0) goto LAB_80037914;
            local_2c = (int)(short)iVar3;
          }
          puVar7 = puVar7 + 0x7e;
        } while (puVar7 < DAT_800d4224);
      }
      iVar3 = FUN_800389e8(&local_30,&DAT_800cfbf8);
      uVar5 = 0;
      if (iVar3 == 0) {
        uVar5 = 1;
        if (DAT_800cfddc != (uint *)0x0) {
          DAT_800cfddc = (uint *)0x0;
        }
        DAT_800cfbfc._1_1_ = 8;
        DAT_800cfbfc._2_1_ = 2;
        DAT_800cfbfc._3_1_ = 0;
      }
    }
  }
  else {
LAB_80037914:
    uVar5 = 0;
  }
  return uVar5;
}


