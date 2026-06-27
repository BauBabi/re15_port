/* FUN_800129b4 @ 0x800129b4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_800129b4(int param_1,int param_2)

{
  undefined1 *puVar1;
  int iVar2;
  uint uVar3;
  undefined *puVar4;
  ushort *puVar5;
  undefined1 local_28 [4];
  undefined1 auStack_24 [4];
  undefined1 auStack_20 [4];
  undefined1 auStack_1c [4];
  ushort local_18;
  
  puVar1 = local_28 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80010818 >> (3 - uVar3) * 8;
  local_28 = (undefined1  [4])DAT_80010818;
  puVar1 = auStack_24 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_8001081c >> (3 - uVar3) * 8;
  auStack_24 = (undefined1  [4])DAT_8001081c;
  puVar1 = auStack_20 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80010820 >> (3 - uVar3) * 8;
  auStack_20 = (undefined1  [4])DAT_80010820;
  puVar1 = auStack_1c + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_80010824 >> (3 - uVar3) * 8;
  auStack_1c = (undefined1  [4])DAT_80010824;
  local_18 = DAT_80010828;
  if (param_1 == 0) {
    uVar3 = (uint)*(ushort *)(local_28 + DAT_800d481c * 2 + 2);
    puVar4 = _DAT_800ce544;
    if (DAT_800d481c == 7) {
      puVar4 = &DAT_8009a3cc;
      uVar3 = (uint)DAT_80010828;
    }
  }
  else {
    uVar3 = DAT_80010818 & 0xffff;
    puVar4 = &DAT_8009a37c;
  }
  puVar5 = (ushort *)(puVar4 + param_2 * 6);
  DAT_800d5b48 = 1;
  DAT_800d5b49 = (undefined1)*puVar5;
  iVar2 = (uint)(byte)(&DAT_800988aa)[uVar3 * 8] * 0x10000 +
          (uint)*(ushort *)(&DAT_800988a8 + uVar3 * 8);
  DAT_800d5324 = iVar2 + (uint)puVar5[1] * 0x10 + (uint)*puVar5;
  DAT_800d532c = iVar2 + -0x1e + ((uint)puVar5[1] + (uint)puVar5[2]) * 0x10 + (uint)*puVar5;
  DAT_800d5328 = DAT_800d5324;
  DsIntToPos(DAT_800d5324,(DslLOC *)&DAT_800d5304);
  SsSetSerialAttr('\0','\0','\x01');
  DAT_800d5301 = 1;
  DAT_800d5335 = 0;
  DAT_800d5303 = 0;
  DAT_800d5337 = 0;
  DAT_800d5334 = 1;
  FUN_80013b4c(1);
  DAT_800cfbd8 = DAT_800cfbd8 | 0x20;
  DsFlush();
  DsSyncCallback((DslCB)&LAB_80012d30);
  do {
    iVar2 = DsCommand('\t',(u_char *)0x0,(DslCB)0x0,-1);
  } while (iVar2 == 0);
  do {
    iVar2 = DsCommand('\r',&DAT_800d5b48,(DslCB)0x0,-1);
  } while (iVar2 == 0);
  do {
    iVar2 = DsCommand('\x0e',&DAT_8009a415,(DslCB)0x0,-1);
  } while (iVar2 == 0);
  do {
    iVar2 = DsCommand('\x15',"",(DslCB)0x0,-1);
  } while (iVar2 == 0);
  do {
    iVar2 = DsCommand('\x1b',"",(DslCB)0x0,-1);
  } while (iVar2 == 0);
  return;
}


