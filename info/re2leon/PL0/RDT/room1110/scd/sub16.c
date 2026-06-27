int sub16(void) {

	Work_set(4, 0);
	nop();
	Speed_set(3, -769);
	Cut_chg(0x08);
	for(n = 0; n < 16384; n++) {
		Add_speed();
		Evt_next();
	}
	Cut_chg(0x00);
	for(n = 0; n < 16384; n++) {
		Add_speed();
		Evt_next();
	}
	goto Sub19();
	return 0;
}