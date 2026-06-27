void FUN_8004dc4c(char param_1,undefined1 param_2,undefined4 param_3)

{
  bool bVar1;
  byte bVar2;
  int iVar3;
  
  FUN_8004df90();
  bVar1 = 5 < (byte)(param_1 - 0xeU);
  if (bVar1) {
    bVar2 = FUN_8004df2c(6);
    iVar3 = (uint)bVar2 * 4;
    (&DAT_800b10ac)[iVar3] = param_1;
    (&DAT_800b10ad)[iVar3] = param_2;
    (&DAT_800b10ae)[iVar3] = 0;
  }
  else {
    DAT_800b25c8 = DAT_800b25c8 + '\x02';
    FUN_80049390(6,8);
    DAT_800b10cc = DAT_800b10c4;
    DAT_800b10cd = DAT_800b10c5;
    DAT_800b10ce = DAT_800b10c6;
    FUN_80049390(7,9);
    DAT_800b10d0 = DAT_800b10c8;
    DAT_800b10d1 = DAT_800b10c9;
    DAT_800b10d2 = DAT_800b10ca;
    FUN_80049390(4,6);
    DAT_800b10c4 = DAT_800b10bc;
    DAT_800b10c5 = DAT_800b10bd;
    DAT_800b10c6 = DAT_800b10be;
    FUN_80049390(5,7);
    DAT_800b10c8 = DAT_800b10c0;
    DAT_800b10c9 = DAT_800b10c1;
    DAT_800b10ca = DAT_800b10c2;
    FUN_80049390(2,4);
    DAT_800b10bc = DAT_800b10b4;
    DAT_800b10bd = DAT_800b10b5;
    DAT_800b10be = DAT_800b10b6;
    FUN_80049390(3,5);
    DAT_800b10c0 = DAT_800b10b8;
    DAT_800b10c1 = DAT_800b10b9;
    DAT_800b10c2 = DAT_800b10ba;
    FUN_80049390(0,2);
    DAT_800b10b4 = DAT_800b10ac;
    DAT_800b10b5 = DAT_800b10ad;
    DAT_800b10b6 = DAT_800b10ae;
    FUN_80049390(1,3);
    DAT_800b10ba = DAT_800b10b2;
    bVar2 = 0;
    DAT_800b10ae = 1;
    DAT_800b10b2 = 2;
    DAT_800b10b8 = DAT_800b10b0;
    DAT_800b10b9 = DAT_800b10b1;
    DAT_800b10ac = param_1;
    DAT_800b10ad = param_2;
    DAT_800b10b0 = param_1;
    DAT_800b10b1 = param_2;
  }
  FUN_800492b8(bVar2,!bVar1,param_3);
  return;
}
