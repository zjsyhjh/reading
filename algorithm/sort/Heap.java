public class Heap {
	private Comparable[] aux;
	public void sort(Comparable[] a) {
		aux = new Comparable[a.length + 1];
		for (int i = 0; i < a.length; i++) {
			aux[i + 1] = a[i];
		}
		sort(aux, aux.length - 1);
		show(aux);
	}

	private void sort(Comparable[] a, int n) {
		for (int i = n / 2; i >= 1; i--) {
			sink(a, i, n);
		}
		while (n > 1) {
			exch(a, 1, n--);
			sink(a, 1, n);
		}
	}

	private void sink(Comparable[] a, int k, int n) {
		while (2 * k <= n) {
			int  j = 2 * k;
			if (j < n && less(a[j], a[j + 1])) j++;
			if (!less(a[k], a[j])) break;
			exch(a, k, j);
			k = j;
		}
	}

	private boolean less(Comparable a, Comparable b) {
		return a.compareTo(b) < 0;
	}

	private void exch(Comparable[] a, int i, int j) {
		Comparable temp = a[i];
		a[i] = a[j];
		a[j] = temp;
	}

	private void show(Comparable[] a) {
		for (int i = 1; i < a.length; i++) {
			System.out.print(a[i] + " " );
		}
		System.out.println();
	}

	public static void main(String[] args) {
		new Heap().sort(new Integer[]{10, -5, 6, 1, 4, 40, 30, 25, 28, 15, 50, 60, 55});
	}
}