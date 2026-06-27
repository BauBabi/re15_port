/* FUN_80019b3c @ 0x80019b3c  (Ghidra headless, raw MIPS overlay) */

void FUN_80019b3c(uint *param_1)

{
  int iVar1;
  MATRIX *r0;
  uint uVar2;
  uint uVar3;
  MATRIX *r0_00;
  undefined4 uVar4;
  uint uVar5;
  undefined4 uVar6;
  MATRIX MStack_58;
  MATRIX MStack_38;
  
  FUN_80076f88(param_1[0x66] + 0x5c);
  SetColorMatrix((MATRIX *)&DAT_8009db84);
  FUN_8002ce94(&DAT_800dcba8,&DAT_8009db44,&MStack_58);
  r0_00 = &MStack_38;
  FUN_8002ce94(&DAT_8009db64,&DAT_8009db44,r0_00);
  iVar1 = FUN_8002c820(param_1 + 0xe,DAT_800ce338);
  if ((iVar1 != 0) && (r0 = &MStack_58, (*param_1 & 0x80000) == 0)) {
    gte_SetRotMatrix(r0);
    gte_SetTransMatrix(r0);
    gte_SetLightMatrix(r0_00);
    uVar2 = (uint)DAT_800ce5e0;
    uVar3 = param_1[0x66];
    uVar6 = *(undefined4 *)(uVar3 + 0x70);
    iVar1 = *(int *)(uVar3 + 0x14);
    uVar4 = *(undefined4 *)(uVar3 + 0x10);
    uVar5 = (*(uint *)(uVar3 + 4) & 2) >> 1;
    FUN_80027bec(*(undefined4 *)(uVar3 + 8),*(int *)(uVar3 + 0xc) + uVar2 * 0x28,uVar6,uVar5);
    FUN_80027dbc(uVar4,iVar1 + uVar2 * 0x34,uVar6,uVar5);
  }
  return;
}


