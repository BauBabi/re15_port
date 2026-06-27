TMD_PRIM * ReadTMD(TMD_PRIM *tmdprim)

{
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  TMD_PRIM *pTVar4;
  
  iVar3 = unpack_packet(DAT_8008ff74,tmdprim);
  pTVar4 = (TMD_PRIM *)0x0;
  if (-1 < iVar3) {
    tmdprim->v_ofs = (SVECTOR *)0x0;
    tmdprim->n_ofs = (SVECTOR *)0x0;
    uVar1 = tmdprim->norm0;
    (tmdprim->n0).vx = *(short *)((uint)tmdprim->norm0 * 8);
    uVar2 = tmdprim->norm0;
    (tmdprim->n0).vy = *(short *)((uint)uVar1 * 8 + 2);
    uVar1 = tmdprim->norm1;
    (tmdprim->n0).vz = *(short *)((uint)uVar2 * 8 + 4);
    uVar2 = tmdprim->norm1;
    (tmdprim->n1).vx = *(short *)((uint)uVar1 * 8);
    uVar1 = tmdprim->norm1;
    (tmdprim->n1).vy = *(short *)((uint)uVar2 * 8 + 2);
    uVar2 = tmdprim->norm2;
    (tmdprim->n1).vz = *(short *)((uint)uVar1 * 8 + 4);
    uVar1 = tmdprim->norm2;
    (tmdprim->n2).vx = *(short *)((uint)uVar2 * 8);
    uVar2 = tmdprim->norm2;
    (tmdprim->n2).vy = *(short *)((uint)uVar1 * 8 + 2);
    uVar1 = tmdprim->norm3;
    (tmdprim->n2).vz = *(short *)((uint)uVar2 * 8 + 4);
    DAT_8008ff74 = iVar3 + DAT_8008ff74;
    (tmdprim->n3).vx = *(short *)((uint)uVar1 * 8);
    uVar1 = tmdprim->norm3;
    (tmdprim->n3).vy = *(short *)((uint)tmdprim->norm3 * 8 + 2);
    uVar2 = tmdprim->vert0;
    (tmdprim->n3).vz = *(short *)((uint)uVar1 * 8 + 4);
    uVar1 = tmdprim->vert0;
    (tmdprim->x0).vx = *(short *)((uint)uVar2 * 8);
    uVar2 = tmdprim->vert0;
    (tmdprim->x0).vy = *(short *)((uint)uVar1 * 8 + 2);
    uVar1 = tmdprim->vert1;
    (tmdprim->x0).vz = *(short *)((uint)uVar2 * 8 + 4);
    uVar2 = tmdprim->vert1;
    (tmdprim->x1).vx = *(short *)((uint)uVar1 * 8);
    uVar1 = tmdprim->vert1;
    (tmdprim->x1).vy = *(short *)((uint)uVar2 * 8 + 2);
    uVar2 = tmdprim->vert2;
    (tmdprim->x1).vz = *(short *)((uint)uVar1 * 8 + 4);
    uVar1 = tmdprim->vert2;
    (tmdprim->x2).vx = *(short *)((uint)uVar2 * 8);
    uVar2 = tmdprim->vert2;
    (tmdprim->x2).vy = *(short *)((uint)uVar1 * 8 + 2);
    uVar1 = tmdprim->vert3;
    (tmdprim->x2).vz = *(short *)((uint)uVar2 * 8 + 4);
    (tmdprim->x3).vx = *(short *)((uint)uVar1 * 8);
    uVar1 = tmdprim->vert3;
    (tmdprim->x3).vy = *(short *)((uint)tmdprim->vert3 * 8 + 2);
    (tmdprim->x3).vz = *(short *)((uint)uVar1 * 8 + 4);
    pTVar4 = tmdprim;
  }
  return pTVar4;
}
