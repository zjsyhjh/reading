import java.util.*;

/**
 * Created by iHge2k on 2017/3/22.
 * lca + rmq在线查询
 */

public class LCARMQ {

    //the number of vertex and edge
    private int N;
    private int M;

    //the number of edges
    private int NE;
    private int total;


    public LCARMQ(int N, int M) {
        this.N = N;
        this.M = M;
        this.NE = 0;
        this.total = 0;
        this.head = new int[N + 1];
        this.edges = new Edge[(N << 1) + 4];
        this.dep = new int[(N << 1) + 1];
        this.vis = new int[(N << 1) + 1];
        this.index = new int[N + 1];
        this.dp = new int[(N << 1) + 1][25];
        this.mark = new boolean[N + 1];
        this.dist = new int[N + 1];
        Arrays.fill(head, -1);
        Arrays.fill(mark, false);
    }

    private class Edge {
        int v, w, next;
    }

    private Edge[] edges;
    private int[] head;

    private void insert(int u, int v, int w) {
        edges[NE] = new Edge();
        edges[NE].v = v;
        edges[NE].w = w;
        edges[NE].next = head[u];
        head[u] = NE++;
    }

    private boolean[] mark;
    private int[] dist;
    private int[] dep; // 深度
    private int[] vis;
    private int[] index;

    private void dfs(int u, int d) {
        mark[u] = true;
        vis[++total] = u;
        index[u] = total;
        dep[total] = d;
        for (int i = head[u]; i != -1; i = edges[i].next) {
            if (!mark[edges[i].v]) {
                dist[edges[i].v] = dist[u] + edges[i].w;
                dfs(edges[i].v, d + 1);
                vis[++total] = u;
                dep[total] = d;
            }
        }
    }

    private int dp[][]; //dp[i][j]表示访问区间起始为i，长度为2^j的区间的最小值的下标
    private void st(int n) {
        for (int i = 1; i <= n; i++) {
            dp[i][0] = i;
        }
        for (int j = 1; (1 << j) <= n; j++) {
            for (int i = 1; i + (1 << (j - 1)) <= n; i++) {
                int l = dp[i][j - 1], r = dp[i + (1 << (j - 1))][j - 1];
                dp[i][j] = (dep[l] < dep[r] ? l : r);
            }
        }
    }

    private int rmq(int st, int ed) {
        int k = 0;
        while ((1 << (k + 1)) <= ed - st + 1) k++;
        int l = dp[st][k], r = dp[ed - (1 << k) + 1][k];
        return dep[l] < dep[r] ? l : r;
    }

    private int lca(int u, int v) {
        int st = Math.min(index[u], index[v]);
        int ed = Math.max(index[u], index[v]);
        return vis[rmq(st, ed)];
    }

    private void solve(Scanner in) {
        dist[1] = 0;
        dfs(1, 0);
        st(2 * N - 1);
        for (int i = 1; i <= M; i++) {
            int u = in.nextInt(), v = in.nextInt();
            System.out.println(dist[u] + dist[v] - 2 * dist[lca(u,v)]);
        }
    }

    private void init(Scanner in) {

        for (int i = 1; i < N; i++) {
            int u = in.nextInt();
            int v = in.nextInt();
            int w = in.nextInt();
            insert(u, v, w);
            insert(v, u, w);
        }
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int ncase = sc.nextInt();
        while (ncase-- > 0) {

            int n = sc.nextInt();
            int m = sc.nextInt();
            LCARMQ obj = new LCARMQ(n, m);

            obj.init(sc);
            obj.solve(sc);
        }
    }
}
