int sub02(void) {

	Obj_model_set(0x00, 0, 0, 0, 0, 0, 0, 0, 2560, 4096, 1216, -15110, 24727, 0, 252, 0, 0, 0, 0, 0, 0, 0);
	Obj_model_set(0x01, 0, 0, 0, 0, 0, 0, 0, 2560, 4096, -13632, -15110, 29371, 0, 8, 0, 0, 0, 0, 0, 0, 0);
	Obj_model_set(0x02, 0, 0, 0, 0, 0, 0, 0, 2560, 4096, -13632, -15110, -6453, 0, 8, 0, 0, 0, 0, 0, 0, 0);
	Obj_model_set(0x03, 0, 0, 0, 0, 0, 0, 0, 2560, 4096, -13632, -15110, 23776, 0, 8, 0, 0, 0, 0, 0, 0, 0);
	Obj_model_set(0x04, 0, 0, 0, 0, 0, 0, 0, 2560, 4096, 14008, -15110, 12025, 0, 244, 0, 0, 0, 0, 0, 0, 0);
	if(Ck(29, 10, 0)) {
		Work_set(4, 0);
		nop();
		Member_set(12, 8270);
		Work_set(4, 1);
		nop();
		Member_set(12, 8270);
		Work_set(4, 2);
		nop();
		Member_set(12, 8270);
		Work_set(4, 3);
		nop();
		Member_set(12, 8270);
		Work_set(4, 4);
		nop();
		Member_set(12, 8270);
	} else {
		if(Ck(1, 1, 1)) {
			if(Ck(4, 153, 1)) {
				Work_set(4, 0);
				nop();
				Member_set(12, 8270);
				Work_set(4, 1);
				nop();
				Member_set(12, 8270);
				Work_set(4, 2);
				nop();
				Member_set(12, 8270);
				Work_set(4, 3);
				nop();
				Member_set(12, 8270);
				Work_set(4, 4);
				nop();
				Member_set(12, 8270);
			}
		}
		nop();
		nop();
		Aot_set(3, 6, 33, 0, 0, 12223, 5858, 30725, -18421, 10, 0, 86);
		Aot_set(4, 5, 49, 0, 0, 12223, 5858, 30725, -18421, 255, 0, 24);
		if(Ck(29, 10, 1)) {
			if(Ck(1, 1, 1)) {
				if(Ck(4, 153, 1)) {
					Aot_reset(4, 4, 49, 1024, 0, 0xFF);
				} else {
					Aot_reset(4, 4, 49, 768, 0, 0xFF);
					nop();
					nop();
				} else {
					Aot_reset(4, 4, 49, 768, 0, 0xFF);
					nop();
					nop();
				}
				return 0;
}