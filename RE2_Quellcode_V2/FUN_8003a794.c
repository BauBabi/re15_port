/* FUN_8003a794 @ 0x8003a794  (Ghidra headless, raw MIPS overlay) */

void FUN_8003a794(int param_1)

{
  byte *pbVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 *p;
  undefined4 *puVar4;
  undefined4 *puVar5;
  
  p = (undefined4 *)(DAT_800ea23c + (uint)DAT_800ce5e0 * 0x1c70 + 0x1a80);
  puVar3 = (undefined4 *)(DAT_800ea23c + 0x300);
  puVar5 = puVar3 + param_1 * 4;
  pbVar1 = (byte *)(DAT_800ea23c + 0x30c);
  puVar2 = p + 3;
  do {
    puVar4 = p;
    if (pbVar1[3] != 0) {
      *p = 0x4000000;
      puVar2[-2] = (uint)pbVar1[1] * 0x1000000 + (uint)pbVar1[2] * 0x10101 + 0x64000000;
      puVar2[-1] = *puVar3;
      puVar2[1] = *(undefined4 *)(pbVar1 + -8);
      puVar4 = p + 5;
      *puVar2 = *(undefined4 *)(pbVar1 + -4);
      puVar2 = puVar2 + 5;
      AddPrim(&DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000 + (uint)*pbVar1 * 4,p);
    }
    puVar3 = puVar3 + 4;
    pbVar1 = pbVar1 + 0x10;
    p = puVar4;
  } while (puVar3 < puVar5);
  FUN_8003a92c();
  return;
}


