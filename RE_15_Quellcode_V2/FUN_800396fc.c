void FUN_800396fc(void)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int unaff_s1;
  
  DAT_800afbb7 = 0;
  DAT_800aca3c = DAT_800aca3c & 0xffff0000;
  DAT_800ac77c = DAT_800ac780;
  DAT_800ac778 = DAT_800ac780;
  DAT_800bbeb0 = DAT_800ac780;
  _DAT_800aca4c = 0xa05;
  if ((DAT_800aca5c & 0xf) != 0) {
    DAT_800aca5c = DAT_800aca5c & 0xf0;
    FUN_800314b0();
  }
  FUN_800443ec();
  FUN_80013b60((uint)*(ushort *)((&PTR_DAT_8007438c)[DAT_800b0fe0] + _DAT_800b0fe2 * 2) -
               ((int)DAT_800aca3c >> 0x1f),DAT_800ac778,1,&DAT_80010c20);
  piVar1 = (int *)(DAT_800ac778 + 8);
  do {
    if (*piVar1 != 0) {
      *piVar1 = *piVar1 + DAT_800ac778;
    }
    piVar1 = piVar1 + 1;
  } while (piVar1 < (int *)(DAT_800ac778 + 0x60));
  uVar3 = 0;
  if (*(char *)(DAT_800ac778 + 1) != '\0') {
    iVar2 = 0;
    do {
      iVar2 = iVar2 + *(int *)(DAT_800ac778 + 0x24);
      *(int *)(iVar2 + 0x1c) = *(int *)(iVar2 + 0x1c) + DAT_800ac778;
      uVar3 = uVar3 + 1;
      iVar2 = uVar3 * 0x20;
    } while (uVar3 < *(byte *)(DAT_800ac778 + 1));
  }
  FUN_8003aea0();
  DAT_800ac77c = unaff_s1 + DAT_800ac77c;
  uVar3 = 0;
  if (*(char *)(DAT_800ac778 + 2) != '\0') {
    iVar2 = 0;
    do {
      piVar1 = (int *)(iVar2 + *(int *)(DAT_800ac778 + 0x30));
      *piVar1 = *piVar1 + DAT_800ac778;
      iVar2 = iVar2 + *(int *)(DAT_800ac778 + 0x30);
      *(int *)(iVar2 + 4) = *(int *)(iVar2 + 4) + DAT_800ac778;
      uVar3 = uVar3 + 1;
      iVar2 = uVar3 * 8;
    } while (uVar3 < *(byte *)(DAT_800ac778 + 2));
  }
  FUN_800142f4((int)DAT_800b0fe4);
  DAT_800bbeb0 = *(int *)(DAT_800ac778 + 0x10) - DAT_800bbeb0;
  FUN_80019354();
  FUN_80043eac();
  FUN_80043fb0();
  DAT_800ac77c = *(int *)(DAT_800ac778 + 0x10);
  FUN_8003ea7c();
  FUN_80039c2c();
  FUN_80044210();
  DAT_800bbeb4 = DAT_800ac77c;
  FUN_80039270();
  DAT_800bbeb8 = 0;
  DAT_800bbebc = 0;
  DAT_800bbeb4 = DAT_800ac77c - DAT_800bbeb4;
  FUN_8003ef6c();
  FUN_8001b3f8();
  FUN_8001a504();
  return;
}
