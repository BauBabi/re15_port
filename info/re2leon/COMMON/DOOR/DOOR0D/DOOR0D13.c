int DOOR0D13(void) {

	if(Cmp(13 = 256)) {
		Evt_next();
		nop();
		goto(0x0000F4FF);
	}
	return 0;
}