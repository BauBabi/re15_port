/* FUN_8006affc @ 0x8006affc  (Ghidra headless, raw MIPS overlay) */

void FUN_8006affc(void)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  
  DAT_800d5c06 = 0;
  iVar2 = (uint)DAT_800d5bfc * 4;
  if ((byte)(&DAT_800d4a3c)[iVar2] < 0x14) {
    if ((uint)DAT_800d5bf8 == (uint)DAT_800d5bfc) {
      DAT_800d5bf2 = 4;
      DAT_800d5bf3 = 0;
      DAT_800d5bf8 = 0x80;
      DAT_800d5bfa = 0;
      DAT_800d5c06 = 0;
      return;
    }
    DAT_800d5bf2 = 4;
    DAT_800d5bf3 = 0;
    DAT_800d5bf8 = DAT_800d5bfc;
    DAT_800d5bfa = (&DAT_800d4a3c)[iVar2];
    DAT_800d5c06 = 0;
    return;
  }
  switch(DAT_800d5bf4) {
  case 0:
    uVar3 = (uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4];
    if (uVar3 < 0x1e) {
      uVar4 = 5;
      goto LAB_8006b248;
    }
    if (0xb < uVar3 - 0x23) {
switchD_8006b134_caseD_24:
      iVar2 = FUN_80077360(&DAT_800cfbe0,(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4]);
      if (iVar2 != 0) {
        DAT_800d5bf4 = 3;
        return;
      }
      uVar4 = 7;
LAB_8006b248:
      FUN_8002fe38(0xaf0010,0xe400,uVar4,0);
      DAT_800d5bf4 = 1;
      return;
    }
    sVar1 = DAT_800cfd5a >> 2;
    switch(uVar3) {
    case 0x23:
      DAT_800d46c2 = DAT_800d46c2 + 1;
      DAT_800d69f0 = DAT_800cfd5a;
      break;
    default:
      goto switchD_8006b134_caseD_24;
    case 0x26:
      DAT_800d69f0 = sVar1;
      break;
    case 0x28:
      DAT_800d5c52 = '\x02';
      DAT_800d69f0 = 0;
      goto LAB_8006b1e8;
    case 0x29:
      DAT_800d69f0 = DAT_800cfd5a >> 1;
      break;
    case 0x2a:
    case 0x2c:
      DAT_800d69f0 = DAT_800cfd5a;
      break;
    case 0x2b:
      goto code_r0x8006b1e0;
    case 0x2d:
      sVar1 = DAT_800cfd5a >> 1;
      goto code_r0x8006b1e0;
    case 0x2e:
      sVar1 = DAT_800cfd5a;
code_r0x8006b1e0:
      DAT_800d5c52 = '\x03';
      DAT_800d69f0 = sVar1;
LAB_8006b1e8:
      DAT_800cfe10 = 0;
      goto LAB_8006b1ec;
    }
    DAT_800d5c52 = '\x01';
LAB_8006b1ec:
    DAT_800d5bf4 = 4;
    break;
  case 1:
    if ((DAT_800e873c & 0x80) == 0) {
      DAT_800d5bf2 = 4;
      DAT_800d5bf3 = 0;
    }
    break;
  case 2:
    FUN_8006947c(DAT_800d5bfc,0,0,0);
    FUN_80069e54((uint)(DAT_800d5bfc >> 1) + (uint)DAT_800d5bfc);
    DAT_800d5bf3 = 1;
    DAT_800d5bf4 = 2;
    DAT_800d5bf6 = 0;
    DAT_800d5bf5 = 0;
    break;
  case 3:
    DAT_800d5bf1 = 0;
    DAT_800d47f0 = (ushort)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bfc * 4];
    break;
  case 4:
    if (DAT_800d5c52 == '\0') {
      DAT_800d5bf4 = 2;
      DAT_800cfd4c = DAT_800cfd4c & 0xefff;
      DAT_800cfd4e = DAT_800cfd4e + DAT_800d69f0;
      if (DAT_800cfd5a <= DAT_800cfd4e) {
        DAT_800cfd4e = DAT_800cfd5a;
      }
    }
  }
  return;
}


