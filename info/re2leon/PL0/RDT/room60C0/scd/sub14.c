int sub14(void) {

goto Sub21();
Evt_next();
nop();
Xa_on(0, 1024);
Sleep(512);
Message_on(0, 3840, 0x0000);
Work_set(1, 0);
Plc_motion(0, 18, 0);
nop();
Wsleep();
Wsleeping();
return 0;
}