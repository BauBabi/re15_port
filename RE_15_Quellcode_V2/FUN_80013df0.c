void FUN_80013df0(int param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  u_char auStack_18 [8];
  
  do {
    switch(DAT_800be570) {
    case '\0':
      goto code_r0x80013f68;
    case '\x01':
      auStack_18[0] = 0x80;
      CdControl('\x0e',auStack_18,(u_char *)0x0);
      DAT_800be570 = DAT_800be570 + '\x01';
      break;
    case '\x02':
      iVar2 = CdSync(1,(u_char *)0x0);
      if (iVar2 == 2) {
        CdIntToPos(DAT_800be57c,(CdlLOC *)&DAT_800bee70);
        CdControl('\x02',"",(u_char *)0x0);
        cVar1 = DAT_800be570 + '\x01';
      }
      else {
        iVar3 = 5;
LAB_80013f50:
        cVar1 = '\x01';
        if (iVar2 != iVar3) break;
      }
      goto LAB_80013f58;
    case '\x03':
      iVar2 = CdSync(1,(u_char *)0x0);
      if (iVar2 != 2) {
        iVar3 = 5;
        goto LAB_80013f50;
      }
      CdRead(DAT_800be574 + 0x7ffU >> 0xb,DAT_800be580,0x80);
      cVar1 = DAT_800be570 + '\x01';
LAB_80013f58:
      DAT_800be570 = cVar1;
      break;
    case '\x04':
      iVar2 = CdReadSync(1,(u_char *)0x0);
      iVar3 = -1;
      if (iVar2 != 0) goto LAB_80013f50;
      iVar2 = FUN_80013114();
      cVar1 = '\x01';
      if (iVar2 == 0) goto LAB_80013f58;
      DAT_800be570 = '\0';
    }
  } while (param_1 != 0);
code_r0x80013f68:
  return;
}
