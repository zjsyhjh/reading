public class Shell {

	public void sort(Comparable[] a) {
		int N = a.length;
		int h = 1;
		while (h < N / 3) h = 3 * h + 1;
		while (h >= 1) {
			for (int i = h; i < N; i++) {
				for (int j = i; j > 0 && less(a[j], a[j - h]); j--) {
					exch(a, j, j - h);
				}
			}
			h /= 3;
			show(a);
		}
	}

	private void show(Comparable[] a) {
		for (int i = 0; i < a.length; i++) {
			System.out.print(a[i] + " ");
		}
		System.out.println();
	}

	private boolean less(Comparable a, Comparable b) {
		return a.compareTo(b) < 0;
	}

	private void exch(Comparable[] a, int i, int j) {
		Comparable temp = a[i];
		a[i] = a[j];
		a[j] = temp;
	}
	
	public static void main(String[] args) {
		new Shell().sort(new Integer[]{10, 4, 5, 17, 20, 16, 18, 2, 9, 11, 8, 7, 30, 25, 28, 45, 33, 40});
	}
}