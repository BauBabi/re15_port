/* S_SCA_OBJ_15C @ 0x8007d060  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x8007d070) */
/* WARNING: Removing unreachable block (ram,0x8007d084) */
/* WARNING: Removing unreachable block (ram,0x8007d08c) */
/* WARNING: Removing unreachable block (ram,0x8007d094) */
/* WARNING: Removing unreachable block (ram,0x8007d098) */

void S_SCA_OBJ_15C(int param_1)

{
  uint in_t1;
  int in_t2;
  
  *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x182) = *(ushort *)(param_1 + 6) & 0x7fff;
  if ((in_t2 != 0) || ((in_t1 & 0x40) != 0)) {
    *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1b0) = *(undefined2 *)(param_1 + 0x10);
  }
  if ((in_t2 != 0) || ((in_t1 & 0x80) != 0)) {
    *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1b2) = *(undefined2 *)(param_1 + 0x12);
  }
  if ((in_t2 != 0) || ((in_t1 & 0x400) != 0)) {
    *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1b4) = *(undefined2 *)(param_1 + 0x1c);
  }
  if ((in_t2 != 0) || ((in_t1 & 0x800) != 0)) {
    *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1b6) = *(undefined2 *)(param_1 + 0x1e);
  }
  if ((in_t2 != 0) || ((in_t1 & 0x100) != 0)) {
    if (*(int *)(param_1 + 0x14) == 0) {
      S_SCA_OBJ_288();
      return;
    }
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
         *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 4;
  }
  if ((in_t2 != 0) || ((in_t1 & 0x200) != 0)) {
    if (*(int *)(param_1 + 0x18) == 0) {
      S_SCA_OBJ_2DC();
      return;
    }
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
         *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 1;
  }
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


