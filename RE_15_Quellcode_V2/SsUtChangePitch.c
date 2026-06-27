short SsUtChangePitch(short param_1,short param_2,short param_3,short param_4,short param_5,
                     short param_6,short param_7)

{
  undefined2 uVar1;
  short sVar2;
  int iVar3;
  
  if ((ushort)param_1 < 0x18) {
    iVar3 = (int)param_1;
    if ((((&DAT_8008f85a)[iVar3 * 0x1a] == param_2) && ((&DAT_8008f856)[iVar3 * 0x1a] == param_3))
       && ((&DAT_8008f850)[iVar3 * 0x1a] == param_4)) {
      SpuVmVSetUp();
      DAT_800b532a = 0x21;
      DAT_800b5320 = *(undefined1 *)(&DAT_8008f858 + iVar3 * 0x1a);
      DAT_800b532e = param_1;
      uVar1 = note2pitch2(param_6,param_7);
      (&DAT_8008f6b0)[iVar3 * 8] = uVar1;
      (&DAT_8008f82c)[iVar3] = (&DAT_8008f82c)[iVar3] | 4;
      sVar2 = VMANAGER_OBJ_48F4();
      return sVar2;
    }
  }
  return -1;
}
