/* FUN_80113f38 @ 0x80113f38  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113f38(void)

{
  char cVar1;
  ushort uVar2;
  uint *puVar3;
  uint uVar4;
  
  puVar3 = (uint *)(uint)*(byte *)((int)_DAT_800ac784 + 7);
  uVar2 = (ushort)(puVar3 < (uint *)0x2);
  if (puVar3 != (uint *)0x1) {
    if (puVar3 >= (uint *)0x2) {
      uVar2 = 2;
      if (puVar3 == (uint *)0x2) {
        func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
        cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
        if (cVar1 != '\0') {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 1) = 7;
        FUN_80115d74(0);
        *_DAT_800ac784 = *_DAT_800ac784 | 2;
        *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
        return;
      }
      goto code_r0x801149b8;
    }
    if (puVar3 != (uint *)0x0) goto code_r0x801149b8;
    func_0x800453d0(3);
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x7a) = 0x26;
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
    _DAT_800aca50 = _DAT_800aca50 + 1;
    if (*(char *)((int)_DAT_800ac784 + 0x1db) != '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xf0ff | 0x800;
    }
  }
  *(short *)(_DAT_800ac784 + 0x1b) = (short)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)_DAT_800ac784[0x1b]) {
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + (short)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar4 = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)_DAT_800ac784[0xe] < (int)uVar4) {
    return;
  }
  _DAT_800ac784[0xe] = uVar4;
  *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
  FUN_80115d94(10);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
  func_0x800453d0(5);
  puVar3 = _DAT_800ac784;
  uVar2 = *(byte *)((int)_DAT_800ac784 + 7) + 1;
  *(char *)((int)_DAT_800ac784 + 7) = (char)uVar2;
code_r0x801149b8:
  *(ushort *)(puVar3 + 0x79) = -uVar2;
  *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  FUN_801152cc();
  return;
}


