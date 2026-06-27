int sub06(void) {

	Light_kido_set(0, -16291);
	for(n = 0; n < 768; n++) {
		Evt_next();
		nop();
		if(Cmp(26 != 1536)) {
			goto(0x00004200);
		}
	}
	Light_kido_set(0, 12405);
	Evt_next();
	nop();
	if(Cmp(26 != 1536)) {
		goto(0x00002800);
	}
	Light_kido_set(0, -28571);
	for(n = 0; n < 512; n++) {
		Evt_next();
		nop();
		if(Cmp(26 != 1536)) {
			goto(0x00000A00);
		}
	}
	Set(5, 2, 0);
	return 0;
}