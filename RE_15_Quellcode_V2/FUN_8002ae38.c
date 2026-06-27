void FUN_8002ae38(void)

{
  int iVar1;
  
  iVar1 = FUN_800130c4(0,&DAT_800b8984);
  CdIntToPos(iVar1,(CdlLOC *)&DAT_800b3f80);
  do {
    do {
      iVar1 = CdSync(1,(u_char *)0x0);
    } while (iVar1 != 2);
    iVar1 = CdControlB('\x15',"",(u_char *)0x0);
  } while ((iVar1 == 0) || (iVar1 = CdControlB('\t',(u_char *)0x0,(u_char *)0x0), iVar1 == 0));
  return;
}
