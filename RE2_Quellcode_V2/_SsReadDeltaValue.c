/* _SsReadDeltaValue @ 0x8007c638  (Ghidra headless, raw MIPS overlay) */

void _SsReadDeltaValue(int param_1,short param_2)

{
  byte bVar1;
  int iVar2;
  byte *pbVar3;
  uint uVar4;
  undefined4 *puVar5;
  
  puVar5 = (undefined4 *)
           (*(int *)((int)&DAT_800ea250 + ((param_1 << 0x10) >> 0xe)) + param_2 * 0xb0);
  pbVar3 = (byte *)*puVar5;
  *puVar5 = pbVar3 + 1;
  bVar1 = *pbVar3;
  uVar4 = (uint)bVar1;
  if (uVar4 == 0) {
    MIDITIME_OBJ_A8();
    return;
  }
  iVar2 = uVar4 << 2;
  if ((bVar1 & 0x80) != 0) {
    uVar4 = uVar4 & 0x7f;
    do {
      pbVar3 = (byte *)*puVar5;
      *puVar5 = pbVar3 + 1;
      bVar1 = *pbVar3;
      uVar4 = uVar4 * 0x80 + (bVar1 & 0x7f);
    } while ((bVar1 & 0x80) != 0);
    iVar2 = uVar4 * 4;
  }
  puVar5[0x22] = puVar5[0x22] + (iVar2 + uVar4) * 2;
  return;
}


