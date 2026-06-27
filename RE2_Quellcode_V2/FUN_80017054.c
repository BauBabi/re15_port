/* FUN_80017054 @ 0x80017054  (Ghidra headless, raw MIPS overlay) */

void FUN_80017054(int param_1,ushort param_2,undefined4 param_3,int param_4)

{
  char cVar1;
  char cVar2;
  ushort *puVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = *(int *)(param_1 + 0x10);
  iVar4 = *(int *)(*(int *)(param_1 + 4) + 0x14) * 2;
  cVar1 = (char)param_3;
  cVar2 = (char)((uint)param_3 >> 0x10);
  if (iVar4 != 0) {
    puVar3 = (ushort *)(*(int *)(param_1 + 8) + 0xe);
    do {
      iVar4 = iVar4 + -1;
      *(char *)(puVar3 + -1) = (char)puVar3[-1] + cVar1;
      *(char *)(puVar3 + 5) = (char)puVar3[5] + cVar1;
      *(char *)((int)puVar3 + -1) = *(char *)((int)puVar3 + -1) + cVar2;
      *(char *)(puVar3 + 0xb) = (char)puVar3[0xb] + cVar1;
      *(char *)((int)puVar3 + 0xb) = *(char *)((int)puVar3 + 0xb) + cVar2;
      *(char *)((int)puVar3 + 0x17) = *(char *)((int)puVar3 + 0x17) + cVar2;
      puVar3[6] = puVar3[6] & 0xffe0 | param_2 & 0x1f;
      *puVar3 = *puVar3 & 0x803f | (ushort)(param_4 << 6);
      puVar3 = puVar3 + 0x14;
    } while (iVar4 != 0);
  }
  iVar4 = *(int *)(*(int *)(param_1 + 0xc) + 0x14) * 2;
  if (iVar4 != 0) {
    puVar3 = (ushort *)(iVar5 + 0xe);
    do {
      iVar4 = iVar4 + -1;
      *(char *)(puVar3 + -1) = (char)puVar3[-1] + cVar1;
      *(char *)(puVar3 + 5) = (char)puVar3[5] + cVar1;
      *(char *)((int)puVar3 + -1) = *(char *)((int)puVar3 + -1) + cVar2;
      *(char *)(puVar3 + 0xb) = (char)puVar3[0xb] + cVar1;
      *(char *)((int)puVar3 + 0xb) = *(char *)((int)puVar3 + 0xb) + cVar2;
      *(char *)(puVar3 + 0x11) = (char)puVar3[0x11] + cVar1;
      *(char *)((int)puVar3 + 0x17) = *(char *)((int)puVar3 + 0x17) + cVar2;
      *(char *)((int)puVar3 + 0x23) = *(char *)((int)puVar3 + 0x23) + cVar2;
      puVar3[6] = puVar3[6] & 0xffe0 | param_2 & 0x1f;
      *puVar3 = *puVar3 & 0x803f | (ushort)(param_4 << 6);
      puVar3 = puVar3 + 0x1a;
    } while (iVar4 != 0);
  }
  return;
}


