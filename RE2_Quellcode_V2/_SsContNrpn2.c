/* _SsContNrpn2 @ 0x8007b19c  (Ghidra headless, raw MIPS overlay) */

void _SsContNrpn2(short param_1,short param_2,uchar param_3)

{
  byte bVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  
  puVar3 = (undefined4 *)((&DAT_800ea250)[param_1] + param_2 * 0xb0);
  if (param_3 == '\x14') {
    *(undefined1 *)((int)puVar3 + 0x1b) = 0x14;
    *(undefined1 *)(puVar3 + 7) = 1;
    uVar2 = _SsReadDeltaValue();
    puVar3[0x24] = uVar2;
    puVar3[2] = *puVar3;
    CC_99_OBJ_120();
    return;
  }
  if (param_3 != '\x1e') {
    CC_99_OBJ_F8((uint)(ushort)param_1 << 0x10);
    return;
  }
  bVar1 = *(byte *)((int)puVar3 + 0x1d);
  *(undefined1 *)((int)puVar3 + 0x1b) = 0x1e;
  if (bVar1 == 0) {
    *(undefined1 *)((int)puVar3 + 0x15) = 0;
    CC_99_OBJ_114();
    return;
  }
  if (bVar1 < 0x7f) {
    *(byte *)((int)puVar3 + 0x1d) = bVar1 - 1;
    uVar2 = _SsReadDeltaValue();
    puVar3[0x24] = uVar2;
    if (*(char *)((int)puVar3 + 0x1d) != '\0') {
      *puVar3 = puVar3[2];
      CC_99_OBJ_120();
      return;
    }
    *(undefined1 *)((int)puVar3 + 0x15) = 0;
    CC_99_OBJ_120();
    return;
  }
  _SsReadDeltaValue();
  puVar3[0x24] = 0;
  *puVar3 = puVar3[2];
  CC_99_OBJ_120();
  return;
}


