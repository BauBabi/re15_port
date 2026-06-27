int sub29(void) {

	Aot_reset(0, 0, 0, 0, 0, 0x00);
	Message_on(0, 2048, 0x00FF);
	Evt_next();
	Evt_next();
	Aot_reset(0, 5, 49, 65280, 6173, 0x00);
	return 0;
}