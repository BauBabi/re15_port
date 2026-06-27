int sub18(void) {

	Message_on(0, 256, 0xFFFF);
	Evt_next();
	nop();
	return 0;
}