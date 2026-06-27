int sub02(void) {

	if(Ck(4, 2, 0)) {
		if(Ck(8, 4, 1)) {
			Message_on(0, 256, 0xFFFF);
			Se_on(2, 37, 0, 0, 0, 6952, -1800, -25908);
			Set(4, 2, 1);
			Aot_reset(0, 1, 49, 4251, 0, 0x20FE);
		} else {
			Message_on(0, 0, 0xFFFF);
			Se_on(2, 22, 0, 0, 0, 6952, -1800, -25908);
			nop();
			nop();
		}
		return 0;
}