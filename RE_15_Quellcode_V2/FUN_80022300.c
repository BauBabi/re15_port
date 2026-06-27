int FUN_80022300(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  undefined1 uVar1;
  undefined1 uVar2;
  int iVar3;
  uint uVar4;
  int *piVar5;
  
  uVar4 = *(byte *)(param_3 + 8) - 0x10;
  if (DAT_800aca3c < 0) {
    uVar4 = uVar4 & 0xff;
    iVar3 = uVar4 * 8;
    DAT_800be572 = 0x27;
    DAT_800be574 = *(undefined4 *)(&DAT_8007317c + iVar3);
    DAT_800be57c = *(int *)(&DAT_80073178 + iVar3);
    DAT_8006f57b = (&DAT_80073378)[uVar4];
  }
  else {
    uVar4 = uVar4 & 0xff;
    iVar3 = uVar4 * 8;
    DAT_800be572 = 0x26;
    DAT_800be574 = *(undefined4 *)(&DAT_80072f3c + iVar3);
    DAT_800be57c = *(int *)(&DAT_80072f38 + iVar3);
    DAT_8006f573 = (&DAT_80073138)[uVar4];
  }
  DAT_800be57c = (uint)(byte)(&DAT_8006f442)[(uint)DAT_800be572 * 8] * 0x10000 +
                 (uint)*(ushort *)(&DAT_8006f440 + (uint)DAT_800be572 * 8) + DAT_800be57c;
  FUN_80013c50(param_4,1,&DAT_8001070c);
  piVar5 = (int *)((int)param_4 + *param_4);
  *(int *)(param_3 + 0x1b0) = (int)param_4 + *piVar5;
  *(int *)(param_3 + 0x16c) = (int)param_4 + piVar5[1];
  *(int *)(param_3 + 0x84) = (int)param_4 + piVar5[2];
  *(int *)(param_3 + 0x174) = (int)param_4 + piVar5[3];
  *(int *)(param_3 + 0x170) = (int)param_4 + piVar5[4];
  *(int *)(param_3 + 0x17c) = (int)param_4 + piVar5[5];
  *(int *)(param_3 + 0x178) = (int)param_4 + piVar5[6];
  iVar3 = piVar5[7];
  *(int **)(param_3 + 0x10) = (int *)((int)param_4 + iVar3);
  *(int *)((int)param_4 + iVar3) = piVar5[8] - piVar5[7];
  *(undefined4 *)(param_3 + 0x70) = 0x808080;
  if (*(char *)(param_3 + 0x97) == '\x01') {
    FUN_800104b0((int)param_4 + piVar5[3],(int)param_4 + piVar5[7],**(undefined4 **)(param_3 + 0x10)
                );
    *(int *)(param_3 + 0x10) = (int)param_4 + piVar5[3];
  }
  if (*(char *)(param_3 + 0x97) == '\x02') {
    FUN_800104b0((int)param_4 + piVar5[1],(int)param_4 + piVar5[3],piVar5[8] - piVar5[3]);
    iVar3 = piVar5[3] - piVar5[1];
    if (iVar3 < 0) {
      iVar3 = iVar3 + 3;
    }
    iVar3 = iVar3 >> 2;
    *(int *)(param_3 + 0x10) = *(int *)(param_3 + 0x10) + iVar3 * -4;
    *(int *)(param_3 + 0x170) = *(int *)(param_3 + 0x170) + iVar3 * -4;
    *(int *)(param_3 + 0x174) = *(int *)(param_3 + 0x174) + iVar3 * -4;
    *(int *)(param_3 + 0x178) = *(int *)(param_3 + 0x178) + iVar3 * -4;
    *(int *)(param_3 + 0x17c) = *(int *)(param_3 + 0x17c) + iVar3 * -4;
    *(undefined4 *)(param_3 + 0x16c) = DAT_800acbc0;
    *(undefined4 *)(param_3 + 0x84) = DAT_800acad8;
  }
  uVar2 = DAT_800aca4d;
  *(undefined1 *)(param_3 + 0x81) = DAT_800aca4d;
  uVar1 = DAT_800aca4c;
  *(undefined1 *)(param_3 + 0x80) = DAT_800aca4c;
  FUN_8004ee78((int)param_4 + piVar5[8]);
  FUN_80029ac8(1);
  uVar4 = FUN_80022150(2,*(undefined4 *)(param_3 + 0x10),uVar1,uVar2);
  iVar3 = *(int *)(param_3 + 0x10);
  *(int *)(param_3 + 0x10) = iVar3 + 0xc;
  *(int *)(param_3 + 0x18) = iVar3 + 0x28;
  return ((uVar4 & 0xfffffffc) - 0xc) + *(int *)(param_3 + 0x10);
}
