/* FUN_8004aab8 @ 0x8004aab8  (Ghidra headless, raw MIPS overlay) */

uint FUN_8004aab8(short param_1,short param_2)

{
  ushort *puVar1;
  uint uVar2;
  ushort *puVar3;
  
  puVar1 = *(ushort **)(DAT_800ce324 + 0x38);
  uVar2 = (uint)*puVar1;
  puVar3 = puVar1 + uVar2 * 6 + 2;
  puVar1 = puVar1 + uVar2 * 6 + -3;
  while( true ) {
    puVar3 = puVar3 + -6;
    uVar2 = uVar2 - 1;
    if (((ushort)(param_1 - *puVar3) < (ushort)(puVar1[1] - *puVar3)) &&
       ((ushort)(param_2 - *puVar1) < (ushort)(puVar1[2] - *puVar1))) break;
    puVar1 = puVar1 + -6;
    if ((uVar2 & 0xffff) == 0) {
      return 0xff;
    }
  }
  return uVar2 & 0xff;
}


