int sub04(void) {

	if(Ck(8, 119, 0)) {
		if(Ck(6, 21, 0)) {
			Work_set(3, 0);
			nop();
			Member_copy(16, 0);
			nop();
			Calc(16 && 65023)
			Member_set2(0, 16);
			nop();
			Member_copy(16, 1);
			nop();
			Calc(16 && 63487)
			Member_set2(1, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 && 65407)
			Member_set2(7, 16);
			nop();
		}
		if(Ck(6, 22, 0)) {
			Work_set(3, 1);
			nop();
			Member_copy(16, 0);
			nop();
			Calc(16 && 65023)
			Member_set2(0, 16);
			nop();
			Member_copy(16, 1);
			nop();
			Calc(16 && 63487)
			Member_set2(1, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 && 65407)
			Member_set2(7, 16);
			nop();
		}
		if(Ck(6, 23, 0)) {
			Work_set(3, 2);
			nop();
			Member_copy(16, 0);
			nop();
			Calc(16 && 65023)
			Member_set2(0, 16);
			nop();
			Member_copy(16, 1);
			nop();
			Calc(16 && 63487)
			Member_set2(1, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 && 65407)
			Member_set2(7, 16);
			nop();
		}
		if(Ck(0, 25, 1)) {
			if(Ck(6, 209, 0)) {
				Work_set(3, 3);
				nop();
				Member_copy(16, 0);
				nop();
				Calc(16 && 65023)
				Member_set2(0, 16);
				nop();
				Member_copy(16, 1);
				nop();
				Calc(16 && 63487)
				Member_set2(1, 16);
				nop();
				Member_copy(16, 7);
				nop();
				Calc(16 && 65407)
				Member_set2(7, 16);
				nop();
			}
			if(Ck(1, 1, 1)) {
				if(Ck(6, 210, 0)) {
					Work_set(3, 4);
					nop();
					Member_copy(16, 0);
					nop();
					Calc(16 && 65023)
					Member_set2(0, 16);
					nop();
					Member_copy(16, 1);
					nop();
					Calc(16 && 63487)
					Member_set2(1, 16);
					nop();
					Member_copy(16, 7);
					nop();
					Calc(16 && 65407)
					Member_set2(7, 16);
					nop();
				}
			}
		}
	} else {
		if(Ck(6, 112, 0)) {
			Work_set(3, 0);
			nop();
			Member_copy(16, 0);
			nop();
			Calc(16 && 65023)
			Member_set2(0, 16);
			nop();
			Member_copy(16, 1);
			nop();
			Calc(16 && 63487)
			Member_set2(1, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 && 65407)
			Member_set2(7, 16);
			nop();
		}
		nop();
		nop();
		return 0;
}