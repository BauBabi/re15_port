int sub03(void) {

	Light_pos_set(0, 0, 11, -4945);
	Light_pos_set(0, 0, 13, 3311);
	for(n = 0; n < 768; n++) {
		Evt_next();
		nop();
	}
	Light_pos_set(0, 0, 11, 12461);
	Light_pos_set(0, 0, 13, 15591);
	Evt_next();
	nop();
	Light_pos_set(0, 0, 11, -1875);
	Light_pos_set(0, 0, 13, 9451);
	for(n = 0; n < 512; n++) {
		Evt_next();
		nop();
	}
	Set(5, 0, 0);
	return 0;
}