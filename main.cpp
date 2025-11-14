#include <iostream>
#include <stack>
#include <deque>
#include <random>
#include <map>
#include <cmath>
#include <numeric>
#include <queue>
#include <set>
#include <algorithm>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>

using namespace std;
#define all(a) a.begin(), a.end()
#define pb push_back
#define get(v) for (int & iq : v) cin >> iq
#define give(vv) for (int & iqq : vv) cout << iqq << " "
#define vi vector <int>
#define pii pair <int, int>
#define SOLVE int t; cin >> t; while (t--) {solve();}
typedef __int128 lll;
typedef long long ll;
typedef long double ld;
#define int ll
ll inf = 1e9 + 7, mod = 1e6 + 3;
int N = 1e6;


static const int MIN_N = 500;
static const int MAX_N = 1000;
static const int STEP = 100;
static const int REPEATS = 1; // Тута мы считаем сколько запускаем раз бесполезный бред
static const int MIN_RANDOM_VALUE = 0;
static const int MAX_RANDOM_VALUE = 6000;

static const int THRESHOLDS[] = {10, 16, 30};

void insertion_sort(vector<int> &a, int l, int r) {
    for (int i = l + 1; i < r; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= l && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

void merge_sort(vector<int> &v, int l, int r) {
    if (r - l <= 1) return;
    int m = (l + r) / 2;
    merge_sort(v, l, m);
    merge_sort(v, m, r);

    vector<int> tmp;
    int i = l, j = m;
    while (i < m && j < r) {
        if (v[i] <= v[j]) tmp.push_back(v[i++]);
        else tmp.push_back(v[j++]);
    }
    while (i < m) tmp.push_back(v[i++]);
    while (j < r) tmp.push_back(v[j++]);

    for (int k = 0; k < (int) tmp.size(); k++)
        v[l + k] = tmp[k];
}

void merge_sort_hybrid(vector<int> &v, int l, int r, int threshold) {
    if (r - l <= threshold) {
        insertion_sort(v, l, r);
        return;
    }

    int m = (l + r) / 2;
    merge_sort_hybrid(v, l, m, threshold);
    merge_sort_hybrid(v, m, r, threshold);

    vector<int> tmp;
    int i = l, j = m;
    while (i < m && j < r) {
        if (v[i] <= v[j]) tmp.push_back(v[i++]);
        else tmp.push_back(v[j++]);
    }
    while (i < m) tmp.push_back(v[i++]);
    while (j < r) tmp.push_back(v[j++]);

    for (int k = 0; k < (int) tmp.size(); k++)
        v[l + k] = tmp[k];
}

// Генерация
struct ArrayGenerator {
    vector<int> base_random; // случайный
    vector<int> base_reverse; // обратный
    vector<int> base_nearly; // Почти готовый

    explicit ArrayGenerator(unsigned seed = 19) {
        generate_bases(seed);
    }

    void generate_bases(unsigned seed) {
        // случайный
        base_random.resize(MAX_N);
        mt19937 rng(seed);
        uniform_int_distribution<int> dist(MIN_RANDOM_VALUE, MAX_RANDOM_VALUE);
        for (int i = 0; i < MAX_N; ++i) base_random[i] = dist(rng);

        // обратный
        base_reverse.resize(MAX_N);
        for (int i = 0; i < MAX_N; ++i) base_reverse[i] = MAX_N - i;

        // Почти готовый
        base_nearly.resize(MAX_N);
        iota(base_nearly.begin(), base_nearly.end(), 0);
        for (int k = 0; k < MAX_N / 100; ++k) {
            int i = rng() % MAX_N;
            int j = rng() % MAX_N;
            swap(base_nearly[i], base_nearly[j]);
        }
    }

    static vector<int> take_prefix(const vector<int> &src, int n) {
        return {src.begin(), src.begin() + n};
    }
};

// Запуск
struct SortTester {
    static long long avg_ms_merge(const vector<int> &base, int n) {
        long long sum = 0;
        for (int t = 0; t < REPEATS; ++t) {
            vector<int> a = ArrayGenerator::take_prefix(base, n);
            auto start = chrono::high_resolution_clock::now();
            merge_sort(a, 0, (int) a.size());
            auto elapsed = chrono::high_resolution_clock::now() - start;
            sum += chrono::duration_cast<chrono::milliseconds>(elapsed).count();
        }
        return sum / REPEATS;
    }

    static long long avg_ms_hybrid(const vector<int> &base, int n, int threshold) {
        long long sum = 0;
        for (int t = 0; t < REPEATS; ++t) {
            vector<int> a = ArrayGenerator::take_prefix(base, n);
            auto start = chrono::high_resolution_clock::now();
            merge_sort_hybrid(a, 0, (int) a.size(), threshold);
            auto elapsed = chrono::high_resolution_clock::now() - start;
            sum += chrono::duration_cast<chrono::milliseconds>(elapsed).count();
        }
        return sum / REPEATS;
    }
};

signed main() {
#ifdef _LOCAL
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
#endif
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ArrayGenerator gen(19);
    SortTester tester;

    cout << "type, size, algo, threshold, avg_ms\n";

    auto run_group = [&](const string &type_name, const vector<int> &base_vec) {
        for (int n = MIN_N; n <= MAX_N; n += STEP) {
            long long ms_merge = tester.avg_ms_merge(base_vec, n);
            cout << type_name << ", " << n << ", merge, " << -1 << ", " << ms_merge << "\n";
            for (int th: THRESHOLDS) {
                long long ms_h = tester.avg_ms_hybrid(base_vec, n, th);
                cout << type_name << ", " << n << ", merge_insertion, " << th << ", " << ms_h << "\n";
            }
        }
    };

    run_group("random", gen.base_random);
    run_group("reverse", gen.base_reverse);
    run_group("nearly", gen.base_nearly);

    return 0;
}
