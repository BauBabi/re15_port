void FUN_800314b0(void)

{
  undefined1 uVar1;
  undefined1 uVar2;
  int iVar3;
  int extraout_v1;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
  DAT_800acac4 = 0x808080;
  _DAT_800aca58 = 0;
  FUN_80013b60(*(undefined2 *)(&DAT_80073f70 + (uint)DAT_800aca5c * 2),&DAT_801bd814,1,&DAT_80010b28
              );
  iVar3 = DAT_801bd814;
  uVar2 = DAT_800aca4d;
  uVar1 = DAT_800aca4c;
  DAT_800acbc0 = (int)&DAT_801bd814 + *(int *)((int)&DAT_801bd814 + DAT_801bd814);
  DAT_800acad5 = 1;
  DAT_800acad4 = 0x17;
  DAT_800acad8 = (int)&DAT_801bd814 + *(int *)(&DAT_801bd818 + DAT_801bd814);
  DAT_800aca4d = 1;
  DAT_800aca4c = 0x17;
  DAT_800aca64 = (int)&DAT_801bd814 + *(int *)(&DAT_801bd81c + DAT_801bd814);
  FUN_8004ee78((int)&DAT_801bd814 + *(int *)(&DAT_801bd820 + DAT_801bd814));
  FUN_80022150(2,DAT_800aca64,0x17,1);
  DAT_800aca6c = DAT_800aca64 + 0x28;
  DAT_800aca4c = uVar1;
  DAT_800aca4d = uVar2;
  DAT_800aca64 = DAT_800aca64 + 0xc;
  FUN_8001e56c(&DAT_800aca54,(int)&DAT_801bd814 + *(int *)(&DAT_801bd820 + iVar3));
  FUN_8001e5b0(&DAT_800aca54,&DAT_801df100);
  DAT_800ac774 = 0x1c3;
  DAT_800acacc = &DAT_80073e94;
  DAT_800acad0 = &DAT_800b2354;
  uVar4 = uRam0000076c;
  uVar5 = uRam00000774;
  uVar6 = uRam00000770;
  uVar7 = uRam00000778;
  DAT_800aca5d = FUN_800c00a8();
  if (extraout_v1 == 0xf) {
    DAT_800aca5d = 0;
  }
  DAT_800b2848 = uVar6;
  DAT_800b2b2c = uVar4;
  DAT_800b2b48 = uVar7;
  DAT_800b521c = uVar5;
  FUN_80037c1c();
  FUN_800c01c4();
  FUN_800440c4(DAT_800aca5c);
  FUN_80036b68();
  FUN_80043d8c(DAT_800aca5d,DAT_800ac77c);
  DAT_800acaee = 100;
  DAT_800acaec = 0;
  return;
}
