import java.util.*;

/**
 * Created by iHge2k on 2017/3/22.
 * LCA + 离线tarjan算法
 * HDU-2586
 * dist[u]代表从根到节点u的距离，计算节点u、v之间的距离为dist[u] + dist[v] - 2 * dist[lca(u,v)];
 * 而lca(u,v)可以通过一次tarjan算法求出，算法原理请参考博客：
 * http://blog.csdn.net/jhgkjhg_ugtdk77/article/details/47190261
 * http://noalgo.info/476.html
 * http://blog.jobbole.com/96560/
 */
public class Main {

    private static final int MAX_N = 40040;

    //the number of vertex and edge
    private int N;
    private int M;

    //the number of edges
    private int NE1;
    private int NE2;


    public Main(int N, int M) {
        this.N = N;
        this.M = M;
        this.NE1 = 0;
        this.NE2 = 0;
        this.head1 = new int[N + 1];
        this.head2 = new int[N + 1];
        this.edges = new Edge[(N << 1) + 4];
        this.queryEdges = new Edge[(M << 1) + 4];
        this.nums = new int[M + 2][3];
        this.parent = new int[N + 1];
        this.mark = new boolean[N + 1];
        this.dist = new int[N + 1];
        Arrays.fill(head1, -1);
        Arrays.fill(head2, -1);
        Arrays.fill(mark, false);
    }

    private class Edge {
        int v, w, next;
    }

    private Edge[] edges;
    private int[] head1;
    private int[][] nums;


    private class Node {
        int v, id;
        Node(int _v, int _id) {
            v = _v;
            id = _id;
        }
    }

    private void insert1(int u, int v, int w) {
        edges[NE1] = new Edge();
        edges[NE1].v = v;
        edges[NE1].w = w;
        edges[NE1].next = head1[u];
        head1[u] = NE1++;
    }

    private Edge[] queryEdges;
    private int[] head2;
    private void insert2(int u, int v, int id) {
        queryEdges[NE2] = new Edge();
        queryEdges[NE2].v = v;
        queryEdges[NE2].w = id;
        queryEdges[NE2].next = head2[u];
        head2[u] = NE2++;
    }


    private int[] parent;
    private boolean[] mark;

    private int find(int x) {
        if (x == parent[x]) {
            return x;
        }
        return parent[x] = find(parent[x]);
    }

    private int[] dist;

    private void tarjan(int u) {
        mark[u] = true;
        for (int i = head1[u]; i != -1; i = edges[i].next) {
            int v = edges[i].v;
            int w = edges[i].w;
            if (!mark[v]) {
                dist[v] = dist[u] + w;
                tarjan(v);
                parent[v] = u;
            }
        }
        for (int i = head2[u]; i != -1; i = queryEdges[i].next) {
            int v = queryEdges[i].v, id = queryEdges[i].w;
            if (mark[v]) {
                nums[id][2] = find(v);
            }
        }
    }

    private void solve() {
        dist[1] = 0;
        tarjan(1);
        for (int i = 1; i <= M; i++) {
            System.out.println(dist[nums[i][0]] + dist[nums[i][1]] - 2 * dist[nums[i][2]]);
        }
    }

    private void init(Scanner in) {

        for (int i = 1; i < N; i++) {
            int u = in.nextInt();
            int v = in.nextInt();
            int w = in.nextInt();
            insert1(u, v, w);
            insert1(v, u, w);
        }

        for (int i = 1; i <= M; i++) {
            int u = in.nextInt();
            int v = in.nextInt();
            nums[i][0] = u;
            nums[i][1] = v;
            insert2(u, v, i);
            insert2(v, u, i);
        }
        for (int i = 1; i <= N; i++) {
            parent[i] = i;
        }
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int ncase = sc.nextInt();
        while (ncase-- > 0) {

            int n = sc.nextInt();
            int m = sc.nextInt();
            Main obj = new Main(n, m);

            obj.init(sc);
            obj.solve();
        }
    }
}
