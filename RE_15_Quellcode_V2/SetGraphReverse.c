int SetGraphReverse(int mode)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  
  uVar4 = (uint)DAT_8007e353;
  if (1 < DAT_8007e352) {
    printf("SetGraphReverse(%d)...\n",mode);
  }
  DAT_8007e353 = (byte)mode;
  uVar1 = _getctl(8);
  if (DAT_8007e353 != 0) {
    iVar2 = SYS_OBJ_1E4();
    return iVar2;
  }
  _ctl(uVar1 | 0x8000000);
  if (DAT_8007e350 == '\x02') {
    uVar3 = 0x20000504;
    if (DAT_8007e353 != 0) {
      uVar3 = 0x20000501;
    }
    _ctl(uVar3);
  }
  return uVar4;
}
