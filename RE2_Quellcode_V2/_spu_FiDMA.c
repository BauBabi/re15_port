/* _spu_FiDMA @ 0x80078ac0  (Ghidra headless, raw MIPS overlay) */

void _spu_FiDMA(void)

{
  undefined *puVar1;
  uint uVar2;
  
  if (DAT_800ab26c == 0) {
    _spu_Fw1ts();
  }
  puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
       *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) & 0xffcf;
  if ((*(ushort *)(puVar1 + 0x1aa) & 0x30) != 0) {
    uVar2 = 1;
    do {
      if (0xf00 < uVar2) break;
      uVar2 = uVar2 + 1;
    } while ((*(ushort *)(puVar1 + 0x1aa) & 0x30) != 0);
  }
  if (DAT_800ab254 == (code *)0x0) {
    DeliverEvent(0xf0000009,0x20);
    return;
  }
  (*DAT_800ab254)();
  SPU_OBJ_510();
  return;
}


