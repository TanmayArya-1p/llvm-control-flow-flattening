int main() {
	int x=1;
	int c=2;
	if((x&c) == 3) {
		int a = 1;
		int b = 2;
		int d = a+b;
		if(d > 2) {
			if(a == 1) {
				int h = d * 2;
				if(h > 5) {
					int i = h + 1;
				} else {
					int j = h - 1;
					if(j < 4) {
						int k = j * 3;
					}
				}
			} else {
				int l = a + d;
			}
		} else {
			int m = b * 2;
		}
	} else {
		int e = 3;
		int f = 4;
		int g = e+f;
	}
}
