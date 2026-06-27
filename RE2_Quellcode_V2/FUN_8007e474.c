/* FUN_8007e474 @ 0x8007e474  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8007e474(short param_1,short param_2)

{
  byte bVar1;
  int iVar2;
  code *pcVar3;
  byte bVar4;
  byte *pbVar5;
  byte *pbVar6;
  int *piVar7;
  
  iVar2 = param_2 * 0xb0;
  piVar7 = (int *)((&DAT_800ea250)[param_1] + iVar2);
  pbVar5 = (byte *)*piVar7;
  pbVar6 = pbVar5 + 1;
  *piVar7 = (int)pbVar6;
  bVar4 = *pbVar5;
  if (((*(uint *)(iVar2 + (&DAT_800ea250)[param_1] + 0x98) & 0x401) == 0x401) &&
     (pbVar6 == (byte *)(piVar7[4] + 1))) {
    FUN_8007e230((int)param_1,(int)param_2,*(undefined1 *)(piVar7[4] + 1));
    return 0xffffffff;
  }
  if ((bVar4 & 0x80) == 0) {
    bVar1 = *(byte *)((int)piVar7 + 0x16);
    pcVar3 = DAT_800cbc3c;
    if (bVar1 == 0xc0) goto LAB_8007e7e4;
    if (bVar1 < 0xc1) {
      if (bVar1 == 0x90) {
        pbVar5 = (byte *)*piVar7;
        *piVar7 = (int)(pbVar5 + 1);
        bVar1 = *pbVar5;
        iVar2 = _SsReadDeltaValue((int)param_1,(int)param_2);
        piVar7[0x24] = iVar2;
        goto code_r0x8007e728;
      }
      pcVar3 = DAT_800cbc48;
      if (bVar1 != 0xb0) {
        return 0;
      }
      goto LAB_8007e7e4;
    }
    if (bVar1 == 0xe0) goto LAB_8007e780;
    pcVar3 = DAT_800cbc44;
    if (bVar1 != 0xff) {
      return 0;
    }
  }
  else {
    *(byte *)((int)piVar7 + 0x17) = bVar4 & 0xf;
    bVar4 = bVar4 & 0xf0;
    if (bVar4 == 0xc0) {
      pbVar5 = (byte *)*piVar7;
      *(undefined1 *)((int)piVar7 + 0x16) = 0xc0;
      *piVar7 = (int)(pbVar5 + 1);
      bVar4 = *pbVar5;
      pcVar3 = DAT_800cbc3c;
      goto LAB_8007e7e4;
    }
    if (bVar4 < 0xc1) {
      if (bVar4 == 0x90) {
        pbVar5 = (byte *)*piVar7;
        *(undefined1 *)((int)piVar7 + 0x16) = 0x90;
        *piVar7 = (int)(pbVar5 + 1);
        bVar4 = *pbVar5;
        *piVar7 = (int)(pbVar5 + 2);
        bVar1 = pbVar5[1];
        iVar2 = _SsReadDeltaValue((int)param_1,(int)param_2);
        piVar7[0x24] = iVar2;
code_r0x8007e728:
        (*DAT_800cbc38)((int)param_1,(int)param_2,bVar4,bVar1);
        return 0;
      }
      if (bVar4 != 0xb0) {
        return 0;
      }
      pbVar5 = (byte *)*piVar7;
      *(undefined1 *)((int)piVar7 + 0x16) = 0xb0;
      *piVar7 = (int)(pbVar5 + 1);
      bVar4 = *pbVar5;
      pcVar3 = DAT_800cbc48;
      goto LAB_8007e7e4;
    }
    if (bVar4 == 0xe0) {
      *(undefined1 *)((int)piVar7 + 0x16) = 0xe0;
      *piVar7 = *piVar7 + 1;
LAB_8007e780:
      (*DAT_800cbc40)((int)param_1,(int)param_2);
      return 0;
    }
    if (bVar4 != 0xf0) {
      return 0;
    }
    pbVar5 = (byte *)*piVar7;
    *(undefined1 *)((int)piVar7 + 0x16) = 0xff;
    *piVar7 = (int)(pbVar5 + 1);
    bVar4 = *pbVar5;
    pcVar3 = DAT_800cbc44;
  }
  DAT_800cbc44 = pcVar3;
  if (bVar4 == 0x2f) {
    FUN_8007e230((int)param_1,(int)param_2,0x2f);
    return 1;
  }
LAB_8007e7e4:
  (*pcVar3)((int)param_1,(int)param_2,bVar4);
  return 0;
}


