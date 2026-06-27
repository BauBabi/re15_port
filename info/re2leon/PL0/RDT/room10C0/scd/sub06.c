int sub06(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Sleep(5120);
	Cut_chg(0x09);
	Sleep(7680);
	goto Sub07();
	Sleep(7680);
	Cut_old();
	nop();
	Sleep(2560);
	goto Sub08();
	return 0;
}