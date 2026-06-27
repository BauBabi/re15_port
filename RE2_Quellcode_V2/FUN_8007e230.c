/* FUN_8007e230 @ 0x8007e230  (Ghidra headless, raw MIPS overlay) */

void FUN_8007e230(ushort param_1,int param_2)

{
  byte bVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  undefined4 *puVar5;
  
  piVar4 = (int *)((int)&DAT_800ea250 + ((int)((uint)param_1 << 0x10) >> 0xe));
  iVar3 = (short)param_2 * 0xb0;
  puVar5 = (undefined4 *)(*piVar4 + iVar3);
  bVar1 = *(char *)((int)puVar5 + 0x21) + 1;
  *(byte *)((int)puVar5 + 0x21) = bVar1;
  if (*(byte *)(puVar5 + 8) == 0) {
    puVar5[0x22] = 0;
    *(undefined1 *)(puVar5 + 7) = 0;
    puVar5[0x24] = 0;
    if ((*(uint *)(iVar3 + *piVar4 + 0x98) & 0x400) == 0) {
      *puVar5 = puVar5[1];
    }
    else {
      *puVar5 = puVar5[3];
    }
  }
  else if (bVar1 < *(byte *)(puVar5 + 8)) {
    puVar5[0x22] = 0;
    *(undefined1 *)(puVar5 + 7) = 0;
    puVar5[0x24] = 0;
    if ((*(uint *)(iVar3 + *piVar4 + 0x98) & 0x400) == 0) {
      uVar2 = puVar5[1];
      *puVar5 = puVar5[1];
    }
    else {
      uVar2 = puVar5[3];
      *puVar5 = puVar5[3];
    }
    puVar5[2] = uVar2;
  }
  else {
    *(uint *)(iVar3 + *piVar4 + 0x98) = *(uint *)(iVar3 + *piVar4 + 0x98) & 0xfffffffe;
    *(uint *)(iVar3 + *piVar4 + 0x98) = *(uint *)(iVar3 + *piVar4 + 0x98) & 0xfffffff7;
    *(uint *)(iVar3 + *piVar4 + 0x98) = *(uint *)(iVar3 + *piVar4 + 0x98) & 0xfffffffd;
    *(uint *)(iVar3 + *piVar4 + 0x98) = *(uint *)(iVar3 + *piVar4 + 0x98) | 0x200;
    *(uint *)(iVar3 + *piVar4 + 0x98) = *(uint *)(iVar3 + *piVar4 + 0x98) | 4;
    *(undefined1 *)(puVar5 + 5) = 0;
    if ((*(uint *)(iVar3 + *piVar4 + 0x98) & 0x400) == 0) {
      puVar5[2] = puVar5[1];
    }
    else {
      puVar5[2] = puVar5[3];
    }
    if (*(char *)((int)puVar5 + 0x22) != -1) {
      *(undefined1 *)(puVar5 + 5) = 0;
      _SsSndNextSep(*(undefined1 *)((int)puVar5 + 0x22),*(undefined1 *)((int)puVar5 + 0x23));
      FUN_800846dc((int)(short)(param_1 | (ushort)(param_2 << 8)));
    }
    FUN_800846dc((int)(short)(param_1 | (ushort)(param_2 << 8)));
    puVar5[0x24] = (int)*(short *)(puVar5 + 0x15);
  }
  return;
}


