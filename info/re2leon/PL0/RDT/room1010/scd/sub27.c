int sub27(void) {

while(Ck(5, 0, 0)) {
Evt_next();
nop();
}
Work_set(1, 0);
nop();
Plc_dest(0, 2336, -9023, -14658);
goto Sub20();
Plc_dest(0, 1824, -26688, 15823);
goto Sub20();
Plc_motion(0, 4, 0);
Sleep(3584);
Set(5, 1, 1);
return 0;
}