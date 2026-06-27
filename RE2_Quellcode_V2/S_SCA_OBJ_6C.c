/* S_SCA_OBJ_6C @ 0x8007cf70  (Ghidra headless, raw MIPS overlay) */

void S_SCA_OBJ_6C(int param_1)

{
  ushort uVar1;
  ushort in_t0;
  uint in_t1;
  int in_t2;
  
  uVar1 = *(ushort *)(param_1 + 4);
  if (0x7f < (short)uVar1) {
    S_SCA_OBJ_CC(param_1,0xa000,0x7f);
    return;
  }
  if ((short)uVar1 < 0) {
    uVar1 = 0;
  }
  *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x180) = uVar1 & 0x7fff | 0xa000;
  if (in_t2 == 0) {
    if ((in_t1 & 2) == 0) goto S_SCA_OBJ_1A8;
    if ((in_t1 & 8) != 0) goto S_SCA_OBJ_F8;
S_SCA_OBJ_15C:
    in_t0 = *(ushort *)(param_1 + 6);
    uVar1 = 0;
  }
  else {
S_SCA_OBJ_F8:
    switch(*(undefined2 *)(param_1 + 10)) {
    default:
      goto S_SCA_OBJ_15C;
    case 1:
      uVar1 = 0x8000;
      break;
    case 2:
      uVar1 = 0x9000;
      break;
    case 3:
      uVar1 = 0xa000;
      break;
    case 4:
      uVar1 = 0xb000;
      break;
    case 5:
      uVar1 = 0xc000;
      break;
    case 6:
      uVar1 = 0xd000;
      break;
    case 7:
      uVar1 = 0xe000;
    }
  }
  if (uVar1 != 0) {
    in_t0 = *(ushort *)(param_1 + 6);
    if (0x7f < (short)in_t0) {
      S_SCA_OBJ_194();
      return;
    }
    if ((short)in_t0 < 0) {
      in_t0 = 0;
    }
  }
  *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x182) = in_t0 & 0x7fff | uVar1;
S_SCA_OBJ_1A8:
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


