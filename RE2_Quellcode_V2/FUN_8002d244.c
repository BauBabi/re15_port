/* FUN_8002d244 @ 0x8002d244  (Ghidra headless, raw MIPS overlay) */

void FUN_8002d244(int *param_1,uint *param_2)

{
  uint uVar1;
  SVECTOR *r0;
  SVECTOR *r2;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  SVECTOR *r1;
  SVECTOR *r0_00;
  undefined *puVar6;
  uint *r0_01;
  undefined1 local_8 [8];
  
  r0_01 = (uint *)local_8;
  iVar4 = param_1[4];
  iVar2 = *param_1;
  iVar3 = param_1[5];
  puVar6 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  r0_00 = (SVECTOR *)(iVar2 + *(short *)(iVar4 + 2) * 8);
  r1 = (SVECTOR *)(iVar2 + *(short *)(iVar4 + 6) * 8);
  r2 = (SVECTOR *)(iVar2 + *(short *)(iVar4 + 10) * 8);
  do {
    iVar5 = iVar4;
    if (iVar3 == 0) {
      return;
    }
    while( true ) {
      gte_ldv3(r0_00,r1,r2);
      iVar4 = iVar5 + 0x10;
      iVar3 = iVar3 + -1;
      gte_rtpt_b();
      r0_00 = (SVECTOR *)(iVar2 + *(short *)(iVar5 + 0x12) * 8);
      r1 = (SVECTOR *)(iVar2 + *(short *)(iVar5 + 0x16) * 8);
      r2 = (SVECTOR *)(iVar2 + *(short *)(iVar5 + 0x1a) * 8);
      gte_nclip_b();
      r0 = (SVECTOR *)(iVar2 + *(short *)(iVar5 + 0xe) * 8);
      gte_stopz((long *)r0_01);
      if ((int)*r0_01 < 0) break;
      gte_stsxy3_gt3(param_2);
      gte_ldv0(r0);
      gte_rtps();
      param_2[1] = *(uint *)((int)r0_01 + 0x10);
      gte_avsz4_b();
      gte_stsxy((long *)(param_2 + 0xb));
      gte_stotz((long *)r0_01);
      if (0x1fff < (int)*r0_01) {
        *r0_01 = 0x1fff;
      }
      uVar1 = *r0_01;
      *r0_01 = *(uint *)(puVar6 + (uVar1 >> 1 & 0xffc));
      *(uint *)(puVar6 + (uVar1 >> 1 & 0xffc)) = (uint)param_2 & 0xffffff;
      *param_2 = *r0_01 & 0xffffff | 0xc000000;
      param_2 = param_2 + 0x1a;
      iVar5 = iVar4;
      if (iVar3 == 0) {
        return;
      }
    }
    param_2 = param_2 + 0x1a;
  } while( true );
}


