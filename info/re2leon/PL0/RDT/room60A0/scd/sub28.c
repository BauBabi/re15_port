int sub28(void) {

	for(n = 0; n < 512; n++) {
		Sce_fade_adjust(2, 0, 1);
		Evt_next();
		nop();
	}
	for(n = 0; n < 512; n++) {
		Sce_fade_adjust(2, 0, 2);
		Evt_next();
		nop();
	}
	for(n = 0; n < 512; n++) {
		Sce_fade_adjust(2, 0, 3);
		Evt_next();
		nop();
	}
	for(n = 0; n < 512; n++) {
		Sce_fade_adjust(2, 0, 4);
		Evt_next();
		nop();
	}
	Sce_fade_adjust(2, 0, 5);
	Evt_next();
	nop();
	Sce_fade_adjust(2, 0, 6);
	Evt_next();
	nop();
	Sce_fade_adjust(2, 0, 7);
	Evt_next();
	nop();
	return 0;
}