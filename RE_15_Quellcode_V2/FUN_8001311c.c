void FUN_8001311c(void)

{
  FUN_80013b60(7,&DAT_800c0000,0);
                    /* WARNING: Read-only address (ram,0x80013b7c) is written */
  uRam80013b7c = 0x80300b0;
                    /* WARNING: Read-only address (ram,0x80013b80) is written */
  uRam80013b80 = 0;
  return;
}
