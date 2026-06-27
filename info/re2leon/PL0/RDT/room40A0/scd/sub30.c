int sub30(void) {

	Aot_reset(1, 0, 0, 0, 0, 0x00);
	Se_on(2, 22, 1, 0, 0, 29466, 4341, 25000);
	Message_on(0, 0, 0x00FF);
	Evt_next();
	nop();
	Aot_reset(1, 5, 49, 65280, 6174, 0x00);
	return 0;
}