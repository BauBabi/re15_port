void FUN_80026658(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  
  switch(param_1) {
  default:
    goto switchD_800266bc_caseD_0;
  case 1:
    FUN_800c0104(0x4c,0x60,0xe,0);
    FUN_800c0104(0x4c,0x78,0xf,0);
    uVar2 = 0x3c;
    break;
  case 2:
    if (param_2 != 0) {
      FUN_800c0158(0x140,0x40,6,0);
    }
    else {
      FUN_800c0158(0x140,0x40,5,0);
    }
    FUN_800c0104(0x68,0x90,param_2 != 0,0);
    FUN_800c0104(0x68,0x60,2,0);
    FUN_800c0104(0x68,0x78,3,0);
    uVar2 = 0x58;
    break;
  case 3:
    if (DAT_801ff404 == '\0') {
      uVar2 = 2;
    }
    else {
      uVar2 = 3;
    }
    FUN_800c0104(0x68,0x18,uVar2,0);
    FUN_800c0158(0x140,0x60,param_4 + 0x11,0x10);
    return;
  case 10:
    if (DAT_801ff404 == '\0') {
      uVar2 = 2;
    }
    else {
      uVar2 = 3;
    }
    FUN_800c0158(0x140,0x18,uVar2,0);
    FUN_800c0104(0x1b,0x40,0xb,0x10);
    FUN_800c0104(0x61,0x60,0xc,0);
    FUN_800c0104(0x61,0x78,0xd,0);
    uVar2 = 0x51;
    break;
  case 0xd:
    FUN_800c0158(0x140,0xb2,8,0x10);
    FUN_800c0104(0x8b,0xc4,9,0);
    FUN_800c0104(0x8b,0xd6,10,0);
    FUN_80026be8(0x7b,param_3 * 0x12 + 0xc4);
  case 6:
  case 7:
  case 8:
  case 9:
  case 0xc:
    uVar4 = 0;
    iVar6 = 0x38;
    iVar5 = 0x1430;
    do {
      if ((byte)(&DAT_801ff407)[uVar4] < 5) {
        iVar3 = iVar5 + DAT_801ff598;
        if ((*(byte *)(iVar3 + 2) & 3) == 0) {
          uVar2 = 0x18;
        }
        else {
          uVar2 = 0x19;
        }
        uVar2 = FUN_800c00e4(uVar2);
        FUN_8004ee38(&DAT_80026aa4,uVar2,0xc);
        iVar1 = FUN_80026e4c(iVar3);
        uVar2 = FUN_800c00e4(iVar1 + 0x1a);
        FUN_8004ee38(&DAT_80026ab0,uVar2,0x10);
        DAT_80026aad = DAT_80026aad + *(byte *)(iVar3 + 1) / 10;
        DAT_80026aae = *(byte *)(iVar3 + 1) % 10 + DAT_80026aae;
      }
      else {
        uVar2 = FUN_800c00e4(0x10);
        FUN_8004ee38(&DAT_80026aa4,uVar2,0x12);
      }
      FUN_80028ec4(0x29,iVar6,&DAT_80026aa4,0);
      FUN_80026b30();
      iVar6 = iVar6 + 0x14;
      uVar4 = uVar4 + 1;
      iVar5 = iVar5 + 0x80;
    } while (uVar4 < 5);
    FUN_800c0104(0x29,0x9c,param_2 != 0,0);
    if (DAT_801ff404 == '\0') {
      uVar2 = 2;
    }
    else {
      uVar2 = 3;
    }
    FUN_800c0158(0x140,0x18,uVar2,0);
    if (param_1 - 0xcU < 2) {
      return;
    }
    FUN_80026be8(0x19,param_3 * 0x14 + 0x38);
    return;
  }
  FUN_80026be8(uVar2,param_3 * 0x18 + 0x60);
switchD_800266bc_caseD_0:
  return;
}
