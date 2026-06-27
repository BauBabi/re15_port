/* FUN_8002ec70 @ 0x8002ec70  (Ghidra headless, raw MIPS overlay) */

void FUN_8002ec70(int param_1)

{
  undefined1 uVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  uint *puVar5;
  uint uVar6;
  
  uVar1 = DAT_800cfb7d;
  puVar5 = *(uint **)(param_1 + 0x1a4);
  uVar6 = (uint)*(byte *)(param_1 + 0x107);
  iVar3 = (int)DAT_800d4820;
  iVar4 = *(int *)(DAT_800ce324 + 0x24);
  do {
    if ((*puVar5 & 0x8000) != 0) {
      *puVar5 = *puVar5 & 0xffffbfff;
      cVar2 = FUN_8002ea40(iVar4 + iVar3 * 0x20,uVar1,puVar5 + 0x17);
      if (cVar2 == '\0') {
        *puVar5 = *puVar5 | 0x4000;
      }
    }
    uVar6 = uVar6 - 1;
    puVar5 = puVar5 + 0x2b;
  } while (uVar6 != 0);
  return;
}


