int sub03(void) {

	Cut_chg(0x09);
	Message_on(0, 256, 0xFFFF);
	Sleep(256);
	Cut_old();
	nop();
	return 0;
}