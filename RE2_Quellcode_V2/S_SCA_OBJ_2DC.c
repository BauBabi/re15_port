/* S_SCA_OBJ_2DC @ 0x8007d1e0  (Ghidra headless, raw MIPS overlay) */

void S_SCA_OBJ_2DC(int param_1)

{
  int in_v0;
  undefined2 in_v1;
  uint in_t1;
  int in_t2;
  
  *(undefined2 *)(in_v0 + 0x1aa) = in_v1;
  if ((in_t2 != 0) || ((in_t1 & 0x1000) != 0)) {
    if (*(int *)(param_1 + 0x20) == 0) {
      S_SCA_OBJ_330();
      return;
    }
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
         *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 8;
  }
  if ((in_t2 != 0) || ((in_t1 & 0x2000) != 0)) {
    if (*(int *)(param_1 + 0x24) == 0) {
      S_SCA_OBJ_384();
      return;
    }
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
         *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 2;
  }
  return;
}


