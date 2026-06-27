int sub14(void) {

	Cut_chg(0x08);
	Message_on(0, 512, 0x00EF);
	Evt_next();
	Cut_old();
	Cut_auto(1);
	return 0;
}