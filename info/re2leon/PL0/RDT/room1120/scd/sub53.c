int sub53(void) {

	Work_set(1, 0);
	nop();
	Member_set(43, 0);
	Work_set(3, 0);
	nop();
	Pos_set(0, 125, 0, 125);
	Member_copy(16, 7);
	nop();
	Calc(16 || 128)
	Member_set2(7, 16);
	nop();
	Work_set(3, 1);
	nop();
	Pos_set(0, 125, 0, 6265);
	Member_copy(16, 7);
	nop();
	Calc(16 || 128)
	Member_set2(7, 16);
	nop();
	Work_set(3, 2);
	nop();
	Pos_set(0, 125, 0, 12405);
	Member_copy(16, 7);
	nop();
	Calc(16 || 128)
	Member_set2(7, 16);
	nop();
	Work_set(3, 3);
	nop();
	Pos_set(0, 125, 0, 18545);
	Member_copy(16, 7);
	nop();
	Calc(16 || 128)
	Member_set2(7, 16);
	nop();
	Work_set(4, 5);
	nop();
	Member_copy(16, 1);
	nop();
	Calc(16 && 63487)
	Member_set2(1, 16);
	nop();
	Work_set(4, 11);
	nop();
	Member_copy(16, 1);
	nop();
	Calc(16 && 63487)
	Member_set2(1, 16);
	nop();
	Sce_espr_kill(4, 10, 0, 0);
	nop();
	return 0;
}