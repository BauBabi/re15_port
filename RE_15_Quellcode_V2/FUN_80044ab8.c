void FUN_80044ab8(void)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  ushort uVar5;
  short local_38;
  short local_36 [19];
  
  if (DAT_800b5218 == 1) {
    sVar1 = 0x10;
    do {
      sVar1 = sVar1 + -1;
      SsUtGetDetVVol(sVar1,&local_38,local_36);
      iVar4 = (int)local_38;
      uVar2 = (uint)DAT_800b5360;
      if (uVar2 == 0) {
        trap(0x1c00);
      }
      if ((uVar2 == 0xffffffff) && (iVar4 == -0x80000000)) {
        trap(0x1800);
      }
      iVar3 = (int)local_36[0];
      if (uVar2 == 0) {
        trap(0x1c00);
      }
      if ((uVar2 == 0xffffffff) && (iVar3 == -0x80000000)) {
        trap(0x1800);
      }
      iVar4 = iVar4 - iVar4 / (int)uVar2;
      iVar3 = iVar3 - iVar3 / (int)uVar2;
      local_38 = (short)iVar4;
      local_36[0] = (short)iVar3;
      if (iVar4 * 0x10000 < 0) {
        local_38 = 0;
      }
      if (iVar3 * 0x10000 < 0) {
        local_36[0] = 0;
      }
      SsUtSetDetVVol(sVar1,local_38,local_36[0]);
    } while (sVar1 != 0);
    DAT_800b5218 = 2;
    DAT_800b5360 = DAT_800b5360 - 1;
  }
  else if (1 < DAT_800b5218) {
    if (DAT_800b5218 == 2) {
      sVar1 = 0x10;
      do {
        sVar1 = sVar1 + -1;
        SsUtGetDetVVol(sVar1,&local_38,local_36);
        iVar4 = (int)local_38;
        uVar2 = (uint)DAT_800b5360;
        if (uVar2 == 0) {
          trap(0x1c00);
        }
        if ((uVar2 == 0xffffffff) && (iVar4 == -0x80000000)) {
          trap(0x1800);
        }
        iVar3 = (int)local_36[0];
        if (uVar2 == 0) {
          trap(0x1c00);
        }
        if ((uVar2 == 0xffffffff) && (iVar3 == -0x80000000)) {
          trap(0x1800);
        }
        iVar4 = iVar4 - iVar4 / (int)uVar2;
        iVar3 = iVar3 - iVar3 / (int)uVar2;
        local_38 = (short)iVar4;
        local_36[0] = (short)iVar3;
        if (iVar4 * 0x10000 < 0) {
          local_38 = 0;
        }
        if (iVar3 * 0x10000 < 0) {
          local_36[0] = 0;
        }
        SsUtSetDetVVol(sVar1,local_38,local_36[0]);
      } while (sVar1 != 0);
      DAT_800b5360 = DAT_800b5360 - 1;
      if (DAT_800b5360 == 1) {
        DAT_800b5218 = 3;
      }
    }
    else {
      uVar5 = 0;
      if (DAT_800b5218 == 3) {
        DAT_800b5360 = 0;
        DAT_800b5218 = 0;
        uVar2 = 0;
        do {
          iVar4 = uVar2 * 8;
          uVar5 = uVar5 + 1;
          if ((&DAT_800b52ac)[iVar4] != '\0') {
            SsSeqStop((short)(char)(&DAT_800b52ae)[iVar4]);
            (&DAT_800b52ac)[iVar4] = 0;
          }
          uVar2 = (uint)uVar5;
        } while (uVar5 < 3);
      }
    }
  }
  return;
}
