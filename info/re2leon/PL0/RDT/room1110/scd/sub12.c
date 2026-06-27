int sub12(void) {

	Sleep(2560);
	Work_set(4, 1);
	nop();
	Member_copy(16, 16);
	nop();
	Calc(16 + 2048)
	Member_set2(16, 16);
	nop();
	Work_set(4, 2);
	nop();
	Member_copy(16, 16);
	nop();
	Calc(16 - 4096)
	Member_set2(16, 16);
	nop();
	Work_set(4, 3);
	nop();
	Member_copy(16, 16);
	nop();
	Calc(16 - 4096)
	Member_set2(16, 16);
	nop();
	Work_set(4, 4);
	nop();
	Member_copy(16, 16);
	nop();
	Calc(16 + 6144)
	Member_set2(16, 16);
	nop();
	Work_set(4, 5);
	nop();
	Member_copy(16, 16);
	nop();
	Calc(16 + 2304)
	Member_set2(16, 16);
	nop();
	Work_set(4, 6);
	nop();
	Member_copy(16, 16);
	nop();
	Calc(16 + 3072)
	Member_set2(16, 16);
	nop();
	Work_set(4, 7);
	nop();
	Member_copy(16, 16);
	nop();
	Calc(16 - 4608)
	Member_set2(16, 16);
	nop();
	Evt_next();
	nop();
	goto(0x000080FF);
	return 0;
}