/* DsRead @ 0x8008c230  (Ghidra headless, raw MIPS overlay) */

int DsRead(DslLOC *pos,int sectors,u_long *buf,int mode)

{
  undefined1 *puVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  DslLOC *pDVar5;
  uint uVar6;
  undefined1 auStack_10 [8];
  
  if (DAT_800ad324 == 1) {
    iVar4 = DSREAD_OBJ_FC();
    return iVar4;
  }
  DAT_800ad304 = 0x200;
  DAT_800ad308 = buf;
  DAT_800ad30c = sectors;
  if (pos == (DslLOC *)0x0) {
    uVar6 = DAT_800ad324;
    pDVar5 = DsLastPos((DslLOC *)0x0);
    uVar2 = (uint)&pDVar5->track & 3;
    uVar3 = (uint)pDVar5 & 3;
    auStack_10._0_4_ =
         (*(int *)(&pDVar5->track + -uVar2) << (3 - uVar2) * 8 |
         uVar6 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar3) * 8 |
         *(uint *)((int)pDVar5 - uVar3) >> uVar3 * 8;
    puVar1 = auStack_10 + 3;
    uVar2 = (uint)puVar1 & 3;
    *(uint *)(puVar1 + -uVar2) =
         *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 |
         (uint)auStack_10._0_4_ >> (3 - uVar2) * 8;
    iVar4 = DSREAD_OBJ_8C();
    return iVar4;
  }
  uVar2 = (uint)&pos->track & 3;
  uVar3 = (uint)pos & 3;
  auStack_10._0_4_ =
       (*(int *)(&pos->track + -uVar2) << (3 - uVar2) * 8 | 0xffffffffU >> (uVar2 + 1) * 8 & 0x200)
       & -1 << (4 - uVar3) * 8 | *(uint *)((int)pos - uVar3) >> uVar3 * 8;
  puVar1 = auStack_10 + 3;
  uVar2 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar2) =
       *(uint *)(puVar1 + -uVar2) & -1 << (uVar2 + 1) * 8 |
       (uint)auStack_10._0_4_ >> (3 - uVar2) * 8;
  iVar4 = DsPacket((byte)mode | 0x20,(DslLOC *)auStack_10,'\x06',DS_read_cbsync,-1);
  DAT_800ad31c = VSync(-1);
  if ((DAT_800ad310 & 1) != 0) {
    DAT_800ad318 = DsDataCallback(DS_read_cbdata);
  }
  DAT_800ad324 = 1;
  return iVar4;
}


