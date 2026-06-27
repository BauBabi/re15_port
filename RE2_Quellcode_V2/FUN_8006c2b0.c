/* FUN_8006c2b0 @ 0x8006c2b0  (Ghidra headless, raw MIPS overlay) */

void FUN_8006c2b0(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = (&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4];
  DAT_800d5c06 = 0;
  iVar2 = bVar1 + 0x10;
  switch(DAT_800d5bf4) {
  case '\0':
    DAT_800d531c = 0xaa;
    DAT_800d5308 = 0x3000;
    DAT_800d5314 = (uint)DAT_80098dfa * 0x10000 + (uint)DAT_80098df8 +
                   (uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4] * 6;
    DAT_800d531e = (&DAT_800a7ab8)[(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4]];
    FUN_80012fb8(0xaa,&DAT_801a0000,2,&DAT_80011be0);
    DAT_800cfbf0 = 0x817;
    FUN_80076a40(&DAT_801a0000);
    FUN_80074014(0);
    FUN_80075a00(0);
    DAT_800d5c3e = 0x102;
    DAT_800d5bf5 = 0;
    DAT_800d5bf4 = DAT_800d5bf4 + '\x01';
  case '\x01':
    DAT_800d5c28 = DAT_800d5c28 + -0xc;
  case '\x02':
    goto switchD_8006c310_caseD_2;
  case '\x03':
    DAT_800d5bf5 = DAT_800d5bf5 + 1;
    if (6 < DAT_800d5bf5) {
      DAT_800d5bf5 = 0;
      DAT_800d5bf4 = DAT_800d5bf4 + '\x01';
    }
    DAT_800d5c3e = DAT_800d5c3e + -0xf;
    break;
  case '\x04':
    DAT_800d5c3e = DAT_800d5c3e + -0xf;
    DAT_800d5c28 = DAT_800d5c28 + -0xc;
  case '\x05':
    DAT_800d5bf5 = DAT_800d5bf5 + 1;
    if (4 < DAT_800d5bf5) {
      DAT_800d5bf5 = 0;
      DAT_800d5bf4 = DAT_800d5bf4 + '\x01';
    }
    DAT_800d5c2a = DAT_800d5c2a + -8;
LAB_8006c5ec:
    DAT_800d5c28 = DAT_800d5c28 + 6;
    break;
  case '\x06':
    DAT_800d5bf5 = DAT_800d5bf5 + 1;
    if (8 < DAT_800d5bf5) {
      FUN_8002fe38(0xaf0010,0xe400,iVar2,0);
      DAT_800d5bf4 = DAT_800d5bf4 + '\x01';
    }
    break;
  case '\a':
    if ((DAT_800e873c & 0x80) != 0) break;
    FUN_8005ba28(0x4050000,0,iVar2);
    FUN_8007730c(&DAT_800d48ec,(&DAT_800a9e1d)[(uint)bVar1 * 8]);
    DAT_800d5bf4 = '\b';
    goto LAB_8006c6b8;
  case '\b':
    DAT_800d5c28 = DAT_800d5c28 + -0xc;
  case '\t':
    DAT_800d5bf5 = DAT_800d5bf5 + 1;
    if (4 < DAT_800d5bf5) {
      DAT_800d5bf5 = 0;
      DAT_800d5bf4 = DAT_800d5bf4 + '\x01';
    }
    DAT_800d5c2a = DAT_800d5c2a + 8;
    goto LAB_8006c5ec;
  case '\n':
    DAT_800d5bf5 = DAT_800d5bf5 + 1;
    if (6 < DAT_800d5bf5) {
      DAT_800d5bf5 = 0;
      DAT_800d5bf4 = DAT_800d5bf4 + '\x01';
    }
    DAT_800d5c3e = DAT_800d5c3e + 0xf;
    break;
  case '\v':
    DAT_800d5c3e = DAT_800d5c3e + 0xf;
    DAT_800d5c28 = DAT_800d5c28 + -0xc;
  case '\f':
    DAT_800d5bf5 = DAT_800d5bf5 + 1;
    if (4 < DAT_800d5bf5) {
      DAT_800d5bf5 = 0;
      DAT_800d5bf4 = DAT_800d5bf4 + '\x01';
    }
    DAT_800d5c28 = DAT_800d5c28 + 6;
    DAT_800d5c30 = DAT_800d5c30 + 6;
    DAT_800d5c2a = DAT_800d5c2a + -8;
    break;
  case '\r':
    DAT_800d5bf4 = '\0';
    DAT_800d5bf3 = 0;
    DAT_800d5bf2 = 0;
LAB_8006c6b8:
    DAT_800d5bf5 = 0;
  }
switchD_8006c310_default:
  FUN_80075c28(0);
  FUN_80074160(0);
  return;
switchD_8006c310_caseD_2:
  DAT_800d5bf5 = DAT_800d5bf5 + 1;
  if (4 < DAT_800d5bf5) {
    DAT_800d5bf5 = 0;
    DAT_800d5bf4 = DAT_800d5bf4 + '\x01';
  }
  DAT_800d5c28 = DAT_800d5c28 + 6;
  DAT_800d5c2a = DAT_800d5c2a + 8;
  DAT_800d5c38 = DAT_800d5c38 + 8;
  DAT_800d5c30 = DAT_800d5c30 + -6;
  goto switchD_8006c310_default;
}


