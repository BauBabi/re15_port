int sub03(void) {

	Cut_chg(0x03);
	if(Ck(4, 75, 0)) {
		Message_on(0, 0, 0xFFFF);
	} else {
		Message_on(0, 256, 0xFFFF);
		nop();
		nop();
		Sleep(256);
		Cut_old();
		nop();
		return 0;
}