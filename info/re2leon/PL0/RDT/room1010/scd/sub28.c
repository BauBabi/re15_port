int sub28(void) {

Set(2, 7, 1);
Set(1, 27, 1);
Evt_kill(9);
Work_set(1, 0);
nop();
Member_copy(4, 11);
nop();
Member_copy(5, 13);
nop();
Work_set(3, 0);
nop();
Work_copy(5, 10, 1);
Work_copy(4, 4, 1);
Plc_dest(0, 2337, 0, 0);
goto Sub21();
Xa_on(0, 1792);
Message_on(0, 2048, 0x0000);
Plc_motion(0, 1, 0);
Sleep(5120);
Plc_motion(0, 1, 128);
Sleep(5120);
Plc_motion(0, 4, 128);
Sleep(6144);
Plc_motion(0, 5, 20);
Sleep(5632);
Plc_motion(0, 3, 0);
Sleep(5120);
Plc_motion(0, 3, 128);
Sleep(5120);
Plc_motion(0, 5, 20);
Evt_exec(9, 0x1813);
Wsleep();
Wsleeping();
Set(2, 7, 0);
Set(1, 27, 0);
return 0;
}