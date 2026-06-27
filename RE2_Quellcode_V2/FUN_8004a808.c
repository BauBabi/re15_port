/* FUN_8004a808 @ 0x8004a808  (Ghidra headless, raw MIPS overlay) */

char FUN_8004a808(int param_1,short *param_2,uint param_3,char param_4)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  short sVar6;
  short sVar7;
  
  if ((*(byte *)(param_1 + 0x150) & 0x7f) == 0) {
    *(byte *)(param_1 + 0x150) = *(byte *)(param_1 + 0x150) | 7;
  }
  bVar1 = *(char *)(param_1 + 0x150) - 1;
  *(byte *)(param_1 + 0x150) = bVar1;
  if ((bVar1 & 0x80) == 0) {
    *(ushort *)(param_1 + 0x154) = *(ushort *)(param_1 + 0x154) & 0xf7ff;
    iVar3 = FUN_80050858(*(int *)(param_1 + 0x198) + (uint)*(byte *)(param_1 + 0x1c1) * 0xac + 0x5c,
                         DAT_800cfd90 + (uint)DAT_800cfdb9 * 0xac + 0x5c,0x2000,1);
    if (iVar3 == 0) {
      *(ushort *)(param_1 + 0x154) = *(ushort *)(param_1 + 0x154) | 0x800;
    }
  }
  if ((*(ushort *)(param_1 + 0x10e) & 0x1000) == 0) {
    cVar2 = '\x02';
    if (((*(ushort *)(param_1 + 0x10e) & 0x4000) == 0) &&
       (cVar2 = '\x02', (*(byte *)(param_1 + 0x150) & 0x80) == 0)) {
      uVar4 = FUN_8004aab8((int)*(short *)(param_1 + 0x38),(int)*(short *)(param_1 + 0x40));
      sVar6 = (short)*(undefined4 *)param_2;
      sVar7 = (short)*(undefined4 *)(param_2 + 4);
      uVar4 = uVar4 & 0xff;
      uVar5 = FUN_8004aab8((int)*param_2,(int)param_2[4]);
      uVar5 = uVar5 & 0xff;
      if (((uVar4 != 0xff) && (uVar5 != 0xff)) || (cVar2 = param_4, param_4 != '\0')) {
        if (param_4 != '\0') {
          uVar5 = param_3 & 0xff;
          iVar3 = *(int *)(DAT_800ce324 + 0x38) + uVar5 * 0xc;
          sVar6 = (short)((int)*(short *)(iVar3 + 4) + (int)*(short *)(iVar3 + 8) >> 1);
          sVar7 = (short)((int)*(short *)(iVar3 + 6) + (int)*(short *)(iVar3 + 10) >> 1);
        }
        cVar2 = '\x01';
        if (uVar4 == uVar5) {
          *(short *)(param_1 + 0x1c4) = sVar6;
          *(short *)(param_1 + 0x1c6) = sVar7;
        }
        else {
          cVar2 = '\x02';
          if (*(char *)(DAT_800ce330 + 0x150) == '\0') {
            FUN_8004ab60(uVar4,uVar5,(int)sVar6,(int)sVar7);
            cVar2 = '\x01';
          }
        }
      }
    }
  }
  else {
    *(short *)(param_1 + 0x1c4) = (short)*(undefined4 *)param_2;
    *(short *)(param_1 + 0x1c6) = (short)*(undefined4 *)(param_2 + 4);
    cVar2 = '\x02';
  }
  return cVar2;
}


