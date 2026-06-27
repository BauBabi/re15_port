int sub04(void) {

	Light_kido_set(0, 8270);
	Light_pos_set(0, 1, 11, -20295);
	Light_pos_set(0, 1, 13, -31510);
	for(n = 0; n < 768; n++) {
		Evt_next();
		nop();
	}
	Light_kido_set(0, 7205);
	Light_pos_set(0, 1, 11, -5960);
	Light_pos_set(0, 1, 13, 8426);
	Evt_next();
	nop();
	Light_kido_set(0, -14286);
	Light_pos_set(0, 1, 11, 19641);
	Light_pos_set(0, 1, 13, -17175);
	for(n = 0; n < 512; n++) {
		Evt_next();
		nop();
	}
	Set(5, 0, 0);
	return 0;
}