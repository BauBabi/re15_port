void _SsContNrpn2(ushort param_1,short param_2,char param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = param_2 * 0xac + (&DAT_800bb500)[(short)param_1];
  if (param_3 == '\x14') {
    *(undefined1 *)(iVar2 + 0x16) = 0x14;
    *(undefined1 *)(iVar2 + 0x27) = 1;
    uVar1 = _SsReadDeltaValue();
    *(undefined4 *)(iVar2 + 0x88) = uVar1;
    *(undefined4 *)(iVar2 + 0xc) = *(undefined4 *)(iVar2 + 4);
    SEQREAD_OBJ_F38();
    return;
  }
  if (param_3 != '\x1e') {
    SEQREAD_OBJ_F10((uint)param_1 << 0x10);
    return;
  }
  *(undefined1 *)(iVar2 + 0x16) = 0x1e;
  if (*(byte *)(iVar2 + 0x28) == 0) {
    *(undefined1 *)(iVar2 + 0x10) = 0;
    SEQREAD_OBJ_F2C();
    return;
  }
  if (*(byte *)(iVar2 + 0x28) < 0x7f) {
    *(char *)(iVar2 + 0x28) = *(char *)(iVar2 + 0x28) + -1;
    uVar1 = _SsReadDeltaValue();
    *(undefined4 *)(iVar2 + 0x88) = uVar1;
    if (*(char *)(iVar2 + 0x28) != '\0') {
      *(undefined4 *)(iVar2 + 4) = *(undefined4 *)(iVar2 + 0xc);
      SEQREAD_OBJ_F38();
      return;
    }
    *(undefined1 *)(iVar2 + 0x10) = 0;
    SEQREAD_OBJ_F38();
    return;
  }
  _SsReadDeltaValue();
  *(undefined4 *)(iVar2 + 0x88) = 0;
  *(undefined4 *)(iVar2 + 4) = *(undefined4 *)(iVar2 + 0xc);
  SEQREAD_OBJ_F38();
  return;
}
