/* SpuSetReverb @ 0x80081218  (Ghidra headless, raw MIPS overlay) */

long SpuSetReverb(long on_off)

{
  long lVar1;
  int iVar2;
  
  if (on_off == 0) {
    DAT_800ab28c = 0;
    lVar1 = S_SR_OBJ_B0();
    return lVar1;
  }
  if (on_off != 1) {
    lVar1 = S_SR_OBJ_B4();
    return lVar1;
  }
  if ((DAT_800ab290 != 1) && (iVar2 = _SpuIsInAllocateArea_(DAT_800ab294), iVar2 != 0)) {
    DAT_800ab28c = 0;
    lVar1 = S_SR_OBJ_B0();
    return lVar1;
  }
  DAT_800ab28c = on_off;
  *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
       *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 0x80;
  return DAT_800ab28c;
}


