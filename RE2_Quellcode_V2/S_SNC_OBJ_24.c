/* S_SNC_OBJ_24 @ 0x8008360c  (Ghidra headless, raw MIPS overlay) */

uint S_SNC_OBJ_24(undefined4 param_1,uint param_2)

{
  *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
       *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) & 0xc0ff |
       (ushort)((param_2 & 0x3f) << 8);
  return param_2;
}


