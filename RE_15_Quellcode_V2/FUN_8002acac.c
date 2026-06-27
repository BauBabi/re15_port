void FUN_8002acac(uint param_1)

{
  if (iRam00000038 == 0) {
    uRam00000002 = (undefined1)(param_1 >> 1);
    uRam00000003 = uRam00000002;
  }
  else {
    uRam00000002 = (undefined1)param_1;
    uRam00000003 = 0;
  }
  uRam00000000 = uRam00000002;
  uRam00000001 = uRam00000003;
  CdMix((CdlATV *)0x0);
  return;
}
