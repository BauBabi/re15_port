/* _SsSetControlChange @ 0x8007c0ec  (Ghidra headless, raw MIPS overlay) */

void _SsSetControlChange(short param_1,short param_2,uchar param_3)

{
  undefined1 uVar1;
  undefined1 *puVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  
  uVar3 = (uint)(ushort)param_1;
  iVar5 = (int)param_1;
  iVar4 = (int)param_2;
  puVar6 = (undefined4 *)((&DAT_800ea250)[iVar5] + iVar4 * 0xb0);
  puVar2 = (undefined1 *)*puVar6;
  *puVar6 = puVar2 + 1;
  uVar1 = *puVar2;
  if (param_3 == '@') {
    MIDICC_OBJ_1D8(iVar5,iVar4);
    return;
  }
  if (0x40 < param_3) {
    if (param_3 == 'c') {
      MIDICC_OBJ_1D8(iVar5,iVar4);
      return;
    }
    if (99 < param_3) {
      if (param_3 == 'e') {
        (*DAT_800cbc70)(iVar5,iVar4,uVar1);
        MIDICC_OBJ_224();
        return;
      }
      if (100 < param_3) {
        if (param_3 != 'y') {
          MIDICC_OBJ_20C(uVar3 << 0x10);
          return;
        }
        (*DAT_800cbc78)(iVar5,iVar4);
        MIDICC_OBJ_224();
        return;
      }
      MIDICC_OBJ_1D8(iVar5,iVar4);
      return;
    }
    if (param_3 == '[') {
      MIDICC_OBJ_1D8(iVar5,iVar4);
      return;
    }
    if (param_3 != 'b') {
      MIDICC_OBJ_20C(uVar3 << 0x10);
      return;
    }
    MIDICC_OBJ_1D8(iVar5,iVar4);
    return;
  }
  if (param_3 == '\a') {
    MIDICC_OBJ_1D8(iVar5,iVar4);
    return;
  }
  if (7 < param_3) {
    if (param_3 == '\n') {
      MIDICC_OBJ_1D8(iVar5,iVar4);
      return;
    }
    if (param_3 != '\v') {
      MIDICC_OBJ_20C(uVar3 << 0x10);
      return;
    }
    MIDICC_OBJ_1D8(iVar5,iVar4);
    return;
  }
  if (param_3 == '\0') {
    *(undefined1 *)((int)puVar6 + 0x26) = uVar1;
    MIDICC_OBJ_218(iVar5,iVar4);
    return;
  }
  if (param_3 != '\x06') {
    MIDICC_OBJ_20C(uVar3 << 0x10);
    return;
  }
  MIDICC_OBJ_1D8(iVar5,iVar4);
  return;
}


