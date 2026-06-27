void FUN_80020894(int param_1,int param_2,int param_3)

{
  short sVar1;
  short sVar2;
  uint *puVar3;
  
  if (param_2 == 0 && param_3 == 0) {
    return;
  }
  if (0 < param_2) {
    return;
  }
  puVar3 = (uint *)(&DAT_800b5584 + (uint)DAT_800aca34 * 0x1680 + param_1 * 0x18);
  sVar1 = (ushort)DAT_800aca34 * 0xf0;
  sVar2 = (short)param_1 + sVar1;
  *(short *)(puVar3 + 3) = -(short)param_2;
  *(short *)(puVar3 + 5) = (short)param_2 + 0x140;
  *(short *)((int)puVar3 + 0x12) = sVar2;
  if (param_1 + param_3 < 0xf0) {
    sVar2 = sVar2 + (short)param_3;
    if (param_1 + param_3 < 0) {
      *(short *)((int)puVar3 + 0xe) = sVar1;
      goto LAB_80020930;
    }
  }
  else {
    sVar2 = sVar1 + 0xef;
  }
  *(short *)((int)puVar3 + 0xe) = sVar2;
LAB_80020930:
  if (param_3 < 0) {
                    /* Probable PsyQ macro: addPrim(). */
    *puVar3 = *puVar3 & 0xff000000 | *DAT_800b5580 & 0xffffff;
    *DAT_800b5580 = *DAT_800b5580 & 0xff000000 | (uint)puVar3 & 0xffffff;
  }
  else {
    *DAT_800b557c = *DAT_800b557c & 0xff000000 | (uint)puVar3 & 0xffffff;
    DAT_800b557c = puVar3;
  }
  return;
}
