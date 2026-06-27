int sub05(void) {

	Set(2, 7, 1);
	Se_on(2, 22, 0, 0, 0, -19567, -7965, -28448);
	Message_on(0, 256, 0xFFFF);
	Evt_next();
	Set(2, 7, 0);
	nop();
	return 0;
}