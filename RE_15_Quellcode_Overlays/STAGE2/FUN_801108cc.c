/* FUN_801108cc @ 0x801108cc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801108cc(void)

{
  ushort uVar1;
  uint in_v0;
  int iVar2;
  uint uVar3;
  int in_v1;
  
  if ((in_v0 & 1) != 0) {
    *(undefined1 *)(in_v1 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 6;
  }
  if (((*(short *)(_DAT_800ac784 + 0x1d4) < 7000) &&
      (iVar2 = func_0x8001a9cc(&DAT_800aca88,0x80), iVar2 == 0)) &&
     (DAT_800acad6 <= *(byte *)(_DAT_800ac784 + 0x82))) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    uVar1 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 6) = (uVar1 & 1) + 6;
  }
  if (((_DAT_800aca58 == 0x701) && (DAT_800aca5a == '\x02')) &&
     (uVar3 = func_0x8001af20(), (uVar3 & 1) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  if (0x1e < *(short *)(_DAT_800ac784 + 0x1dc)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  if ((DAT_800acae7 & 1) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0xb;
  }
  return;
}


