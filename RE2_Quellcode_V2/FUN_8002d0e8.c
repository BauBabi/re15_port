/* FUN_8002d0e8 @ 0x8002d0e8  (Ghidra headless, raw MIPS overlay) */

void FUN_8002d0e8(int *param_1,uint *param_2)

{
  uint uVar1;
  SVECTOR *r2;
  int iVar2;
  SVECTOR *r0;
  int iVar3;
  int iVar4;
  int iVar5;
  SVECTOR *r1;
  undefined *puVar6;
  uint *r0_00;
  undefined1 local_8 [8];
  
  r0_00 = (uint *)local_8;
  iVar3 = param_1[4];
  iVar5 = *param_1;
  iVar2 = param_1[5];
  puVar6 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  r0 = (SVECTOR *)(iVar5 + *(short *)(iVar3 + 2) * 8);
  r1 = (SVECTOR *)(iVar5 + *(short *)(iVar3 + 6) * 8);
  r2 = (SVECTOR *)(iVar5 + *(short *)(iVar3 + 10) * 8);
  do {
    iVar4 = iVar3;
    if (iVar2 == 0) {
      return;
    }
    while( true ) {
      gte_ldv3(r0,r1,r2);
      iVar3 = iVar4 + 0xc;
      iVar2 = iVar2 + -1;
      gte_rtpt_b();
      r1 = (SVECTOR *)(iVar5 + *(short *)(iVar4 + 0x12) * 8);
      r2 = (SVECTOR *)(iVar5 + *(short *)(iVar4 + 0x16) * 8);
      gte_nclip_b();
      r0 = (SVECTOR *)(iVar5 + *(short *)(iVar4 + 0xe) * 8);
      gte_stopz((long *)r0_00);
      if ((int)*r0_00 < 0) break;
      gte_avsz3_b();
      param_2[1] = ((int *)r0_00)[4];
      gte_stsxy3_gt3(param_2);
      gte_stotz((long *)r0_00);
      if (0x1fff < (int)*r0_00) {
        *r0_00 = 0x1fff;
      }
      uVar1 = *r0_00;
      *r0_00 = *(uint *)(puVar6 + (uVar1 >> 1 & 0xffc));
      *(uint *)(puVar6 + (uVar1 >> 1 & 0xffc)) = (uint)param_2 & 0xffffff;
      *param_2 = *r0_00 & 0xffffff | 0x9000000;
      param_2 = param_2 + 0x14;
      iVar4 = iVar3;
      if (iVar2 == 0) {
        return;
      }
    }
    param_2 = param_2 + 0x14;
  } while( true );
}


