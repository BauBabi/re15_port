/* FUN_800396fc @ 0x800396fc  (Ghidra headless, raw MIPS overlay) */

byte FUN_800396fc(char *param_1)

{
  short sVar1;
  uint uVar2;
  byte *pbVar3;
  uint uVar4;
  byte bVar5;
  
  bVar5 = 0;
  uVar4 = 0;
  pbVar3 = (byte *)(param_1 + 1);
  do {
    if (*param_1 != '\0') {
      if (*(short *)(pbVar3 + 1) == 0) {
        sVar1 = *(short *)(pbVar3 + 3);
        *(short *)(pbVar3 + 3) = sVar1 + -1;
        if ((short)(sVar1 + -1) == 0) {
          *param_1 = '\0';
        }
        if (*(short *)(pbVar3 + 5) != 0) {
          uVar2 = (uint)*(ushort *)(pbVar3 + 7) + (int)*(short *)(pbVar3 + 5);
          *(short *)(pbVar3 + 7) = (short)uVar2;
          *pbVar3 = (byte)(uVar2 >> 7);
        }
        if (bVar5 < *pbVar3) {
          bVar5 = *pbVar3;
        }
      }
      else {
        *(short *)(pbVar3 + 1) = *(short *)(pbVar3 + 1) + -1;
      }
    }
    uVar4 = uVar4 + 1;
    pbVar3 = pbVar3 + 10;
    param_1 = param_1 + 10;
  } while (uVar4 < 0x20);
  return bVar5;
}


