/* FUN_80113e94 @ 0x80113e94  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113e94(void)

{
  char cVar1;
  short sVar2;
  int iVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(8);
    *(char *)(_DAT_800ac784 + 0x1d6) = *(char *)(_DAT_800ac784 + 0x1d6) + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 100;
    FUN_801161e8();
    if (_DAT_800acaee < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    }
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
    func_0x800453d0(2);
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar3 != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
  }
  cVar1 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    func_0x800453d0(2);
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
  }
  if ((uint)*(byte *)(_DAT_800ac784 + 0x1d5) % 10 == 0) {
    func_0x800453d0(0);
  }
  sVar2 = func_0x80037024();
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -1 + sVar2 * -3;
  if (*(short *)(_DAT_800ac784 + 0x9c) < 0) {
    _DAT_800aca5a = CONCAT11(DAT_800aca5b,DAT_800aca5a + '\x01');
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x4000;
    FUN_80115d74(0xe);
  }
  return;
}


