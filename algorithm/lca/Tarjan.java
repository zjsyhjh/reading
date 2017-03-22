import java.util.*;

/**
 * Created by iHge2k on 2017/3/22.
 * hihocode 1067
 * tarjan离线处理
 */
public class Tarjan {
    private int N, M, total;
    private int NE1, NE2;
    private boolean[] vis;
    private int[] parent;
    private int[] ans;
    private int[] head1, head2;

    class Edge {
        int v, id, next;
    }

    private Edge[] edges;
    private Edge[] queries;

    public Tarjan(int N) {
        this.N = N;
        this.total = 0;
        this.NE1 = this.NE2 = 0;
        this.head1 = new int[N << 1];
        this.head2 = new int[N << 1];
        this.parent = new int[N << 1];
        this.names = new String[N << 1];
        this.vis = new boolean[N << 1];
        this.edges = new Edge[(N << 1) + 1];
        this.mp = new HashMap<>();
        Arrays.fill(head1, -1);
        Arrays.fill(head2, -1);
    }

    private void insert1(int u, int v, int id) {
        edges[NE1] = new Edge();
        edges[NE1].v = v;
        edges[NE1].id = id;
        edges[NE1].next = head1[u];
        head1[u] = NE1++;
    }

    private void insert2(int u, int v, int id) {
        queries[NE2] = new Edge();
        queries[NE2].v = v;
        queries[NE2].id = id;
        queries[NE2].next = head2[u];
        head2[u] = NE2++;
    }

    private HashMap<String, Integer> mp;
    private String[] names;

    private int find(int x) {
        if (x == parent[x]) {
            return x;
        }
        return parent[x] = find(parent[x]);
    }

    private void tarjan(int u) {
        vis[u] = true;
        for (int i = head1[u]; i != -1; i = edges[i].next) {
            int v = edges[i].v;
            if (!vis[v]) {
                tarjan(v);
                parent[v] = u;
            }
        }
        for (int i = head2[u]; i != -1; i = queries[i].next) {
            if (vis[queries[i].v]) {
                ans[queries[i].id] = find(queries[i].v);
            }
        }
    }

    private void solve() {
        for (int i = 1; i <= total; i++) {
            parent[i] = i;
        }
        tarjan(1);
        for (int i = 1; i <= M; i++) {
            System.out.println(names[ans[i]]);
        }
    }

    private void init(Scanner in) {

        for (int i = 1; i <= N; i++) {
            String[] s = in.nextLine().split(" ");
            if (mp.get(s[0]) == null) {
                mp.put(s[0], ++total);
                names[total] = s[0];
            }
            if (mp.get(s[1]) == null) {
               mp.put(s[1], ++total);
               names[total] = s[1];
            }
            int u = mp.get(s[0]);
            int v = mp.get(s[1]);
            insert1(u, v, -1);
            insert1(v, u, -1);
        }

        M = Integer.parseInt(in.nextLine());
        queries = new Edge[(M << 1) + 1];
        ans = new int[M + 1];
        for (int i = 1; i <= M; i++) {
            String[] s = in.nextLine().split(" ");
            int u = mp.get(s[0]);
            int v = mp.get(s[1]);
            insert2(u, v, i);
            insert2(v, u, i);
        }
        solve();
    }

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int n = Integer.parseInt(in.nextLine());
        Tarjan obj = new Tarjan(n);
        obj.init(in);
    }
}
