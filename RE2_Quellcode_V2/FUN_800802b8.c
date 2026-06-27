/* FUN_800802b8 @ 0x800802b8  (Ghidra headless, raw MIPS overlay) */

int FUN_800802b8(ushort param_1,short param_2,ushort param_3,byte param_4,undefined2 param_5,
                undefined2 param_6,short param_7,short param_8)

{
  byte bVar1;
  undefined2 uVar2;
  int iVar3;
  undefined1 *puVar4;
  int iVar5;
  
  if (DAT_800d7658 != 1) {
    DAT_800d7658 = 1;
    if ((param_1 < 0x18) && (iVar3 = _SsVmVSetUp((int)param_2,(int)(short)param_3), iVar3 == 0)) {
      DAT_800dcc46 = 0x21;
      iVar3 = (int)param_7;
      iVar5 = (int)param_8;
      DAT_800dcc32 = (undefined1)param_5;
      DAT_800dcc33 = (undefined1)param_6;
      DAT_800dcc34 = (undefined1)param_7;
      if (iVar3 == iVar5) {
        DAT_800dcc35 = '@';
      }
      else if (iVar5 < iVar3) {
        DAT_800dcc35 = (char)((iVar5 << 6) / iVar3);
        if (iVar3 == 0) {
          trap(0x1c00);
        }
        if ((iVar3 == -1) && (iVar5 << 6 == -0x80000000)) {
          trap(0x1800);
        }
      }
      else {
        if (iVar5 == 0) {
          trap(0x1c00);
        }
        if ((iVar5 == -1) && (iVar3 << 6 == -0x80000000)) {
          trap(0x1800);
        }
        DAT_800dcc34 = (undefined1)param_8;
        DAT_800dcc35 = '\x7f' - (char)((iVar3 << 6) / iVar5);
      }
      puVar4 = (undefined1 *)(((int)((uint)param_3 << 0x10) >> 0xc) + DAT_800d7838);
      DAT_800dcc3a = puVar4[1];
      DAT_800dcc3b = puVar4[4];
      DAT_800dcc30 = *puVar4;
      puVar4 = (undefined1 *)(((uint)param_4 + (uint)DAT_800dcc37 * 0x10) * 0x20 + DAT_800d784c);
      DAT_800dcc3f = *puVar4;
      DAT_800dcc48 = *(short *)(puVar4 + 0x16);
      DAT_800dcc3d = puVar4[2];
      DAT_800dcc3e = puVar4[3];
      DAT_800dcc40 = puVar4[4];
      DAT_800dcc41 = puVar4[5];
      DAT_800dcc44 = puVar4[1];
      DAT_800dcc42 = puVar4[6];
      DAT_800dcc43 = puVar4[7];
      DAT_800dcc3c = param_4;
      if (DAT_800dcc48 != 0) {
        iVar3 = (int)(short)param_1;
        DAT_800dcc4a = param_1;
        (&DAT_800cbce0)[iVar3 * 0x1b] = 0x21;
        (&DAT_800cbce8)[iVar3 * 0x1b] = param_2;
        bVar1 = DAT_800dcc37;
        (&DAT_800cbce4)[iVar3 * 0x1b] = param_3;
        (&DAT_800cbce2)[iVar3 * 0x1b] = (ushort)bVar1;
        (&DAT_800cbcd0)[iVar3 * 0x1b] = DAT_800dcc48;
        bVar1 = DAT_800dcc3c;
        (&DAT_800cbcde)[iVar3 * 0x1b] = param_5;
        (&DAT_800cbced)[iVar3 * 0x36] = 1;
        (&DAT_800cbcd2)[iVar3 * 0x1b] = 0;
        (&DAT_800cbce6)[iVar3 * 0x1b] = (ushort)bVar1;
        FUN_80081cec();
        if (DAT_800dcc48 == 0xff) {
          FUN_8008310c(param_1 & 0xff);
        }
        else {
          uVar2 = note2pitch2(param_5,param_6);
          FUN_80083760(1,uVar2);
        }
        DAT_800d7658 = 0;
        return (int)(short)param_1;
      }
    }
    DAT_800d7658 = 0;
  }
  return -1;
}


