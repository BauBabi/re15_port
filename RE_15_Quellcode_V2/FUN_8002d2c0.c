undefined4 FUN_8002d2c0(undefined4 param_1)

{
  int iVar1;
  uint *puVar2;
  int iVar3;
  int iVar4;
  
  if (((DAT_800aca3c & 0x4000) == 0) && (iVar3 = *(byte *)(DAT_800ac778 + 2) - 1, -1 < iVar3)) {
    iVar4 = iVar3 * 0x94;
    do {
      puVar2 = (uint *)((int)&DAT_800b3f98 + iVar4);
      if (((((*puVar2 & 0x101) != 0) && (DAT_800acad6 == (&DAT_800b401a)[iVar4])) &&
          (iVar1 = FUN_8002d1e8(&DAT_800aca54,puVar2,0), iVar1 != 0)) &&
         (iVar1 = FUN_8002da4c(param_1,puVar2,0xffffff90,(&DAT_800b401a)[iVar4]), iVar1 != 0)) {
        DAT_800ac78c = puVar2;
        return 1;
      }
      iVar3 = iVar3 + -1;
      iVar4 = iVar4 + -0x94;
    } while (-1 < iVar3);
  }
  return 0;
}
