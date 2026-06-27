int sub15(void) {

	while(Ck(5, 5, 1)) {
		Evt_next();
		nop();
	}
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x2E, -27236, 0, -30524, 254);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x30, -2657, 0, -10051, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x32, 1965, 0, -23112, 254);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x3E, 15013, 0, 15294, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x28, 1456, 0, 29879, 0);
	Sce_espr_on(0x00, 0x0D, 0, 0, 0, 0x2E, 8615, 2286, 3010, 2);
	Sce_espr_on(0x00, 0x0D, 0, 0, 0, 0x32, 7591, 24816, 12734, 2);
	return 0;
}