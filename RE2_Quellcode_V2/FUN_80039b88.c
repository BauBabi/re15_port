/* FUN_80039b88 @ 0x80039b88  (Ghidra headless, raw MIPS overlay) */

undefined1 FUN_80039b88(void)

{
  bool bVar1;
  uint *puVar2;
  char cVar3;
  byte bVar4;
  
  puVar2 = DAT_800ea23c;
  FUN_80039464(0xf000,0x20a);
  bVar1 = false;
  if (DAT_800cfb74 < 0) {
    if ((DAT_800ce304 & 0x1000) != 0) {
      *(char *)((int)puVar2 + 3) = *(char *)((int)puVar2 + 3) + -1;
      bVar1 = true;
      if ((*puVar2 & 0xffff0000) == 0x1090000) {
        *(undefined1 *)((int)puVar2 + 3) = 0;
      }
      if (*(char *)((int)puVar2 + 3) < '\0') {
        *(undefined1 *)((int)puVar2 + 3) = 2;
      }
    }
    if ((DAT_800ce304 & 0x4000) != 0) {
      cVar3 = *(char *)((int)puVar2 + 3) + '\x01';
      *(char *)((int)puVar2 + 3) = cVar3;
      bVar1 = true;
      if ('\x02' < cVar3) {
        *(undefined1 *)((int)puVar2 + 3) = 0;
      }
    }
    if ((DAT_800ce304 & 0x8000) != 0) {
      bVar4 = *(char *)((int)puVar2 + 2) - 1;
      *(byte *)((int)puVar2 + 2) = bVar4;
      bVar1 = true;
      if ((int)((uint)bVar4 << 0x18) < 0) {
        *(undefined1 *)((int)puVar2 + 2) = 9;
      }
    }
    if ((DAT_800ce304 & 0x2000) != 0) {
      cVar3 = *(char *)((int)puVar2 + 2) + '\x01';
      *(char *)((int)puVar2 + 2) = cVar3;
      bVar1 = true;
      if ('\t' < cVar3) {
        *(undefined1 *)((int)puVar2 + 2) = 0;
      }
    }
    if (bVar1) {
      FUN_8005ba28(0x2100000,0);
    }
  }
  cVar3 = *(char *)((int)puVar2 + 0x2da) + *(char *)((int)puVar2 + 0x2db);
  *(char *)((int)puVar2 + 0x2da) = cVar3;
  if ((cVar3 == '`') || (cVar3 == '@')) {
    *(char *)((int)puVar2 + 0x2db) = -*(char *)((int)puVar2 + 0x2db);
  }
  *(undefined2 *)(puVar2 + 0xc5) = 0x18;
  *(short *)(puVar2 + 0xc4) = *(char *)((int)puVar2 + 2) * 0x18 + 0x34;
  *(short *)((int)puVar2 + 0x312) = *(char *)((int)puVar2 + 3) * 0x18 + 0x9a;
  *(char *)(puVar2 + 0xc6) = *(char *)((int)puVar2 + 2) * '\x18' + '\x04';
  *(char *)((int)puVar2 + 0x319) =
       (*(char *)((int)puVar2 + 3) * '\x18' + '*') - *(char *)((int)puVar2 + 0x2de);
  *(undefined1 *)((int)puVar2 + 0x31e) = *(undefined1 *)((int)puVar2 + 0x2da);
  *(ushort *)((int)puVar2 + 0x316) = 0x17 - (ushort)*(byte *)((int)puVar2 + 0x2de);
  if ((*(char *)((int)puVar2 + 2) == '\t') && ('\0' < *(char *)((int)puVar2 + 3))) {
    *(undefined2 *)((int)puVar2 + 0x312) = 0xb2;
    *(char *)((int)puVar2 + 0x319) = 'B' - *(char *)((int)puVar2 + 0x2de);
    *(ushort *)((int)puVar2 + 0x316) = 0x2f - (ushort)*(byte *)((int)puVar2 + 0x2de);
  }
  *(undefined1 *)((int)puVar2 + 0x2de) = 0;
  if ((DAT_800ce310 & 0x1000) == 0) {
    *(undefined1 *)((int)puVar2 + 0x2d9) = 0;
  }
  else {
    FUN_8005ba28(0x2090000,0);
    cVar3 = *(char *)((int)puVar2 + 2) + *(char *)((int)puVar2 + 3) * '\n';
    *(char *)((int)puVar2 + 0x2d9) = cVar3;
    if (cVar3 == '\t') {
      *(undefined1 *)((int)puVar2 + 0x2d9) = 0x1e;
    }
    if (*(byte *)((int)puVar2 + 0x2d9) < 9) {
      *(byte *)((int)puVar2 + 0x2d9) = *(byte *)((int)puVar2 + 0x2d9) + 1;
    }
    if (*(char *)((int)puVar2 + 0x2d9) == '\x13') {
      *(undefined1 *)((int)puVar2 + 0x2d9) = 0x1d;
    }
    if (0x13 < *(byte *)((int)puVar2 + 0x2d9)) {
      *(byte *)((int)puVar2 + 0x2d9) = *(byte *)((int)puVar2 + 0x2d9) - 1;
    }
    *(undefined1 *)((int)puVar2 + 0x2d5) = 0x10;
  }
  if ((DAT_800ce310 & 0x2000) != 0) {
    *(undefined1 *)((int)puVar2 + 0x2d9) = 1;
  }
  return *(undefined1 *)((int)puVar2 + 0x2d9);
}


