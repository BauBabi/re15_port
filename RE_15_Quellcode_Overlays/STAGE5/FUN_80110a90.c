/* FUN_80110a90 @ 0x80110a90  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110a90(int param_1)

{
  uint *puVar1;
  byte bVar2;
  char cVar3;
  uint *puVar4;
  
  bVar2 = *(byte *)(*(int *)(param_1 + -0x387c) + 7);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      return;
    }
    if (bVar2 != 0) {
      return;
    }
    *(undefined1 *)(*(int *)(param_1 + -0x387c) + 0x9e) = 0x5a;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
    *_DAT_800ac784 = *_DAT_800ac784 | 2;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
  }
  puVar4 = _DAT_800ac784;
  puVar1 = _DAT_800ac784 + 0x3b;
  _DAT_800ac784[0x31] = _DAT_800ac784[0x31] & 0xff000000 | 0xffff38;
  puVar4[0x3b] = *puVar1 & 0xff000000 | 0xffff38;
  *(short *)(puVar4 + 0x2f) = (short)puVar4[0x2f] + 8;
  *(short *)((int)puVar4 + 0xbe) = *(short *)((int)puVar4 + 0xbe) + 8;
  cVar3 = *(char *)((int)_DAT_800ac784 + 0x9e);
  *(char *)((int)_DAT_800ac784 + 0x9e) = cVar3 + -1;
  if (cVar3 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 2;
  }
  return;
}


