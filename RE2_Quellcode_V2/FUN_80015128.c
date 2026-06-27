/* FUN_80015128 @ 0x80015128  (Ghidra headless, raw MIPS overlay) */

void FUN_80015128(void)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  uVar3 = 0;
  iVar1 = FUN_80077360(&DAT_800d4854,0xfc);
  uVar2 = 0xf8;
  if (iVar1 == 0) {
    do {
      iVar1 = FUN_80077360(&DAT_800d48bc,uVar2);
      if (iVar1 != 0) {
        uVar3 = uVar3 + 1;
      }
      uVar2 = uVar2 + 1;
    } while (uVar2 < 0x100);
    if (6 < uVar3) {
      FUN_8007730c(&DAT_800d4854,0xfc);
      FUN_8005b9f4(0x105ff34);
      uVar2 = 7;
      do {
        if (uVar2 == 0xf) {
          uVar2 = 0x10;
        }
        FUN_8005b9f4(uVar2 << 0x10 | 0x100ff34);
        uVar2 = uVar2 + 1;
      } while (uVar2 < 0x18);
      uVar2 = 0;
      do {
        if (uVar2 == 8) {
          uVar2 = 9;
        }
        if (uVar2 == 0x17) {
          uVar2 = 0x19;
        }
        FUN_8005b9f4(uVar2 << 0x10 | 0x200ff34);
        uVar2 = uVar2 + 1;
      } while (uVar2 < 0x1c);
      uVar2 = 0;
      do {
        if (uVar2 == 5) {
          uVar2 = 7;
        }
        FUN_8005b9f4(uVar2 << 0x10 | 0x300ff34);
        uVar2 = uVar2 + 1;
      } while (uVar2 < 0xc);
    }
  }
  return;
}


