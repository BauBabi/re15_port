void FUN_800166c4(void)

{
  uint uVar1;
  MATRIX *r0;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int *piVar5;
  uint *puVar6;
  MATRIX *r0_00;
  MATRIX *m;
  int iVar7;
  
  DAT_800b8558 = &DAT_801b7000;
  if (DAT_800aca34 != '\0') {
    DAT_800b8558 = &DAT_801b1000;
  }
  piVar5 = (int *)0x0;
  iVar7 = 0;
  puVar6 = (uint *)0xc;
  r0_00 = (MATRIX *)0x48;
  m = (MATRIX *)0x20;
  do {
    RotMatrix((SVECTOR *)(piVar5 + 0x1a),m);
    if (*piVar5 != 0) {
      if (puVar6[0x1a] == 0) {
        r0 = (MATRIX *)&DAT_800b5288;
        gte_SetRotMatrix((MATRIX *)&DAT_800b5288);
        gte_ldsv_((uint)(ushort)m->m[0][0],(uint)(ushort)m->m[1][0],(uint)(ushort)m->m[2][0]);
        gte_rtir();
        uVar2 = gte_stIR1();
        uVar3 = gte_stIR2();
        uVar4 = gte_stIR3();
        r0_00->m[0][0] = (short)uVar2;
        r0_00->m[1][0] = (short)uVar3;
        r0_00->m[2][0] = (short)uVar4;
        gte_ldsv_((uint)*(ushort *)((int)piVar5 + 0x22),(uint)*(ushort *)(piVar5 + 10),
                  (uint)*(ushort *)((int)piVar5 + 0x2e));
        gte_rtir();
        uVar2 = gte_stIR1();
        uVar3 = gte_stIR2();
        uVar4 = gte_stIR3();
        *(short *)((int)piVar5 + 0x4a) = (short)uVar2;
        *(short *)(piVar5 + 0x14) = (short)uVar3;
        *(short *)((int)piVar5 + 0x56) = (short)uVar4;
        gte_ldsv_((uint)*(ushort *)(piVar5 + 9),(uint)*(ushort *)((int)piVar5 + 0x2a),
                  (uint)*(ushort *)(piVar5 + 0xc));
        gte_rtir();
        uVar2 = gte_stIR1();
        uVar3 = gte_stIR2();
        uVar4 = gte_stIR3();
        *(short *)(piVar5 + 0x13) = (short)uVar2;
        *(short *)((int)piVar5 + 0x52) = (short)uVar3;
        *(short *)(piVar5 + 0x16) = (short)uVar4;
        gte_SetTransMatrix(r0);
        gte_ldVXY0(CONCAT22((short)piVar5[0xe],(short)piVar5[0xd]));
        gte_ldVZ0(piVar5 + 0xf);
        gte_rt();
        gte_stlvl((VECTOR *)(piVar5 + 0x17));
      }
      else {
        gte_SetRotMatrix((MATRIX *)puVar6[0x1a]);
        gte_ldsv_((uint)(ushort)m->m[0][0],(uint)(ushort)m->m[1][0],(uint)(ushort)m->m[2][0]);
        gte_rtir();
        uVar2 = gte_stIR1();
        uVar3 = gte_stIR2();
        uVar4 = gte_stIR3();
        r0_00->m[0][0] = (short)uVar2;
        r0_00->m[1][0] = (short)uVar3;
        r0_00->m[2][0] = (short)uVar4;
        gte_ldsv_((uint)*(ushort *)((int)piVar5 + 0x22),(uint)*(ushort *)(piVar5 + 10),
                  (uint)*(ushort *)((int)piVar5 + 0x2e));
        gte_rtir();
        uVar2 = gte_stIR1();
        uVar3 = gte_stIR2();
        uVar4 = gte_stIR3();
        *(short *)((int)piVar5 + 0x4a) = (short)uVar2;
        *(short *)(piVar5 + 0x14) = (short)uVar3;
        *(short *)((int)piVar5 + 0x56) = (short)uVar4;
        gte_ldsv_((uint)*(ushort *)(piVar5 + 9),(uint)*(ushort *)((int)piVar5 + 0x2a),
                  (uint)*(ushort *)(piVar5 + 0xc));
        gte_rtir();
        uVar2 = gte_stIR1();
        uVar3 = gte_stIR2();
        uVar4 = gte_stIR3();
        *(short *)(piVar5 + 0x13) = (short)uVar2;
        *(short *)((int)piVar5 + 0x52) = (short)uVar3;
        *(short *)(piVar5 + 0x16) = (short)uVar4;
        gte_SetTransMatrix((MATRIX *)puVar6[0x1a]);
        gte_ldVXY0(CONCAT22((short)piVar5[0xe],(short)piVar5[0xd]));
        gte_ldVZ0(piVar5 + 0xf);
        gte_rt();
        gte_stlvl((VECTOR *)(piVar5 + 0x17));
      }
      if ((*puVar6 & 0x10) != 0) {
        gte_ldL11L12(*(undefined4 *)r0_00->m[0]);
        gte_ldL13L21(*(undefined4 *)(r0_00->m[0] + 2));
        gte_ldL22L23(*(undefined4 *)(r0_00->m[1] + 1));
        gte_ldL31L32(*(undefined4 *)r0_00->m[2]);
        gte_ldL33(*(undefined4 *)(r0_00->m[2] + 2));
      }
      if ((puVar6[0x20] & 0x200) != 0) {
        *(char *)((int)puVar6 + 0x83) = *(char *)((int)puVar6 + 0x83) + '\x04';
        uVar1 = (uint)*(byte *)((int)puVar6 + 0x83);
        puVar6[0x19] = uVar1 << 0x10 | uVar1 << 8 | uVar1;
      }
      gte_SetRotMatrix(r0_00);
      gte_SetTransMatrix(r0_00);
      *puVar6 = *puVar6 & 0xfffff1ff;
      FUN_80016b54(puVar6,piVar5);
    }
    iVar7 = iVar7 + 1;
    puVar6 = puVar6 + 0x24;
    r0_00 = (MATRIX *)(r0_00[4].m[2] + 2);
    m = (MATRIX *)(m[4].m[2] + 2);
    piVar5 = piVar5 + 0x24;
  } while (iVar7 < 4);
  return;
}
