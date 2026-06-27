/* FUN_800597a4 @ 0x800597a4  (Ghidra headless, raw MIPS overlay) */

void FUN_800597a4(void)

{
  bool bVar1;
  long lVar2;
  int iVar3;
  ulong voice_bit;
  uint uVar4;
  ulong local_58 [7];
  int local_3c;
  
  uVar4 = 0x17;
  do {
    voice_bit = 1 << (uVar4 & 0x1f);
    lVar2 = SpuGetKeyStatus(voice_bit);
    if (lVar2 != 0) {
      local_58[0] = voice_bit;
      FUN_8007975c(local_58);
      if (local_3c - 0x14441U < 169999) {
        SpuSetKey(0,voice_bit);
      }
    }
    bVar1 = uVar4 != 0;
    uVar4 = uVar4 - 1;
  } while (bVar1);
  iVar3 = 0x2e0;
  do {
    if ((*(short *)(&DAT_800d4f1c + iVar3) != 4) || ((&DAT_800d4f19)[iVar3] != '\x03')) {
      (&DAT_800d4f18)[iVar3] = 0;
      *(undefined2 *)(&DAT_800d4f2a + iVar3) = 0;
    }
    bVar1 = iVar3 != 0;
    iVar3 = iVar3 + -0x20;
  } while (bVar1);
  return;
}


