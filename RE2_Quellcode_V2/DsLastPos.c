/* DsLastPos @ 0x8008c0d8  (Ghidra headless, raw MIPS overlay) */

DslLOC * DsLastPos(DslLOC *p)

{
  uint uVar1;
  u_char *puVar2;
  DslLOC *pDVar3;
  uint uVar4;
  uint uVar5;
  undefined8 uVar6;
  
  if (p == (DslLOC *)0x0) {
    DS_lastpos();
    pDVar3 = (DslLOC *)DSSYS_3_OBJ_260();
    return pDVar3;
  }
  uVar6 = DS_lastpos();
  uVar4 = (uint)uVar6;
  uVar1 = uVar4 + 3 & 3;
  uVar5 = uVar4 & 3;
  uVar5 = (*(int *)((uVar4 + 3) - uVar1) << (3 - uVar1) * 8 |
          (uint)((ulonglong)uVar6 >> 0x20) & 0xffffffffU >> (uVar1 + 1) * 8) & -1 << (4 - uVar5) * 8
          | *(uint *)(uVar4 - uVar5) >> uVar5 * 8;
  uVar1 = (uint)&p->track & 3;
  puVar2 = &p->track + -uVar1;
  *(uint *)puVar2 = *(uint *)puVar2 & -1 << (uVar1 + 1) * 8 | uVar5 >> (3 - uVar1) * 8;
  uVar1 = (uint)p & 3;
  *(uint *)((int)p - uVar1) =
       *(uint *)((int)p - uVar1) & 0xffffffffU >> (4 - uVar1) * 8 | uVar5 << uVar1 * 8;
  return p;
}


