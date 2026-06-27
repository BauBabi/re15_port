/* FUN_80026b7c @ 0x80026b7c  (Ghidra headless, raw MIPS overlay) */

void FUN_80026b7c(void)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  
  FUN_80039694();
  if ((DAT_800cfb74 & 0x1000000) != 0) {
    FUN_80015128();
  }
  uVar2 = DAT_800cfb74 | 0x2000000;
  uVar1 = DAT_800cfb74 & 0x1000000;
  DAT_800cfb74 = uVar2;
  if ((uVar1 == 0) || (iVar3 = FUN_80077360(&DAT_800d4a2c,0x3f), iVar3 == 0)) {
    FUN_8002bda8(2,0);
    FUN_80031f6c(1,&LAB_80013bc4);
    FUN_80031f94(1);
    FUN_80014cd0();
    FUN_80031f94(1);
  }
  else {
    DAT_800dcc1e = '\0';
    DAT_800cfbf2 = (byte)DAT_800d4820;
    FUN_80031f6c(1,&LAB_800335c0);
    while (((DAT_800cfb74 & 0x2000000) != 0 && (DAT_800dcc1e != '\x04'))) {
      FUN_80031f94(1);
    }
  }
  DAT_800cfc3c = *DAT_800ce550;
  DAT_800cfc30 = (int)DAT_800cfc3c;
  DAT_800cfc3e = DAT_800ce550[1];
  DAT_800cfc34 = (int)DAT_800cfc3e;
  DAT_800cfc40 = DAT_800ce550[2];
  DAT_800cfc38 = (int)DAT_800cfc40;
  DAT_800cfc6e = DAT_800ce550[3];
  DAT_800cfcfe = *(byte *)((int)DAT_800ce550 + 0xb);
  DAT_800cfdba = (ushort)DAT_800cfcfe * -0x708;
  DAT_800cfbf2 = *(byte *)(DAT_800ce550 + 5);
  DAT_800d4822 = DAT_800d481e + (DAT_800d481c + 1) * 0x100;
  DAT_800d4820 = (ushort)DAT_800cfbf2;
  DAT_800d481e = (ushort)*(byte *)((int)DAT_800ce550 + 9);
  DAT_800cfd10 = DAT_800cfc3c;
  DAT_800cfd12 = DAT_800cfc40;
  if ((int)DAT_800d481c != (uint)*(byte *)(DAT_800ce550 + 4) % 9) {
    DAT_800d481c = (short)((uint)*(byte *)(DAT_800ce550 + 4) % 9);
    FUN_8004a3c0();
  }
  while ((DAT_800cfbd8 & 0x20000) != 0) {
    FUN_80031f94(1);
  }
  FUN_8005a714();
  FUN_8001633c();
  FUN_8001645c();
  FUN_80049e48();
  while ((DAT_800cfb74 & 0x2000000) != 0) {
    FUN_80031f94(1);
  }
  FUN_8002bda8(0,0);
  FUN_8002c6b4(1);
  DAT_800dfc1b = 1;
  FUN_80031f94(1);
  if ((DAT_800cfbd8 & 0x400000) != 0) {
    FUN_8002bda8(2,0);
    DAT_800cfbd8 = DAT_800cfbd8 & 0xffbfffff;
  }
  return;
}


