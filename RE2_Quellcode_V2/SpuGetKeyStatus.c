/* SpuGetKeyStatus @ 0x80079694  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x800796dc) */
/* WARNING: Removing unreachable block (ram,0x80079718) */
/* WARNING: Removing unreachable block (ram,0x80079708) */
/* WARNING: Removing unreachable block (ram,0x80079710) */
/* WARNING: Removing unreachable block (ram,0x80079720) */

long SpuGetKeyStatus(ulong voice_bit)

{
  uint uVar1;
  long lVar2;
  uint uVar3;
  
  uVar3 = 0;
  uVar1 = 1;
  do {
    if ((voice_bit & uVar1) != 0) {
      lVar2 = S_GKS_OBJ_2C(voice_bit,uVar3);
      return lVar2;
    }
    uVar3 = uVar3 + 1;
    uVar1 = 1 << (uVar3 & 0x1f);
  } while ((int)uVar3 < 0x18);
  lVar2 = S_GKS_OBJ_8C();
  return lVar2;
}


