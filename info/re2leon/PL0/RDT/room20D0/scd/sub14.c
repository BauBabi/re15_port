int sub14(void) {

	Work_set(1, 0);
	Plc_motion(1, 1, 0);
	nop();
	Sleep(2560);
	Plc_dest(0, 5120, 0, 0);
	return 0;
}