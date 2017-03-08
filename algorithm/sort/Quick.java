import java.util.*;

public class Quick {

	public void sort(Comparable[] a) {
		Collections.shuffle(Arrays.asList(a));
		sort(a, 0, a.length - 1);
		show(a);
	}

	private void sort(Comparable[] a, int low, int high) {
		if (low >= high) return;
		int mid = partition(a, low, high);
		sort(a, low, mid - 1);
		sort(a, mid + 1, high);
	}

	private int partition(Comparable[] a, int low, int high) {
		Comparable v = a[low];
		int i = low, j = high + 1;
		while (true) {
			while (less(a[++i], v)) if (i == high) break;
			while (less(v, a[--j])) if (j == low) break;
			if (i >= j) break;
			exch(a, i, j);
		}
		exch(a, low, j);
		return j;
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
		for (int i = 0; i < a.length; i++) {
			System.out.print(a[i] + " ");
		}
		System.out.println();
	}

	public static void main(String[] args) {
		new Quick().sort(new Integer[]{-10, 20, 5, 2, 10, 9, 6, 40, 23, 26, 60, 33, 37});
	}
}