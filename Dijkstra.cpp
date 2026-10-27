#include <iostream>
using namespace std;

const int MAXN = 100;
const int MAXE = 1000;
const int NAMELEN = 32;
const long long INF = 1000000000000000000LL;

int strLen(const char *s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

void strCopy(char *d, const char *s) {
    int i = 0;
    while (s[i]) {
        d[i] = s[i];
        i++;
    }
    d[i] = 0;
}

int strCompare(const char *a, const char *b) {
    int la = strLen(a), lb = strLen(b);
    if (la != lb) return la - lb;
    int i = 0;
    while (a[i] && a[i] == b[i]) i++;
    return (unsigned char)a[i] - (unsigned char)b[i];
}

int findName(char names[][NAMELEN], int cnt, const char *name) {
    for (int i = 0; i < cnt; i++)
        if (strCompare(names[i], name) == 0) return i;
    return -1;
}

void makeName(char *out, int k) {
    if (k < 26) {
        out[0] = 'A' + k;
        out[1] = 0;
        return;
    }
    out[0] = 'R';
    char tmp[12];
    int t = 0;
    int v = k;
    while (v > 0) {
        tmp[t++] = '0' + v % 10;
        v /= 10;
    }
    int p = 1;
    while (t > 0) out[p++] = tmp[--t];
    out[p] = 0;
}

int main() {
    int n, m;
    cout << "Number of routers: ";
    if (!(cin >> n) || n <= 0 || n > MAXN) {
        cout << "Error: invalid number of routers (1 to " << MAXN << ")" << endl                                                                                        ;
        return 1;
    }
    cout << "Number of links: ";
    if (!(cin >> m) || m < 0 || m > MAXE) {
        cout << "Error: invalid number of links (0 to " << MAXE << ")" << endl;
        return 1;
    }

    static char en[MAXE][2][NAMELEN];
    static long long ew[MAXE];
    static char names[MAXN][NAMELEN];
    int cnt = 0;

    cout << "Enter links (router router cost):" << endl;
    for (int i = 0; i < m; i++) {
        cin.width(NAMELEN);
        cin >> en[i][0];
        cin.width(NAMELEN);
        cin >> en[i][1];
        if (!(cin >> ew[i])) {
            cout << "Error: invalid link entry" << endl;
            return 1;
        }
        if (ew[i] < 0) {
            cout << "Error: negative edge weight rejected (" << en[i][0] << " "                                                                                         << en[i][1] << " " << ew[i] << ")" << endl;
            return 1;
        }
        for (int s = 0; s < 2; s++) {
            if (findName(names, cnt, en[i][s]) < 0) {
                if (cnt >= n) {
                    cout << "Error: more distinct routers than declared" << endl                                                                                        ;
                    return 1;
                }
                strCopy(names[cnt++], en[i][s]);
            }
        }
    }

    char srcName[NAMELEN];
    cout << "Source router: ";
    cin.width(NAMELEN);
    if (!(cin >> srcName)) {
        cout << "Error: source router missing" << endl;
        return 1;
    }
    if (findName(names, cnt, srcName) < 0) {
        if (cnt >= n) {
            cout << "Error: source router not in network" << endl;
            return 1;
        }
        strCopy(names[cnt++], srcName);
    }

    int k = 0;
    while (cnt < n) {
        char cand[NAMELEN];
        makeName(cand, k++);
        if (findName(names, cnt, cand) < 0) strCopy(names[cnt++], cand);
    }

    for (int i = 0; i < n - 1; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++)
            if (strCompare(names[j], names[best]) < 0) best = j;
        if (best != i) {
            char tmp[NAMELEN];
            strCopy(tmp, names[i]);
            strCopy(names[i], names[best]);
            strCopy(names[best], tmp);
        }
    }

    static long long mat[MAXN][MAXN];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) mat[i][j] = -1;

    for (int i = 0; i < m; i++) {
        int u = findName(names, n, en[i][0]);
        int v = findName(names, n, en[i][1]);
        if (u == v) continue;
        if (mat[u][v] < 0 || ew[i] < mat[u][v]) {
            mat[u][v] = ew[i];
            mat[v][u] = ew[i];
        }
    }

    int src = findName(names, n, srcName);
    long long dist[MAXN];
    int parent[MAXN];
    bool done[MAXN];
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        parent[i] = -1;
        done[i] = false;
    }
    dist[src] = 0;

    for (int it = 0; it < n; it++) {
        int u = -1;
        for (int i = 0; i < n; i++)
            if (!done[i] && dist[i] < INF && (u == -1 || dist[i] < dist[u])) u =                                                                                         i;
        if (u == -1) break;
        done[u] = true;
        for (int v = 0; v < n; v++) {
            if (mat[u][v] >= 0 && !done[v] && dist[u] + mat[u][v] < dist[v]) {
                dist[v] = dist[u] + mat[u][v];
                parent[v] = u;
            }
        }
    }

    cout << endl << "Destination\tShortest Path" << endl;
    for (int i = 0; i < n; i++) {
        cout << "   " << names[i] << "            ";
        if (dist[i] >= INF) {
            cout << "Unreachable" << endl;
            continue;
        }
        int stack[MAXN];
        int top = 0;
        int cur = i;
        while (cur != -1) {
            stack[top++] = cur;
            cur = parent[cur];
        }
        for (int j = top - 1; j >= 0; j--) {
            cout << names[stack[j]];
            if (j > 0) cout << " -> ";
        }
        cout << "\t\t- Cost = " << dist[i] << endl;
    }
    return 0;
}
