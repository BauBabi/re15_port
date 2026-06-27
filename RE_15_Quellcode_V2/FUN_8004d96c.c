void FUN_8004d96c(void)

{
  uint uVar1;
  ushort uVar2;
  undefined *puVar3;
  ushort uVar4;
  
  uVar4 = 0x13;
  do {
    FUN_80047648(uVar4 & 0xff);
    uVar4 = uVar4 + 1;
  } while (uVar4 < 0x19);
  puVar3 = &DAT_80199800;
  if (DAT_800aca34 == 0) {
    puVar3 = &DAT_80199814;
  }
  uVar4 = 0;
  do {
    uVar2 = uVar4 & 0xff;
    uVar1 = (uint)uVar4;
    uVar4 = uVar4 + 1;
    puVar3 = (undefined *)
             FUN_80048f28(puVar3,uVar2,(&DAT_80074db0)[(uint)(byte)(&DAT_800b10ac)[uVar1 * 4] * 0xc]
                          ,0);
  } while (uVar4 < 10);
  AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,&DAT_800b2638 + (uint)DAT_800aca34 * 0xc);
  puVar3 = &DAT_80198000;
  if (DAT_800aca34 == 0) {
    puVar3 = &DAT_80198014;
  }
  uVar4 = 0;
  do {
    puVar3 = (undefined *)FUN_80048704(uVar4 & 0xff,puVar3);
    uVar4 = uVar4 + 1;
  } while (uVar4 < 10);
  AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,&DAT_800b2620 + (uint)DAT_800aca34 * 0xc);
  return;
}
