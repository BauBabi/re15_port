int DOOR1B00(void) {

	switch(12) {
	case 0:
	case 512:
		goto Sub01();
	break;
	case 256:
	case 768:
		goto Sub02();
	break;
	}
	return 0;
}