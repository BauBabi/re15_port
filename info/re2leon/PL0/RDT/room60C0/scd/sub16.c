int sub16(void) {

goto Sub21();
Evt_next();
nop();
Xa_on(0, 1536);
Sleep(512);
Message_on(0, 4352, 0x0000);
Work_set(1, 0);
Plc_motion(0, 22, 0);
nop();
Plc_neck(2, 0, 0, 4, 0, -14272);
Sleep(1024);
Plc_neck(2, 0, 0, 252, 0, -14272);
Sleep(2048);
Plc_neck(2, 0, 0, 4, 0, -14272);
Sleep(2048);
Plc_neck(5, 3, 0, 0, 0, -14304);
Wsleep();
Wsleeping();
return 0;
}