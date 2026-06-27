void FUN_8002ffb8(void)

{
  undefined4 uVar1;
  uint uVar2;
  uint uVar3;
  ushort *puVar4;
  byte *pbVar5;
  ushort *puVar6;
  
  pbVar5 = &DAT_80073d08;
  puVar4 = &DAT_80073d2c;
  puVar6 = &DAT_80073d2e;
  do {
    uVar2 = (uint)*pbVar5;
    uVar3 = (-(uint)(uVar2 == 7) & 3) << 4;
    pbVar5 = pbVar5 + 1;
    if (uVar2 < 0x12) {
      uVar1 = FUN_800c002c(uVar2);
      FUN_80028ec4((uint)*puVar4 + (uint)(byte)(&UNK_80073d18)[uVar2],*puVar6 + 1,uVar1,uVar3 | 4);
    }
    else {
      FUN_800279c8((*puVar6 + 1) * 0x10000 | (uint)*puVar4,0x84,
                   *(undefined4 *)(&DAT_80073c70 + uVar2 * 4),0);
    }
    puVar4 = puVar4 + 2;
    puVar6 = puVar6 + 2;
  } while (puVar4 < &DAT_80073d6c);
  uVar1 = FUN_800c002c(8);
  FUN_80028ec4(0x1e,0xb7,uVar1,4);
  uVar1 = FUN_800c002c(9);
  FUN_80028ec4(0x1e,0xc9,uVar1,4);
  uVar1 = FUN_800c002c(10);
  FUN_80028ec4(0xae,0xb7,uVar1,4);
  uVar1 = FUN_800c002c(0xb);
  FUN_80028ec4(0xae,0xc9,uVar1,4);
  return;
}
