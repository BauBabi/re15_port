/* FUN_8003b264 @ 0x8003b264  (Ghidra headless, raw MIPS overlay) */

void FUN_8003b264(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint unaff_s1;
  
  iVar4 = DAT_800ea23c;
  iVar1 = FUN_80039ed4();
  iVar2 = DAT_800ce32c;
  if (iVar1 != 0) goto LAB_8003b76c;
  if (DAT_800a3e90 == '\0') {
    if ((DAT_800a3e8c != 0) && ((DAT_800ce30c & 0x1000) != 0)) {
      DAT_800a3e88 = 1;
    }
  }
  else if ((DAT_800a3e90 == '\x01') && (DAT_800a3e8c != 0)) {
    if ((DAT_800ce306 & 0xf0) != 0) {
      DAT_800a3e88 = 1;
    }
    if ((DAT_800ce306 & 0xf) != 0) {
      DAT_800a3e88 = 1;
    }
  }
  if (DAT_800a3e88 != 0) {
    DAT_800c3b64 = 0;
    DAT_800c3b60 = 0;
  }
  unaff_s1 = (uint)DAT_800d4235;
  switch(unaff_s1) {
  case 0:
    DAT_800ea23c = DAT_800ce32c;
    if ((DAT_800cfbd8 & 0x200) == 0) {
      DAT_800ce32c = DAT_800ce32c + 0x53e0;
      FUN_8003b7e8(DAT_800cfb7c >> 4,1);
    }
    else {
      DAT_800ce32c = DAT_800ce32c + 0x53e0;
      FUN_8003b7e8(DAT_800cfb7c >> 4,0);
      FUN_8003a298(0,"Register your fingerprint.]",&DAT_800a3e8c);
    }
    DAT_800a3e8c = 0;
    DAT_800a3e88 = 0;
    iVar4 = iVar2;
    if ((DAT_800cfbd8 & 0x200) == 0) {
      DAT_800d4235 = 1;
      goto LAB_8003b43c;
    }
    DAT_800d4235 = 8;
    break;
  case 1:
LAB_8003b43c:
    FUN_8003a298(1,"UMBRELLA \"RODEM\" SYSTEM]                      Ver 5.0]",&DAT_800a3e8c);
    if (DAT_800a3e8c != 0) {
      DAT_800d4235 = 2;
    }
    break;
  case 2:
    FUN_8003a298(1,"Culture Experiment Room Staff Registry.]]",&DAT_800a3e8c);
    if ((DAT_800a3e8c & DAT_800a3e88) != 0) {
      DAT_800d4235 = 3;
    }
    break;
  case 3:
    FUN_8003a298(1,"Accessing[020.[020.[020.[020.[020.]]",&DAT_800a3e8c);
    uVar3 = DAT_800a3e8c;
    goto LAB_8003b5cc;
  case 4:
    FUN_8003a298(1,"Enter your user name.]>",&DAT_800a3e8c);
    if ((DAT_800a3e8c & DAT_800a3e88) != 0) {
      DAT_800d4235 = 5;
      DAT_800d4236 = 1;
    }
    break;
  case 5:
    iVar2 = FUN_800398ac(8,0);
    if (iVar2 != 0) {
      DAT_800d4236 = 2;
      iVar2 = strcmp((char *)(iVar4 + 0x2e4),"GUEST");
      if (iVar2 == 0) {
        DAT_800d4235 = 7;
        FUN_8003a668();
      }
      else {
        DAT_800d4235 = 6;
      }
    }
    break;
  case 6:
    FUN_8003a298(1,"|1This name is not in the]researcher data base.|0]",&DAT_800a3e8c);
    uVar3 = DAT_800a3e8c & DAT_800a3e88;
LAB_8003b5cc:
    if (uVar3 != 0) {
      DAT_800d4235 = 4;
    }
    break;
  case 7:
    FUN_8003a298(1,"Register your fingerprint.",&DAT_800a3e8c);
    if ((DAT_800a3e8c & DAT_800a3e88) != 0) {
      DAT_800cfbd8 = DAT_800cfbd8 | 0x200;
      FUN_8007730c(&DAT_800d4854,0x5b);
      DAT_800d4235 = 0xc;
    }
    break;
  case 8:
    FUN_8003a298(1,"Please wait[025.[025.[025.[025.[025.]",&DAT_800a3e8c);
    if (DAT_800a3e8c != 0) {
      DAT_800d4235 = 9;
    }
    break;
  case 9:
    FUN_8003a298(1,"Registration complete.]]",&DAT_800a3e8c);
    if ((DAT_800a3e8c & DAT_800a3e88) != 0) {
      DAT_800d4235 = 10;
    }
    break;
  case 10:
    FUN_8003a298(1,"Guest registration is valid]for 24 hours.]",&DAT_800a3e8c);
    if ((DAT_800a3e8c & DAT_800a3e88) != 0) {
      DAT_800cfbd8 = DAT_800cfbd8 & 0xfffffdff;
      FUN_8003b918(1);
      return;
    }
    break;
  case 0xb:
    FUN_8003b918(1);
    return;
  case 0xc:
    FUN_8003b918(0);
    return;
  }
  if (*(char *)(iVar4 + 0x2d9) == '\x01') {
    *(undefined1 *)(iVar4 + 0x2d9) = 0;
    DAT_800d4235 = 0xb;
  }
LAB_8003b76c:
  if (((DAT_800a3e88 & DAT_800a3e8c) != 0) || (unaff_s1 != DAT_800d4235)) {
    DAT_800a3e8c = 0;
    DAT_800a3e88 = 0;
    DAT_800c3b64 = 0;
    DAT_800c3b60 = 0;
  }
  FUN_8003a6ac(1);
  FUN_8003a794(0x16c);
  return;
}


