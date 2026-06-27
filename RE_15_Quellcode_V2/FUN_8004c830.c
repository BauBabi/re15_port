void FUN_8004c830(void)

{
  void *pvVar1;
  uint uVar2;
  ushort uVar3;
  ushort uVar4;
  void *p;
  
  uVar4 = 0xc;
  do {
    pvVar1 = (void *)FUN_80047648(uVar4 & 0xff);
    uVar4 = uVar4 + 1;
  } while (uVar4 < 0x13);
  uVar4 = 0;
  do {
    uVar3 = uVar4 & 0xff;
    uVar2 = (uint)uVar4;
    uVar4 = uVar4 + 1;
    pvVar1 = (void *)FUN_80048f28(pvVar1,uVar3,
                                  (&DAT_80074db0)[(uint)(byte)(&DAT_800b10ac)[uVar2 * 4] * 0xc],0);
  } while (uVar4 < 10);
  uVar3 = 0;
  AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,&DAT_800b2638 + (uint)DAT_800aca34 * 0xc);
  uVar4 = 0;
  do {
    pvVar1 = (void *)FUN_80048704(uVar4,pvVar1);
    uVar3 = uVar3 + 1;
    uVar4 = uVar3 & 0xff;
  } while (uVar3 < 10);
  if ((DAT_800b25c0 & 0xffff00) == 0x30200) {
    p = pvVar1;
    if ((&DAT_800b10ae)[(uint)DAT_800b25cf * 4] == '\0') {
      *(undefined1 *)((int)pvVar1 + 0xc) = (&DAT_80076244)[(uint)DAT_800b25cf * 4];
      *(undefined1 *)((int)pvVar1 + 0xd) = (&DAT_80076246)[(uint)DAT_800b25cf * 4];
      *(short *)((int)pvVar1 + 8) = DAT_800b25dc;
      *(undefined2 *)((int)pvVar1 + 10) = DAT_800b25de;
      p = (void *)((int)pvVar1 + 0x50);
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,pvVar1);
    }
    if ((&DAT_800b10ae)[(uint)DAT_800b25cf * 4] == '\x01') {
      *(undefined1 *)((int)p + 0xc) = (&DAT_80076244)[(uint)DAT_800b25cf * 4];
      *(undefined1 *)((int)p + 0xd) = (&DAT_80076246)[(uint)DAT_800b25cf * 4];
      *(short *)((int)p + 8) = DAT_800b25dc;
      *(undefined2 *)((int)p + 10) = DAT_800b25de;
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,p);
      pvVar1 = (void *)((int)p + 0x28);
      *(undefined1 *)((int)p + 0x34) = (&DAT_80076244)[(DAT_800b25cf + 1) * 4];
      *(undefined1 *)((int)p + 0x35) = (&DAT_80076246)[(DAT_800b25cf + 1) * 4];
      *(short *)((int)p + 0x30) = DAT_800b25dc + 0x28;
      *(undefined2 *)((int)p + 0x32) = DAT_800b25de;
      p = (void *)((int)p + 0x50);
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,pvVar1);
    }
    if ((&DAT_800b10ae)[(uint)DAT_800b25cf * 4] == '\x02') {
      *(undefined1 *)((int)p + 0xc) = (&DAT_80076244)[(DAT_800b25cf - 1) * 4];
      *(undefined1 *)((int)p + 0xd) = (&DAT_80076246)[(DAT_800b25cf - 1) * 4];
      *(short *)((int)p + 8) = DAT_800b25dc + -0x28;
      *(undefined2 *)((int)p + 10) = DAT_800b25de;
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,p);
      *(undefined1 *)((int)p + 0x34) = (&DAT_80076244)[(uint)DAT_800b25c8 * 4];
      *(undefined1 *)((int)p + 0x35) = (&DAT_80076246)[(uint)DAT_800b25c8 * 4];
      *(short *)((int)p + 0x30) = DAT_800b25dc;
      *(undefined2 *)((int)p + 0x32) = DAT_800b25de;
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,(void *)((int)p + 0x28));
    }
  }
  AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,&DAT_800b2620 + (uint)DAT_800aca34 * 0xc);
  return;
}
