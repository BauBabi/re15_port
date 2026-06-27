int sub09(void) {

	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x10, -6975, 0, -6975, 0);
	Evt_next();
	nop();
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x11, -13115, 0, -6975, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x18, -10045, 0, -6975, 0);
	Sleep(512);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x1A, -21310, 0, -3905, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x13, -24380, 0, -3905, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x12, -21310, 0, -3905, 0);
	Sleep(512);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x16, -13115, 0, -835, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x1C, -10045, 0, -835, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x12, -6975, 0, -835, 0);
	Sleep(512);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x12, -13115, 0, -835, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x14, -10045, 0, -835, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x1A, -6975, 0, -835, 0);
	Sleep(512);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x12, -3905, 0, 27836, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x12, -13115, 0, 2236, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x1E, -10045, 0, 2236, 0);
	Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x1D, -6975, 0, 2236, 0);
	Sleep(512);
	Save(4, 0);
	for(n = 0; n < 1024; n++) {
		Work_copy(4, 10, 1);
		Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x10, 24766, 0, 16571, 0);
		Work_copy(4, 10, 1);
		Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x14, 21696, 0, 16571, 0);
		Copy(16, 4);
		nop();
		Calc(16 - 36865)
		Copy(4, 16);
		nop();
		Evt_next();
		nop();
	}
	for(n = 0; n < 768; n++) {
		Work_copy(4, 10, 1);
		Sce_espr_on(0x00, 0x0E, 0, 0, 0, 0x10, 24766, 0, 16571, 0);
		Copy(16, 4);
		nop();
		Calc(16 - 36865)
		Copy(4, 16);
		nop();
		Evt_next();
		nop();
	}
	Set(5, 1, 1);
	return 0;
}