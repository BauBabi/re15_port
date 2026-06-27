int sub08(void) {

Set(2, 7, 1);
Work_set(1, 0);
nop();
if(Ck(1, 0, 0)) {
Plc_motion(0, 26, 0);
Sleep(5120);
Plc_stop();
nop();
} else {
Plc_motion(0, 15, 0);
Sleep(5120);
Plc_stop();
nop();
nop();
nop();
Sleep(2560);
Set(2, 7, 0);
return 0;
}