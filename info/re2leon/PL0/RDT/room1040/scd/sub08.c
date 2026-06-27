int sub08(void) {

	Light_kido_set(0, -20426);
	for(n = 0; n < 768; n++) {
		Evt_next();
		nop();
		if(Cmp(26 = 768)) {
			goto(0x00004200);
		}
	}
	Light_kido_set(0, 4135);
	Evt_next();
	nop();
	if(Cmp(26 = 768)) {
		goto(0x00002800);
	}
	Light_kido_set(0, -32706);
	for(n = 0; n < 512; n++) {
		Evt_next();
		nop();
		if(Cmp(26 = 768)) {
			goto(0x00000A00);
		}
	}
	Set(5, 4, 0);
	return 0;
}