int sub09(void) {

	Aot_reset(2, 0, 0, 0, 0, 0x00);
	Se_on(2, 19, 1, 4, 1, 0, 0, 0);
	Evt_exec(255, 0x180A);
	return 0;
}