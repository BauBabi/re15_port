/* FUN_8004a694 @ 0x8004a694  (Ghidra headless, raw MIPS overlay) */

void FUN_8004a694(void)

{
  bool bVar1;
  short sVar2;
  ushort uVar3;
  uint *puVar4;
  undefined4 *puVar5;
  
  bVar1 = (DAT_800cfbdc & 0x20000000) != 0;
  if (bVar1) {
    DAT_800cfbdc = DAT_800cfbdc & 0xdfffffff;
  }
  puVar5 = &DAT_800cfe18;
  if (DAT_800ce334 != &DAT_800cfe18) {
    do {
      puVar4 = (uint *)*puVar5;
      DAT_800ce330 = puVar4;
      if ((*puVar4 & 1) != 0) {
        uVar3 = *(ushort *)((int)puVar4 + 0x10e);
        *(ushort *)((int)puVar4 + 0x10e) = uVar3 & 0x7fff;
        sVar2 = (short)puVar4[0x56];
        (**(code **)(&DAT_800cfe5c + (uint)(byte)puVar4[2] * 4))(puVar4);
        if ((int)sVar2 != 0) {
          puVar4[1] = ((int)sVar2 & 0xff00U) - 0x100 | 4;
          *(char *)(puVar4 + 0x53) = (char)sVar2;
          *(undefined2 *)(puVar4 + 0x73) = *(undefined2 *)((int)puVar4 + 0x15a);
          (**(code **)(&DAT_800cfe5c + (uint)(byte)puVar4[2] * 4))(puVar4);
        }
        *(ushort *)((int)puVar4 + 0x10e) = *(ushort *)((int)puVar4 + 0x10e) | uVar3 & 0x8000;
      }
      puVar5 = puVar5 + 1;
    } while (puVar5 != DAT_800ce334);
  }
  if (bVar1) {
    DAT_800cfbdc = DAT_800cfbdc | 0x20000000;
  }
  return;
}


