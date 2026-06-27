int sub07(void) {

	Aot_reset(2, 0, 0, 0, 0, 0x00);
	Evt_exec(255, 0x1808);
	return 0;
}