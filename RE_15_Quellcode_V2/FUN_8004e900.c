char FUN_8004e900(void)

{
  byte bVar1;
  uint uVar2;
  char *pcVar3;
  byte bVar4;
  uint uVar5;
  
  if ((&DAT_800b10ae)[(uint)DAT_800b25bd * 4] == '\x02') {
    uVar5 = DAT_800b25bd - 1;
  }
  else {
    uVar5 = (uint)DAT_800b25bd;
  }
  if ((&DAT_800b10ae)[(uint)DAT_800b25be * 4] == '\x02') {
    uVar2 = DAT_800b25be - 1;
  }
  else {
    uVar2 = (uint)DAT_800b25be;
  }
  if ((uVar2 & 0xff) != (uVar5 & 0xff)) {
    if ((&DAT_800b10ac)[(uVar2 & 0xff) * 4] != '\0') {
      bVar1 = (&DAT_80074db1)[(uint)(byte)(&DAT_800b10ac)[(uVar5 & 0xff) * 4] * 0xc];
      pcVar3 = (&PTR_DAT_80074dac)[(uint)(byte)(&DAT_800b10ac)[(uVar5 & 0xff) * 4] * 3];
      if (bVar1 == 0) {
        return '\0';
      }
      if (*pcVar3 == '\0') {
        return '\0';
      }
      bVar4 = 0;
      if (bVar1 != 0) {
        do {
          bVar4 = bVar4 + 1;
          if (*pcVar3 == (&DAT_800b10ac)[(uVar2 & 0xff) * 4]) {
            DAT_800b25d5 = pcVar3[1];
            DAT_800b260c = pcVar3[3];
            return pcVar3[2];
          }
          pcVar3 = pcVar3 + 4;
        } while (bVar4 < bVar1);
      }
    }
  }
  return '\0';
}
