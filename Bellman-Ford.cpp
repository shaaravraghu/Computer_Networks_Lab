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
        cout << "Error: invalid number of routers (1 to " << MAXN << ")" << endl;
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

    cout << "Enter directed links (from to cost):" << endl;
    for (int i = 0; i < m; i++) {
        cin.width(NAMELEN);
        cin >> en[i][0];
        cin.width(NAMELEN);
        cin >> en[i][1];
        if (!(cin >> ew[i])) {
            cout << "Error: invalid link entry" << endl;
            return 1;
        }
        for (int s = 0; s < 2; s++) {
            if (findName(names, cnt, en[i][s]) < 0) {
                if (cnt >= n) {
                    cout << "Error: more distinct routers than declared" << endl;
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

    static int eu[MAXE], ev[MAXE];
    for (int i = 0; i < m; i++) {
        eu[i] = findName(names, n, en[i][0]);
        ev[i] = findName(names, n, en[i][1]);
    }

    int src = findName(names, n, srcName);
    long long dist[MAXN];
    int pred[MAXN];
    bool neg[MAXN];
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        pred[i] = -1;
        neg[i] = false;
    }
    dist[src] = 0;

    for (int pass = 1; pass <= n - 1; pass++) {
        bool changed = false;
        for (int i = 0; i < m; i++) {
            if (dist[eu[i]] != INF && dist[eu[i]] + ew[i] < dist[ev[i]]) {
                dist[ev[i]] = dist[eu[i]] + ew[i];
                pred[ev[i]] = eu[i];
                changed = true;
            }
        }
        if (!changed) break;
    }

    bool cycle = false;
    for (int pass = 0; pass < n; pass++) {
        for (int i = 0; i < m; i++) {
            int u = eu[i], v = ev[i];
            if (dist[u] == INF) continue;
            if (neg[u] || dist[u] + ew[i] < dist[v]) {
                if (!neg[v]) neg[v] = true;
            }
        }
    }
    for (int i = 0; i < n; i++)
        if (neg[i]) cycle = true;

    cout << endl;
    if (cycle) {
        cout << "Negative-weight cycle: Detected" << endl;
        cout << "Warning: shortest paths to affected routers are not well-defined." << endl;
        return 1;
    } else {
        cout << "Negative-weight cycle: Not detected" << endl;
    }

    cout << "Destination\tShortest Path" << endl;
    for (int i = 0; i < n; i++) {
        cout << "   " << names[i] << "            ";
        if (neg[i]) {
            cout << "Not well-defined" << endl;
            continue;
        }
        if (dist[i] == INF) {
            cout << "Unreachable" << endl;
            continue;
        }
        int stack[MAXN];
        int top = 0;
        int cur = i;
        while (cur != -1 && top < MAXN) {
            stack[top++] = cur;
            cur = pred[cur];
        }
        for (int j = top - 1; j >= 0; j--) {
            cout << names[stack[j]];
            if (j > 0) cout << " -> ";
        }
        cout << "\t\t- Cost = " << dist[i] << endl;
    }
    return 0;
}
