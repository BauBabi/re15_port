/* FUN_80082754 @ 0x80082754  (Ghidra headless, raw MIPS overlay) */

uint FUN_80082754(uint param_1,short param_2,short param_3,undefined2 param_4,ushort param_5,
                 undefined2 param_6)

{
  byte bVar1;
  undefined2 uVar2;
  int iVar3;
  undefined1 *puVar4;
  uint uVar5;
  uint uVar6;
  byte *pbVar7;
  byte bVar8;
  short sVar9;
  int iVar10;
  byte local_140 [128];
  byte local_c0 [128];
  short local_40;
  undefined2 local_38;
  
  iVar10 = (&DAT_800ea250)[param_1 & 0xff] + ((param_1 << 0x10) >> 0x18) * 0xb0;
  local_38 = param_6;
  local_40 = param_3;
  iVar3 = _SsVmVSetUp((int)param_2,(int)param_3);
  uVar5 = 0xffffffff;
  if (iVar3 == 0) {
    sVar9 = (short)param_1;
    DAT_800dcc32 = (undefined1)param_4;
    DAT_800dcc33 = 0;
    if ((int)(param_1 << 0x10) >> 0x10 == 0x21) {
      DAT_800dcc34 = (undefined1)param_5;
    }
    else {
      DAT_800dcc34 = (undefined1)
                     ((int)((uint)param_5 *
                           (int)*(short *)((uint)*(byte *)(iVar10 + 0x17) * 2 + iVar10 + 0x60)) /
                     0x7f);
    }
    DAT_800dcc35 = (undefined1)local_38;
    puVar4 = (undefined1 *)(local_40 * 0x10 + DAT_800d7838);
    DAT_800dcc3a = puVar4[1];
    DAT_800dcc3b = puVar4[4];
    DAT_800dcc30 = *puVar4;
    uVar5 = 0xffffffff;
    DAT_800dcc46 = sVar9;
    if ((ushort)DAT_800dcc37 < *(ushort *)(DAT_800d7848 + 0x12)) {
      if (param_5 == 0) {
        uVar5 = FUN_80082cd8((int)sVar9,(int)param_2,(int)local_40,param_4);
      }
      else {
        bVar1 = _SsVmSelectToneAndVag(local_c0,local_140);
        bVar8 = 0;
        uVar5 = 0;
        if (bVar1 != 0) {
          uVar6 = 0;
          do {
            DAT_800dcc48 = (ushort)local_140[uVar6];
            DAT_800dcc3c = local_c0[uVar6];
            pbVar7 = (byte *)(((uint)DAT_800dcc3c + (uint)DAT_800dcc37 * 0x10) * 0x20 + DAT_800d784c
                             );
            DAT_800dcc3f = *pbVar7;
            DAT_800dcc3d = pbVar7[2];
            DAT_800dcc3e = pbVar7[3];
            DAT_800dcc40 = pbVar7[4];
            DAT_800dcc41 = pbVar7[5];
            DAT_800dcc44 = pbVar7[1];
            DAT_800dcc42 = pbVar7[6];
            DAT_800dcc43 = pbVar7[7];
            uVar6 = FUN_80081774(0);
            uVar6 = uVar6 & 0xff;
            DAT_800dcc4a = (short)uVar6;
            if (uVar6 < DAT_800d7854) {
              (&DAT_800cbced)[uVar6 * 0x36] = 1;
              (&DAT_800cbcd2)[DAT_800dcc4a * 0x1b] = 0;
              (&DAT_800cbce0)[DAT_800dcc4a * 0x1b] = sVar9;
              (&DAT_800cbce8)[DAT_800dcc4a * 0x1b] = (ushort)DAT_800dcc31;
              (&DAT_800cbce2)[DAT_800dcc4a * 0x1b] = (ushort)DAT_800dcc37;
              (&DAT_800cbce4)[DAT_800dcc4a * 0x1b] = local_40;
              if (sVar9 != 0x21) {
                (&DAT_800cbcd8)[DAT_800dcc4a * 0x1b] = param_5;
                (&DAT_800cbcdc)[DAT_800dcc4a * 0x1b] = (ushort)*(byte *)(iVar10 + 0x17);
              }
              (&DAT_800cbcda)[DAT_800dcc4a * 0x36] = (undefined1)local_38;
              (&DAT_800cbce6)[DAT_800dcc4a * 0x1b] = (ushort)DAT_800dcc3c;
              (&DAT_800cbcde)[DAT_800dcc4a * 0x1b] = param_4;
              (&DAT_800cbcea)[DAT_800dcc4a * 0x1b] = (ushort)DAT_800dcc3f;
              (&DAT_800cbcd0)[DAT_800dcc4a * 0x1b] = DAT_800dcc48;
              FUN_80081cec();
              if (DAT_800dcc48 == 0xff) {
                FUN_8008310c((undefined1)DAT_800dcc4a);
              }
              else {
                uVar2 = note2pitch();
                FUN_80083760(bVar1,uVar2);
              }
              uVar5 = uVar5 | 1 << ((int)DAT_800dcc4a & 0x1fU);
            }
            else {
              uVar5 = 0xffffffff;
            }
            bVar8 = bVar8 + 1;
            uVar6 = (uint)bVar8;
          } while (bVar8 < bVar1);
        }
      }
    }
  }
  return uVar5;
}


