public class Merge {

	private Comparable[] aux;
	private void merge(Comparable[] a, int low, int mid, int high) {
		int i = low, j = mid + 1;
		for (int k = low; k <= high; k++) {
			aux[k] = a[k];
		}
		for (int k = low; k <= high; k++) {
			if (i > mid) a[k] = aux[j++];
			else if (j > high) a[k] = aux[i++];
			else if (less(aux[i], aux[j])) a[k] = aux[i++];
			else a[k] = aux[j++];
		}
	}

	public void sort(Comparable[] a) {
		aux = new Comparable[a.length];
		sort(a, 0, a.length - 1);
		show(a);
	}

	private void sort(Comparable[] a, int low, int high) {
		if (low >= high) return;
		int mid = low + (high - low) / 2;
		sort(a, low, mid);
		sort(a, mid + 1, high);
		merge(a, low, mid, high);
	}


	private boolean less(Comparable a, Comparable b) {
		return a.compareTo(b) < 0;
	}

	private void show(Comparable[] a) {
		for (int i = 0; i < a.length; i++) {
			System.out.print(a[i] + " ");
		}
		System.out.println();
	}

	public static void main(String[] args) {
		new Merge().sort(new Integer[]{6, 8, 10, 1, 20, 14, 9, 50, 3, 25});
	}
}