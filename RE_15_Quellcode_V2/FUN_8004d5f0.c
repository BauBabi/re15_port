void FUN_8004d5f0(int param_1,int param_2)

{
  uint uVar1;
  byte *pbVar2;
  uint uVar3;
  undefined4 *puVar4;
  short sVar5;
  
  uVar1 = DAT_800b2360;
  sVar5 = 0;
  uVar3 = 0;
  if (DAT_800b2360 != 0) {
    puVar4 = &DAT_800b2368;
    do {
      pbVar2 = (byte *)*puVar4;
      puVar4 = puVar4 + 1;
      if (pbVar2 != (byte *)0x0) {
        _DAT_800bbe58 = (int)DAT_800b0fd2;
        uVar3 = uVar3 + 1;
        if (((uint)(param_1 - *(short *)(pbVar2 + 2)) <= (uint)*(ushort *)(pbVar2 + 6)) &&
           ((uint)(param_2 - *(short *)(pbVar2 + 4)) <= (uint)*(ushort *)(pbVar2 + 8))) {
          DAT_800b0fd2 = sVar5;
          (*(code *)(&PTR_LAB_80074c68)[*pbVar2])(pbVar2 + 10);
        }
      }
      sVar5 = sVar5 + 1;
    } while (uVar3 < uVar1);
  }
  return;
}
