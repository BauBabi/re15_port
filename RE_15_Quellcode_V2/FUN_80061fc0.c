uint FUN_80061fc0(int param_1)

{
  dword dVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  undefined8 uVar5;
  
  uVar5 = FUN_8007d904();
  uVar4 = (int)uVar5 - (int)((ulonglong)uVar5 >> 0x20) & 0xffff;
  uVar2 = DAT_800787dc;
  if ((-1 < param_1) && (uVar2 = uVar4, param_1 != 1)) {
    uVar4 = DAT_800787f0;
    if (0 < param_1) {
      uVar4 = (DAT_800787f0 - 1) + param_1;
    }
    iVar3 = 0;
    if (0 < param_1) {
      iVar3 = param_1 + -1;
    }
    FUN_80062108(uVar4,iVar3);
    dVar1 = GPU_REG1;
    FUN_80062108(DAT_800787dc + 1,1);
    if (((dVar1 & 0x80000) != 0) && (-1 < (int)(dVar1 ^ GPU_REG1))) {
      do {
      } while (((dVar1 ^ GPU_REG1) & 0x80000000) == 0);
    }
    DAT_800787f0 = DAT_800787dc;
    DAT_800787ec = TMR_HRETRACE_VAL;
  }
  return uVar2;
}
