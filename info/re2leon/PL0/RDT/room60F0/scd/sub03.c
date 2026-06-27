int sub03(void) {

if(Ck(7, 59, 1)) {
Ck(7, 60, 1)
Ck(7, 61, 1)
Set(2, 7, 1);
Work_set(1, 0);
Plc_motion(1, 6, 0);
nop();
Sleep(6400);
Message_on(0, 512, 0xFFFF);
Evt_next();
Plc_motion(1, 6, 128);
nop();
Sleep(6400);
Plc_ret();
Set(2, 7, 0);
nop();
} else {
Message_on(0, 512, 0xFFFF);
Evt_next();
nop();
nop();
nop();
return 0;
}