/* FUN_80042470 @ 0x80042470  (Ghidra headless, raw MIPS overlay) */

void FUN_80042470(int param_1)

{
  char cVar1;
  byte bVar2;
  SVECTOR *r;
  int iVar3;
  int iVar4;
  uint *puVar5;
  uint *puVar6;
  MATRIX MStack_48;
  SVECTOR local_28;
  SVECTOR local_20;
  
  if (((*(ushort *)(param_1 + 0x10e) & 0xfff) != 0) &&
     (iVar3 = (byte)(&DAT_800a6e77)[*(ushort *)(param_1 + 0x10e) & 0xfff] - 1,
     (byte)(&DAT_800a6e77)[*(ushort *)(param_1 + 0x10e) & 0xfff] != 0)) {
    puVar5 = (uint *)(*(int *)(param_1 + 0x198) + (uint)(byte)(&DAT_800a6e90)[iVar3 * 6] * 0xac);
    if (((*(byte *)(param_1 + 8) & 1) != 0) && (*(byte *)(param_1 + 8) < 10)) {
      puVar5 = puVar5 + 0xac;
    }
    if ((*(char *)(param_1 + 8) == '\x0e') ||
       (r = (SVECTOR *)(puVar5 + 0x1a), puVar6 = puVar5, *(char *)(param_1 + 8) == 'A')) {
      puVar6 = puVar5 + 0x2b;
      r = (SVECTOR *)(puVar5 + 0x45);
    }
    iVar3 = iVar3 * 6;
    iVar4 = *(int *)(param_1 + 0x198);
    *puVar6 = *puVar6 | 0x41;
    cVar1 = (&DAT_800a6e91)[iVar3];
    *(undefined1 *)((int)puVar6 + 0x79) = 1;
    *(undefined1 *)((int)puVar6 + 0x7a) = 0;
    *(short *)(puVar6 + 0x21) = (short)cVar1;
    bVar2 = (&DAT_800a6e93)[iVar3];
    *(undefined2 *)(puVar6 + 0x1a) = 0;
    *(undefined2 *)((int)puVar6 + 0x6a) = 0;
    *(undefined2 *)(puVar6 + 0x1b) = 0;
    *(ushort *)((int)puVar6 + 0x86) = (ushort)bVar2;
    RotMatrix(r,(MATRIX *)(puVar6 + 6));
    FUN_8002ce94(iVar4 + 0x7ac,(MATRIX *)(puVar6 + 6),puVar6 + 0x12);
    *(undefined2 *)(puVar6 + 0xe) = *(undefined2 *)(param_1 + 0x1c2);
    local_28.vx = 0;
    local_28.vz = 0;
    local_28.vy = *(short *)(&DAT_800a6e94 + iVar3) + *(short *)(param_1 + 0x76);
    RotMatrix(&local_28,&MStack_48);
    local_28.vy = 0;
    local_28.vx = (short)(byte)(&DAT_800a6e92)[iVar3];
    ApplyMatrixSV(&MStack_48,&local_28,&local_20);
    *(short *)((int)puVar6 + 0x3a) = local_20.vx;
    *(short *)(puVar6 + 0xf) = local_20.vz;
  }
  return;
}


