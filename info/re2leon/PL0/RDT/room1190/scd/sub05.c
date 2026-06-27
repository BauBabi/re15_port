int sub05(void) {

	Set(4, 152, 1);
	Sleep(2560);
	Work_set(3, 4);
	nop();
	goto Sub06();
	Sleep(5120);
	Work_set(3, 2);
	nop();
	goto Sub06();
	Sleep(12800);
	Work_set(3, 3);
	nop();
	goto Sub06();
	Sleep(5120);
	Work_set(3, 1);
	nop();
	goto Sub06();
	Sleep(23040);
	goto Sub07();
	return 0;
}