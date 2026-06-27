void FUN_800161e0(void)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  undefined2 *puVar4;
  undefined *puVar5;
  undefined4 *puVar6;
  int iVar7;
  
  iVar7 = 3;
  puVar6 = &DAT_800b2400;
  puVar5 = &DAT_801bd1b0;
  DAT_800b5456 = 0;
  DAT_800aca38 = DAT_800aca38 | 0x2000000;
  do {
    *puVar6 = puVar5;
    puVar6 = puVar6 + -1;
    iVar7 = iVar7 + -1;
    puVar5 = puVar5 + -0x90;
  } while (-1 < iVar7);
  puVar4 = (undefined2 *)0x0;
  do {
    *puVar4 = 0;
    puVar4 = puVar4 + 1;
  } while (puVar4 <= (undefined2 *)(DAT_800b2400 + 0x8c));
                    /* WARNING: Read-only address (ram,0x801a1000) is written */
  puVar4 = &DAT_800b220c;
  do {
    *puVar4 = 0;
    puVar4 = puVar4 + 1;
  } while (puVar4 < &UNK_800b223b);
  DAT_801a1004 = (int)&DAT_801a1000 + DAT_801a1004;
  DAT_801a1008 = (int)&DAT_801a1000 + DAT_801a1008;
  DAT_800b5364 = (int *)((int)&DAT_801a1000 + DAT_801a1000);
  FUN_800170e0(DAT_801a1008 - DAT_801a1004);
  *DAT_800b5364 = (int)(&UNK_801a100c + *DAT_800b5364);
  piVar3 = DAT_800b5364;
  DAT_800b5364[1] = (int)(&UNK_801a100c + DAT_800b5364[1]);
  piVar1 = DAT_800b5364 + 2;
  piVar2 = DAT_800b5364 + 2;
  DAT_800b5364 = DAT_800b5364 + 2;
  *piVar2 = (int)(&UNK_801a100c + *piVar1);
  _DAT_800aca4c = 0x1f15;
  FUN_8004ee78(piVar3[2]);
  DAT_800b8550 = &DAT_801ab000;
  DAT_800b8554 = DAT_800b5364[-2];
  DAT_800b5364 = DAT_800b5364 + -2;
  FUN_80022150(2,DAT_800b8554,0,0);
  DAT_800b8554 = DAT_800b8554 + 0xc;
  piVar1 = DAT_800b5364 + 2;
  piVar2 = DAT_800b5364 + 1;
  DAT_800b5364 = DAT_800b5364 + 1;
  FUN_800171b4(*piVar1 - *piVar2);
  DAT_800b3f70 = *DAT_800b5364;
  FUN_8003edec(&DAT_800b39ac,0);
  DAT_800b855c = 2;
  DAT_800b8560 = 0x140;
  DAT_800b8564 = 0xf0;
  FUN_80021764(0);
  FUN_80021634(2,0);
  InitGeom();
  FUN_80066d60(0xa0,0x78);
  SetBackColor(0xff,0xff,0xff);
  DAT_800b2210 = 30000;
  DAT_800b2214 = 0;
  DAT_800b2218 = 0;
  DAT_800b221c = 22000;
  DAT_800b2220 = 0;
  DAT_800b2224 = 0;
  FUN_80053ca4(&DAT_800b220c);
  DAT_800b0fde = (ushort)*(byte *)(DAT_800ac9a8 + 0xd);
  return;
}
