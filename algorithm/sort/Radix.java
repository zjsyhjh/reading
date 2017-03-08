/*
 * 基数排序：一种整数排序算法
 */
public class Radix {

	public void sort(int[] a) {
		int d = getMaxBit(a);
		sort(a, a.length, d);
	}

	private void sort(int[] a, int n, int d) {
		int radix = 1;
		for (int i = 1; i <= d; i++) {
			int[] count = new int[10];
			int[] tmp = new int[n];
			for (int j = 0; j < n; j++) {
				int k = (a[j] / radix) % 10;
				count[k] += 1;
			}
			for (int j = 1; j < 10; j++) {
				count[j] += count[j - 1];
			}

			for (int j = n - 1; j >= 0; j--) {
				int k = (a[j] / radix) % 10;
				tmp[count[k] - 1] = a[j];
				count[k] -= 1;
			}

			for (int j = 0; j < n; j++) {
				a[j] = tmp[j];
			}
			radix *= 10;
		}
		show(a);
	}

	private void show(int[] a) {
		for (int i = 0; i < a.length; i++) {
			System.out.print(a[i] + " ");
		}
		System.out.println();
	}

	private int getMaxBit(int[] a) {
		int n = a.length;
		int maxData = a[0];
		for (int i = 1; i < a.length; i++) {
			if (a[i] > maxData) maxData = a[i];
		}
		int d = 0;
		while (maxData >= 1) {
			maxData /= 10;
			d += 1;
		}
		return d;
	}

	public static void main(String[] args) {
		new Radix().sort(new int[]{10, 3, 50, 100, 40, 56, 78, 90, 45, 30});
	}
}