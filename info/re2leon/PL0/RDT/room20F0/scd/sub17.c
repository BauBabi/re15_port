int sub17(void) {

	Work_set(1, 0);
	nop();
	Member_copy(16, 1);
	nop();
	Calc(16 && 63487)
	Member_set2(1, 16);
	nop();
	if(Ck(5, 1, 1)) {
		Work_set(3, 0);
		nop();
		goto Sub18();
	} else {
		if(Ck(5, 2, 1)) {
			Work_set(3, 0);
			nop();
			goto Sub18();
			Work_set(3, 1);
			nop();
			goto Sub18();
			Work_set(3, 2);
			nop();
			goto Sub18();
			Work_set(3, 3);
			nop();
			goto Sub18();
			Work_set(3, 4);
			nop();
			goto Sub18();
		}
		nop();
		nop();
		return 0;
}