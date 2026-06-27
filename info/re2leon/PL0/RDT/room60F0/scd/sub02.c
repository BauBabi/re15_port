int sub02(void) {

Message_on(0, 256, 0xFFFF);
Evt_next();
nop();
if(Ck(11, 31, 0)) {
if(Ck(7, 59, 1)) {
Ck(7, 60, 1)
Ck(7, 61, 1)
Set(2, 7, 1);
Work_set(1, 0);
Plc_motion(1, 6, 0);
nop();
Sleep(6400);
Message_on(0, 0, 0xFFFF);
Set(8, 63, 1);
Evt_next();
Plc_motion(1, 6, 128);
nop();
Sleep(6400);
Aot_reset(3, 5, 49, 65280, 6147, 0x00);
Plc_ret();
Set(2, 7, 0);
nop();
} else {
Message_on(0, 0, 0xFFFF);
Set(8, 63, 1);
Evt_next();
nop();
Aot_reset(3, 5, 49, 65280, 6147, 0x00);
nop();
nop();
}
return 0;
}