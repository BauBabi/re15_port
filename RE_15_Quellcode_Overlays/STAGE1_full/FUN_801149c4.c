/* FUN_801149c4 @ 0x801149c4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801149c4(void)

{
  char cVar1;
  uint *puVar2;
  uint uVar3;
  ushort uVar4;
  
  if (*(char *)((int)_DAT_800ac784 + 7) == '\0') {
    _DAT_800aca50 = _DAT_800aca50 + 1;
    if (*(char *)((int)_DAT_800ac784 + 0x1db) != '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xf0ff | 0x800;
    }
    func_0x800453d0(3);
    uVar4 = 0;
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
    uVar3 = 0;
    do {
      puVar2 = (uint *)(uVar3 * 0xac + _DAT_800ac784[0x62]);
      puVar2[0x1a] = 0x8f;
      *(undefined2 *)(puVar2 + 0x25) = 0;
      *(undefined2 *)((int)puVar2 + 0x96) = 0xffce;
      *(undefined2 *)(puVar2 + 0x26) = 0;
      *(undefined2 *)((int)puVar2 + 0x9a) = 3;
      *(undefined2 *)(puVar2 + 0x27) = 0;
      *(undefined2 *)((int)puVar2 + 0x9e) = 0;
      *puVar2 = *puVar2 | 0x4a;
      *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x32;
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      uVar4 = uVar4 + 1;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
      uVar3 = (uint)uVar4;
    } while (uVar4 < 0xd);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  }
  else if ((*(char *)((int)_DAT_800ac784 + 7) == '\x01') &&
          (cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5),
          *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1, cVar1 == '\0')) {
    uVar4 = 0;
    do {
      uVar3 = (uint)uVar4;
      uVar4 = uVar4 + 1;
      *(undefined4 *)(uVar3 * 0xac + _DAT_800ac784[0x62]) = 0;
    } while (uVar4 < 0xd);
    *(undefined1 *)(_DAT_800ac784 + 1) = 7;
    FUN_80115d74(1);
  }
  return;
}


