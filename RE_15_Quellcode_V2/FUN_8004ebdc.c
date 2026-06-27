void FUN_8004ebdc(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  
  cVar2 = FUN_8004dfec(*(&PTR_DAT_80074dac)[(uint)(byte)(&DAT_800b10ac)[(uint)DAT_800b25c8 * 4] * 3]
                      );
  iVar3 = (uint)DAT_800b25c8 * 4;
  iVar4 = cVar2 * 4;
  bVar1 = (&DAT_80074da8)[(uint)(byte)(&DAT_800b10ac)[iVar3] * 0xc];
  if (bVar1 < (byte)(&DAT_800b10ad)[iVar4]) {
    (&DAT_800b10ad)[iVar3] = bVar1 + (&DAT_800b10ad)[iVar3];
    (&DAT_800b10ad)[iVar4] = (&DAT_800b10ad)[iVar4] - bVar1;
  }
  else {
    (&DAT_800b10ad)[iVar3] = (&DAT_800b10ad)[iVar3] + (&DAT_800b10ad)[iVar4];
    FUN_8004947c((int)cVar2 & 0xffff);
    (&DAT_800b10ac)[iVar4] = 0;
    (&DAT_800b10ad)[iVar4] = 0;
    (&DAT_800b10ae)[iVar4] = 0;
  }
  return;
}
