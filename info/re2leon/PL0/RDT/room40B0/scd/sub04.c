int sub04(void) {

	Copy(16, 4);
	nop();
	Calc(16 + 256)
	Copy(4, 16);
	nop();
	Copy(16, 4);
	nop();
	Calc(16 && 65295)
	Copy(4, 16);
	nop();
	Work_set(4, 0);
	nop();
	Member_set2(14, 4);
	nop();
	Work_set(4, 1);
	nop();
	Member_set2(14, 4);
	nop();
	Evt_next();
	nop();
	goto(0x0000D2FF);
	return 0;
}