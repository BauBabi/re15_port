void FUN_80037c1c(void)

{
  byte bVar1;
  DR_MOVE *p;
  int iVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  uint uVar5;
  
  uVar5 = 0;
  puVar4 = &DAT_800b10ec;
  puVar3 = &DAT_800b10ed;
  do {
    *puVar4 = 0;
    bVar1 = DAT_800acad4;
    *(undefined2 *)(puVar3 + 0x17) = 0x10;
    *(undefined2 *)(puVar3 + 0x19) = 0x40;
    iVar2 = (uVar5 & 0xff) * 2;
    p = (DR_MOVE *)(puVar4 + 4);
    *(ushort *)(puVar3 + 0xf) = (bVar1 - 0x10) * 0x40 + ((ushort)uVar5 & 3) * 0x10 + 0x80;
    *(ushort *)(puVar3 + 0x13) =
         (ushort)(byte)(&DAT_80074208)[iVar2 + (uint)DAT_800aca5c * 0x10] +
         (DAT_800acad4 - 0x10) * 0x40;
    puVar4 = puVar4 + 0x1c;
    uVar5 = uVar5 + 1;
    *(ushort *)(puVar3 + 0x15) = (byte)(&DAT_80074209)[iVar2 + (uint)DAT_800aca5c * 0x10] + 0x100;
    SetDrawMove(p);
    *puVar3 = 0;
    puVar3 = puVar3 + 0x1c;
  } while ((uVar5 & 0xff) < 8);
  return;
}
