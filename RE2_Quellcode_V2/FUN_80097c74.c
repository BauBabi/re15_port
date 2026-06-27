/* FUN_80097c74 @ 0x80097c74  (Ghidra headless, raw MIPS overlay) */

void FUN_80097c74(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined *puVar3;
  undefined *puVar4;
  undefined *puVar5;
  int iVar6;
  undefined *puVar7;
  
  DAT_800c39d4 = 0;
  DAT_800c39e8 = 0;
  FUN_800982d4();
  puVar7 = &DAT_800c4168;
  DAT_800c399c = &LAB_80097e3c;
  DAT_800c39a0 = &LAB_80097dd4;
  DAT_800c39a4 = &LAB_80097f40;
  DAT_800c39a8 = &LAB_80097ffc;
  DAT_800c39ac = &LAB_80098278;
  DAT_800c39b0 = &LAB_800982b0;
  DAT_800c39d0 = &DAT_800c4168;
  DAT_800c39c0 = FUN_80097f30;
  bzero("",0x1e0);
  iVar6 = 0;
  puVar2 = &DAT_800c41a8;
  puVar5 = &DAT_800c4120;
  puVar4 = &DAT_800c40d8;
  DAT_800c4198 = param_1;
  DAT_800c4288 = param_2;
  do {
    puVar3 = puVar7 + 0x5d;
    puVar2[-0xd] = 0;
    puVar2[-0xc] = puVar7;
    *(undefined1 *)puVar2[-4] = 0xff;
    iVar1 = 5;
    *(undefined1 *)(puVar2[-4] + 1) = 0;
    puVar2[-1] = puVar4;
    *puVar2 = puVar5;
    do {
      *puVar3 = 0xff;
      iVar1 = iVar1 + -1;
      puVar3 = puVar3 + 1;
    } while (-1 < iVar1);
    puVar5 = puVar5 + 0x23;
    puVar4 = puVar4 + 0x23;
    iVar6 = iVar6 + 1;
    puVar2 = puVar2 + 0x3c;
    puVar7 = puVar7 + 0xf0;
  } while (iVar6 < 2);
  FUN_8009624c();
  DAT_800c39d4 = 1;
  return;
}


