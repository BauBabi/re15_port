int sub04(void) {

	Light_kido_set(1, -26591);
	for(n = 0; n < 768; n++) {
		Evt_next();
		nop();
	}
	Light_kido_set(1, -8146);
	Evt_next();
	nop();
	Light_kido_set(1, 4135);
	for(n = 0; n < 512; n++) {
		Evt_next();
		nop();
	}
	Set(5, 0, 0);
	return 0;
}