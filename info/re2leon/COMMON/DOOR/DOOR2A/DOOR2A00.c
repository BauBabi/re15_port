int DOOR2A00(void) {

	switch(12) {
	case 0:
		goto Sub01();
	break;
	case 256:
		goto Sub02();
	break;
	case 512:
	case 768:
		goto Sub03();
	break;
	}
	return 0;
}