/* FUN_800172f8 @ 0x800172f8  (Ghidra headless, raw MIPS overlay) */

void FUN_800172f8(int param_1,ushort param_2,undefined4 param_3,short param_4,byte *param_5)

{
  char cVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  
  uVar4 = (uint)*param_5;
  iVar5 = *(int *)(param_1 + 0x10);
  if (uVar4 != 0) {
    do {
      param_5 = param_5 + 1;
      uVar4 = uVar4 - 1;
      iVar3 = iVar5 + (uint)*param_5 * 0x68;
      cVar1 = (char)param_3;
      *(char *)(iVar3 + 0xc) = *(char *)(iVar3 + 0xc) + cVar1;
      *(char *)(iVar3 + 0x18) = *(char *)(iVar3 + 0x18) + cVar1;
      cVar2 = (char)((uint)param_3 >> 0x10);
      *(char *)(iVar3 + 0xd) = *(char *)(iVar3 + 0xd) + cVar2;
      *(char *)(iVar3 + 0x24) = *(char *)(iVar3 + 0x24) + cVar1;
      *(char *)(iVar3 + 0x19) = *(char *)(iVar3 + 0x19) + cVar2;
      *(char *)(iVar3 + 0x30) = *(char *)(iVar3 + 0x30) + cVar1;
      *(char *)(iVar3 + 0x25) = *(char *)(iVar3 + 0x25) + cVar2;
      *(char *)(iVar3 + 0x31) = *(char *)(iVar3 + 0x31) + cVar2;
      *(ushort *)(iVar3 + 0x1a) = *(ushort *)(iVar3 + 0x1a) & 0xffe0 | param_2 & 0x1f;
      *(ushort *)(iVar3 + 0xe) = *(ushort *)(iVar3 + 0xe) & 0x803f | param_4 << 6;
      *(char *)(iVar3 + 0x40) = *(char *)(iVar3 + 0x40) + cVar1;
      *(char *)(iVar3 + 0x4c) = *(char *)(iVar3 + 0x4c) + cVar1;
      *(char *)(iVar3 + 0x41) = *(char *)(iVar3 + 0x41) + cVar2;
      *(char *)(iVar3 + 0x58) = *(char *)(iVar3 + 0x58) + cVar1;
      *(char *)(iVar3 + 0x4d) = *(char *)(iVar3 + 0x4d) + cVar2;
      *(char *)(iVar3 + 0x59) = *(char *)(iVar3 + 0x59) + cVar2;
      *(char *)(iVar3 + 100) = *(char *)(iVar3 + 100) + cVar1;
      *(char *)(iVar3 + 0x65) = *(char *)(iVar3 + 0x65) + cVar2;
      *(ushort *)(iVar3 + 0x4e) = *(ushort *)(iVar3 + 0x4e) & 0xffe0 | param_2 & 0x1f;
      *(ushort *)(iVar3 + 0x42) = *(ushort *)(iVar3 + 0x42) & 0x803f | param_4 << 6;
    } while (uVar4 != 0);
  }
  return;
}


