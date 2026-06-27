int sub18(void) {

	Set(2, 7, 1);
	Message_on(0, 5376, 0xFFFF);
	Sleep(256);
	Cut_chg(0x08);
	Sleep(512);
	while(Sce_trg_ck(0, 48);) {
		Evt_next();
		nop();
	}
	Cut_chg(0x01);
	Cut_auto(1);
	Set(2, 7, 0);
	return 0;
}