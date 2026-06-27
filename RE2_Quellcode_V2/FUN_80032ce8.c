/* FUN_80032ce8 @ 0x80032ce8  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80032ce8(int param_1)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  u_long uVar3;
  int iVar4;
  uint uVar5;
  u_char local_28 [8];
  u_long *local_20;
  u_long *local_1c;
  
  puVar1 = DAT_800d763c;
  DAT_800d767c = *(undefined2 *)(&DAT_800a2170 + param_1 * 0x20);
  DAT_800d7681 = 0;
  DAT_800d7670 = DAT_800e8980 + 0xb004;
  iVar4 = param_1 * 0x20;
  DAT_800d7674 = DAT_800d7670;
  if ((DAT_800d7632 & 0x100) != 0) {
    DAT_800d7674 = DAT_800d7670 + (uint)*(ushort *)(&DAT_800a2182 + param_1 * 0x20);
  }
  DAT_800d7678 = DAT_800d7674 + (uint)*(ushort *)(&DAT_800a2182 + iVar4);
  DAT_800d763c[1] = 0;
  *puVar1 = 0;
  FUN_80033370();
  StSetRing((u_long *)(DAT_800e8980 + 4),0x16);
  StSetStream(1,0,0xfffffff,(func1 *)0x0,(func2 *)0x0);
  DAT_800d768c = (uint)(byte)(&DAT_800988aa)[*(int *)(&DAT_800a216c + iVar4) * 8] * 0x10000 +
                 (uint)*(ushort *)(&DAT_800988a8 + *(int *)(&DAT_800a216c + iVar4) * 8);
  if (*(ushort *)(&DAT_800a2188 + iVar4) != 0) {
    DAT_800d768c = DAT_800d768c + (uint)*(ushort *)(&DAT_800a2188 + iVar4) * 5;
  }
  DsIntToPos(DAT_800d768c,(DslLOC *)&DAT_800d8cd8);
  DAT_800d5b81 = 1;
  DAT_800d5b80 = 1;
  local_28[0] = 0xa8;
  DsCommand('\t',(u_char *)0x0,(DslCB)0x0,-1);
  DsCommand('\x0e',local_28,(DslCB)0x0,-1);
  DsCommand('\r',&DAT_800d5b80,(DslCB)0x0,-1);
  DsRead2((DslLOC *)&DAT_800d8cd8,0x1c8);
  uVar5 = 0;
  do {
    uVar3 = StGetNext(&local_20,&local_1c);
    if (uVar3 == 0) {
      DecDCTReset(0);
      DecDCToutCallback((func *)&LAB_8003324c);
      DecDCTvlc(local_20,(u_long *)(&DAT_800d7670)[DAT_800d7681]);
      puVar1 = DAT_800d763c;
      DAT_800d75de = *(short *)((int)local_1c + 0x12) + 8U & 0xfff0;
      DAT_800d763c[2] = DAT_800d75de;
      puVar2 = DAT_800d763c;
      if (DAT_800d4238 == 1) {
        DAT_800d762c = puVar1[2] * 0xc;
      }
      else {
        DAT_800d762c = puVar1[2] << 3;
      }
      DAT_800d7624 = -(ushort)((short)local_1c[4] != 0x140) & 0x30;
      DAT_800d7630 = (undefined2)((ushort)local_1c[4] + 8 >> 4);
      uVar3 = local_1c[2];
      DAT_800d763c[1] = (short)uVar3;
      *puVar2 = (short)uVar3;
      StFreeRing(local_20);
      return 0;
    }
    VSync(0);
    uVar5 = uVar5 + 1;
  } while (uVar5 < 0xb5);
  return 1;
}


