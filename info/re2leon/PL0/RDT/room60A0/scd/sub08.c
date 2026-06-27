int sub08(void) {

	Work_set(1, 0);
	nop();
	Sleep(7680);
	Se_on(2, 11, 1, 1, 0, 0, 0, 0);
	Evt_exec(255, 0x180D);
	Evt_exec(255, 0x1809);
	Plc_ret();
	nop();
	return 0;
}