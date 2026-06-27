/* FUN_8007fdc8 @ 0x8007fdc8  (Ghidra headless, raw MIPS overlay) */

uint FUN_8007fdc8(short param_1,ushort param_2,byte param_3,undefined2 param_4,undefined2 param_5,
                 short param_6,short param_7)

{
  byte bVar1;
  undefined2 uVar2;
  int iVar3;
  undefined1 *puVar4;
  uint uVar5;
  int iVar6;
  
  if (DAT_800d7658 != 1) {
    DAT_800d7658 = 1;
    iVar3 = _SsVmVSetUp((int)param_1,(int)(short)param_2);
    if (iVar3 == 0) {
      DAT_800dcc46 = 0x21;
      iVar3 = (int)param_6;
      iVar6 = (int)param_7;
      DAT_800dcc32 = (undefined1)param_4;
      DAT_800dcc33 = (undefined1)param_5;
      DAT_800dcc34 = (undefined1)param_6;
      if (iVar3 == iVar6) {
        DAT_800dcc35 = '@';
      }
      else if (iVar6 < iVar3) {
        DAT_800dcc35 = (char)((iVar6 << 6) / iVar3);
        if (iVar3 == 0) {
          trap(0x1c00);
        }
        if ((iVar3 == -1) && (iVar6 << 6 == -0x80000000)) {
          trap(0x1800);
        }
      }
      else {
        if (iVar6 == 0) {
          trap(0x1c00);
        }
        if ((iVar6 == -1) && (iVar3 << 6 == -0x80000000)) {
          trap(0x1800);
        }
        DAT_800dcc34 = (undefined1)param_7;
        DAT_800dcc35 = '\x7f' - (char)((iVar3 << 6) / iVar6);
      }
      puVar4 = (undefined1 *)(((int)((uint)param_2 << 0x10) >> 0xc) + DAT_800d7838);
      DAT_800dcc3a = puVar4[1];
      DAT_800dcc3b = puVar4[4];
      DAT_800dcc30 = *puVar4;
      puVar4 = (undefined1 *)(((uint)param_3 + (uint)DAT_800dcc37 * 0x10) * 0x20 + DAT_800d784c);
      DAT_800dcc3f = *puVar4;
      DAT_800dcc48 = *(short *)(puVar4 + 0x16);
      DAT_800dcc3d = puVar4[2];
      DAT_800dcc3e = puVar4[3];
      DAT_800dcc40 = puVar4[4];
      DAT_800dcc41 = puVar4[5];
      DAT_800dcc44 = puVar4[1];
      DAT_800dcc42 = puVar4[6];
      DAT_800dcc43 = puVar4[7];
      DAT_800dcc3c = param_3;
      if (DAT_800dcc48 != 0) {
        uVar5 = FUN_80081774();
        uVar5 = uVar5 & 0xff;
        if (uVar5 != DAT_800d7854) {
          DAT_800dcc4a = (undefined2)uVar5;
          (&DAT_800cbce0)[uVar5 * 0x1b] = 0x21;
          (&DAT_800cbce8)[uVar5 * 0x1b] = param_1;
          bVar1 = DAT_800dcc37;
          (&DAT_800cbce4)[uVar5 * 0x1b] = param_2;
          (&DAT_800cbce2)[uVar5 * 0x1b] = (ushort)bVar1;
          (&DAT_800cbcd0)[uVar5 * 0x1b] = DAT_800dcc48;
          bVar1 = DAT_800dcc3c;
          (&DAT_800cbcde)[uVar5 * 0x1b] = param_4;
          (&DAT_800cbced)[uVar5 * 0x36] = 1;
          (&DAT_800cbcd2)[uVar5 * 0x1b] = 0;
          (&DAT_800cbce6)[uVar5 * 0x1b] = (ushort)bVar1;
          FUN_80081cec();
          if (DAT_800dcc48 == 0xff) {
            FUN_8008310c(uVar5);
          }
          else {
            uVar2 = note2pitch2(param_4,param_5);
            FUN_80083760(1,uVar2);
          }
          DAT_800d7658 = 0;
          return uVar5;
        }
      }
    }
    DAT_800d7658 = 0;
  }
  return 0xffffffff;
}


