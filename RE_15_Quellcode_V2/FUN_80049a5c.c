void FUN_80049a5c(void)

{
  undefined1 uVar1;
  uint uVar2;
  int iVar3;
  ushort uVar4;
  ushort uVar5;
  undefined *puVar6;
  undefined *p;
  
  FUN_80047648(0);
  uVar1 = FUN_8004ed6c();
  FUN_80048a44(uVar1,&DAT_8019a000);
  FUN_800c6c58();
  puVar6 = &DAT_80199800;
  if (DAT_800aca34 == 0) {
    puVar6 = &DAT_80199814;
  }
  uVar5 = 0;
  do {
    uVar4 = uVar5 & 0xff;
    uVar2 = (uint)uVar5;
    uVar5 = uVar5 + 1;
    puVar6 = (undefined *)
             FUN_80048f28(puVar6,uVar4,(&DAT_80074db0)[(uint)(byte)(&DAT_800b10ac)[uVar2 * 4] * 0xc]
                          ,0);
  } while (uVar5 < 10);
  FUN_80048f28(puVar6,(uint)DAT_800b25c8,
               (&DAT_80074db0)[(uint)(byte)(&DAT_800b10ac)[(uint)DAT_800b25c8 * 4] * 0xc],1);
  AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,&DAT_800b2638 + (uint)DAT_800aca34 * 0xc);
  if ((DAT_800b25c0 & 0xffffff) == 0x10100) {
    FUN_800473f8();
    AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,&DAT_800b2650 + (uint)DAT_800aca34 * 0xc);
  }
  puVar6 = &DAT_80198000;
  if (DAT_800aca34 == 0) {
    puVar6 = &DAT_80198014;
  }
  uVar5 = 0;
  do {
    puVar6 = (undefined *)FUN_80048704(uVar5 & 0xff,puVar6);
    uVar5 = uVar5 + 1;
  } while (uVar5 < 10);
  puVar6[4] = DAT_800b25cd;
  puVar6[5] = DAT_800b25cd;
  puVar6[6] = DAT_800b25cd;
  *(short *)(puVar6 + 8) = DAT_800b25d8 + 0x18;
  *(short *)(puVar6 + 10) = DAT_800b25da + DAT_800b2608 + 0x58;
  AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,puVar6);
  iVar3 = (uint)DAT_800b25c8 * 4;
  if (DAT_800b25c8 == 0x80) {
    p = puVar6 + 0x78;
  }
  else {
    p = puVar6 + 0x28;
    if ((&DAT_800b10ae)[iVar3] == '\0') {
      puVar6[0x34] = (&DAT_80076244)[iVar3];
      puVar6[0x35] = (&DAT_80076246)[(uint)DAT_800b25c8 * 4];
      *(short *)(puVar6 + 0x30) = DAT_800b25dc;
      *(undefined2 *)(puVar6 + 0x32) = DAT_800b25de;
      p = puVar6 + 0x78;
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,puVar6 + 0x28);
    }
    if ((&DAT_800b10ae)[(uint)DAT_800b25c8 * 4] == '\x01') {
      p[0xc] = (&DAT_80076244)[(uint)DAT_800b25c8 * 4];
      p[0xd] = (&DAT_80076246)[(uint)DAT_800b25c8 * 4];
      *(short *)(p + 8) = DAT_800b25dc;
      *(undefined2 *)(p + 10) = DAT_800b25de;
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,p);
      puVar6 = p + 0x28;
      p[0x34] = (&DAT_80076244)[(DAT_800b25c8 + 1) * 4];
      p[0x35] = (&DAT_80076246)[(DAT_800b25c8 + 1) * 4];
      *(short *)(p + 0x30) = DAT_800b25dc + 0x28;
      *(undefined2 *)(p + 0x32) = DAT_800b25de;
      p = p + 0x50;
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,puVar6);
    }
    if ((&DAT_800b10ae)[(uint)DAT_800b25c8 * 4] == '\x02') {
      p[0xc] = (&DAT_80076244)[(DAT_800b25c8 - 1) * 4];
      p[0xd] = (&DAT_80076246)[(DAT_800b25c8 - 1) * 4];
      *(short *)(p + 8) = DAT_800b25dc + -0x28;
      *(undefined2 *)(p + 10) = DAT_800b25de;
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,p);
      puVar6 = p + 0x28;
      p[0x34] = (&DAT_80076244)[(uint)DAT_800b25c8 * 4];
      p[0x35] = (&DAT_80076246)[(uint)DAT_800b25c8 * 4];
      *(short *)(p + 0x30) = DAT_800b25dc;
      *(undefined2 *)(p + 0x32) = DAT_800b25de;
      p = p + 0x50;
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,puVar6);
    }
  }
  uVar5 = 0;
  *(short *)(p + 8) = DAT_800b25d8 + 5;
  *(short *)(p + 10) = DAT_800b25da + 0x20;
  AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,p);
  *(undefined2 *)(p + 0x30) = DAT_800b25f0;
  *(undefined2 *)(p + 0x32) = DAT_800b25f2;
  AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,p + 0x28);
  puVar6 = p + 0x50;
  do {
    uVar5 = uVar5 + 1;
    AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,puVar6);
    puVar6 = puVar6 + 0x28;
  } while (uVar5 < 0x30);
  AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,&DAT_800b2620 + (uint)DAT_800aca34 * 0xc);
  return;
}
