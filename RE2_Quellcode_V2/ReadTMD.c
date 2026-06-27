/* ReadTMD @ 0x8009302c  (Ghidra headless, raw MIPS overlay) */

TMD_PRIM * ReadTMD(TMD_PRIM *tmdprim)

{
  ushort uVar1;
  ushort uVar2;
  SVECTOR *pSVar3;
  SVECTOR *pSVar4;
  int iVar5;
  SVECTOR *pSVar6;
  TMD_PRIM *pTVar7;
  
  iVar5 = unpack_packet(DAT_800c406c,tmdprim);
  pSVar4 = DAT_800c4068;
  pSVar3 = DAT_800c4064;
  pTVar7 = (TMD_PRIM *)0x0;
  if (-1 < iVar5) {
    pSVar6 = DAT_800c4068 + tmdprim->norm0;
    tmdprim->v_ofs = DAT_800c4064;
    tmdprim->n_ofs = pSVar4;
    uVar1 = tmdprim->norm0;
    (tmdprim->n0).vx = pSVar6->vx;
    uVar2 = tmdprim->norm0;
    (tmdprim->n0).vy = pSVar4[uVar1].vy;
    uVar1 = tmdprim->norm1;
    (tmdprim->n0).vz = pSVar4[uVar2].vz;
    uVar2 = tmdprim->norm1;
    (tmdprim->n1).vx = pSVar4[uVar1].vx;
    uVar1 = tmdprim->norm1;
    (tmdprim->n1).vy = pSVar4[uVar2].vy;
    uVar2 = tmdprim->norm2;
    (tmdprim->n1).vz = pSVar4[uVar1].vz;
    uVar1 = tmdprim->norm2;
    (tmdprim->n2).vx = pSVar4[uVar2].vx;
    uVar2 = tmdprim->norm2;
    (tmdprim->n2).vy = pSVar4[uVar1].vy;
    uVar1 = tmdprim->norm3;
    (tmdprim->n2).vz = pSVar4[uVar2].vz;
    DAT_800c406c = DAT_800c406c + iVar5;
    (tmdprim->n3).vx = pSVar4[uVar1].vx;
    uVar1 = tmdprim->norm3;
    (tmdprim->n3).vy = pSVar4[tmdprim->norm3].vy;
    uVar2 = tmdprim->vert0;
    (tmdprim->n3).vz = pSVar4[uVar1].vz;
    uVar1 = tmdprim->vert0;
    (tmdprim->x0).vx = pSVar3[uVar2].vx;
    uVar2 = tmdprim->vert0;
    (tmdprim->x0).vy = pSVar3[uVar1].vy;
    uVar1 = tmdprim->vert1;
    (tmdprim->x0).vz = pSVar3[uVar2].vz;
    uVar2 = tmdprim->vert1;
    (tmdprim->x1).vx = pSVar3[uVar1].vx;
    uVar1 = tmdprim->vert1;
    (tmdprim->x1).vy = pSVar3[uVar2].vy;
    uVar2 = tmdprim->vert2;
    (tmdprim->x1).vz = pSVar3[uVar1].vz;
    uVar1 = tmdprim->vert2;
    (tmdprim->x2).vx = pSVar3[uVar2].vx;
    uVar2 = tmdprim->vert2;
    (tmdprim->x2).vy = pSVar3[uVar1].vy;
    uVar1 = tmdprim->vert3;
    (tmdprim->x2).vz = pSVar3[uVar2].vz;
    (tmdprim->x3).vx = pSVar3[uVar1].vx;
    uVar1 = tmdprim->vert3;
    (tmdprim->x3).vy = pSVar3[tmdprim->vert3].vy;
    (tmdprim->x3).vz = pSVar3[uVar1].vz;
    pTVar7 = tmdprim;
  }
  return pTVar7;
}


