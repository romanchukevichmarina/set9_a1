#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <functional>
using namespace std;
long long charComparisons = 0;

bool lessString(const string &a, const string &b) {
    size_t n = min(a.size(), b.size());

    for (size_t i = 0; i < n; ++i) {
        charComparisons++;

        if (a[i] < b[i]) return true;
        if (a[i] > b[i]) return false;
    }

    charComparisons++;

    return a.size() < b.size();
}

class StringGenerator {
public:
    static const string ALPHABET;
    static mt19937 rng;
    static string randomString(size_t minLen, size_t maxLen) {
        uniform_int_distribution<size_t> lenDist(minLen, maxLen);
        uniform_int_distribution<size_t> charDist(0, ALPHABET.size() - 1);
        size_t len = lenDist(rng);
        string s;
        s.reserve(len);
        for (size_t i = 0; i < len; ++i) s.push_back(ALPHABET[charDist(rng)]);
        return s;
    }
    static vector<string> generateBaseRandom(size_t size) {
        vector<string> v;
        v.reserve(size);
        for (size_t i = 0; i < size; ++i) v.push_back(randomString(10, 200));
        return v;
    }
    static vector<string> subArray(const vector<string> &base, size_t n) {
        return vector<string>(base.begin(), base.begin() + n);
    }
    static vector<string> makeReversed(const vector<string> &base) {
        vector<string> v = base;
        sort(v.begin(), v.end(), [](const string &a, const string &b) {
            return lessString(b, a);
        });
        return v;
    }
    static vector<string> makeNearlySorted(const vector<string> &base, double frac = 0.05) {
        vector<string> v = base;
        sort(v.begin(), v.end(), lessString);
        size_t swaps = max<size_t>(1, static_cast<size_t>(v.size() * frac));
        uniform_int_distribution<size_t> idDist(0, v.size() - 1);
        for (size_t i = 0; i < swaps; ++i) {
            size_t a = idDist(rng), b = idDist(rng);
            swap(v[a], v[b]);
        }
        return v;
    }
};

const string StringGenerator::ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-";
mt19937 StringGenerator::rng(random_device{}());

class SortingAlgorithms {
    static void mergeRanges(vector<string> &a, vector<string> &t, int l, int m, int r) {
        int i = l, j = m + 1, k = 0;
        while (i <= m && j <= r) {
            if (!lessString(a[j], a[i])) t[k++] = a[i++];
            else t[k++] = a[j++];
        }
        while (i <= m) t[k++] = a[i++];
        while (j <= r) t[k++] = a[j++];
        for (int x = 0; x < k; ++x) a[l + x] = move(t[x]);
    }
    static void mergeRec(vector<string> &a, vector<string> &t, int l, int r) {
        if (l >= r) return;
        int m = l + (r - l) / 2;
        mergeRec(a, t, l, m);
        mergeRec(a, t, m + 1, r);
        mergeRanges(a, t, l, m, r);
    }
    static int charAtQuick(const string &s, int d) {
        return d < (int)s.size() ? (int)(unsigned char)s[d] : -1;
    }
    static void quickRec(vector<string> &a, int lo, int hi, int d) {
        if (hi <= lo) return;
        int lt = lo, gt = hi;
        int v = charAtQuick(a[lo], d);
        int i = lo + 1;
        while (i <= gt) {
            int t = charAtQuick(a[i], d);
            if (t < v) { swap(a[lt], a[i]); lt++; i++; }
            else if (t > v) { swap(a[i], a[gt]); gt--; }
            else i++;
        }
        quickRec(a, lo, lt - 1, d);
        if (v >= 0) quickRec(a, lt, gt, d + 1);
        quickRec(a, gt + 1, hi, d);
    }
    static int charAtRadix(const string &s, int d) {
        return d < (int)s.size() ? (int)(unsigned char)s[d] + 1 : 0;
    }
    static void msdRec(vector<string> &a, vector<string> &aux, int lo, int hi, int d) {
        if (hi <= lo) return;
        const int R = 256;
        vector<int> count(R + 2, 0);
        for (int i = lo; i <= hi; ++i) {
            int c = charAtRadix(a[i], d);
            count[c + 1]++;
        }
        for (int r = 0; r <= R; ++r) count[r + 1] += count[r];
        vector<int> start = count;
        for (int i = lo; i <= hi; ++i) {
            int c = charAtRadix(a[i], d);
            aux[start[c]++] = move(a[i]);
        }
        for (int i = lo; i <= hi; ++i) a[i] = move(aux[i - lo]);
        for (int r = 1; r <= R; ++r) {
            int sub_lo = lo + count[r];
            int sub_hi = lo + count[r + 1] - 1;
            if (sub_lo < sub_hi) msdRec(a, aux, sub_lo, sub_hi, d + 1);
        }
    }
    static void msdRecQuick(vector<string> &a, vector<string> &aux, int lo, int hi, int d, int threshold) {
        if (hi - lo + 1 < threshold) {
            quickRec(a, lo, hi, d);
            return;
        }
        const int R = 256;
        vector<int> count(R + 2, 0);
        for (int i = lo; i <= hi; ++i) {
            int c = charAtRadix(a[i], d);
            count[c + 1]++;
        }
        for (int r = 0; r <= R; ++r) count[r + 1] += count[r];
        vector<int> start = count;
        for (int i = lo; i <= hi; ++i) {
            int c = charAtRadix(a[i], d);
            aux[start[c]++] = move(a[i]);
        }
        for (int i = lo; i <= hi; ++i) a[i] = move(aux[i - lo]);
        for (int r = 1; r <= R; ++r) {
            int sub_lo = lo + count[r];
            int sub_hi = lo + count[r + 1] - 1;
            if (sub_lo < sub_hi) msdRecQuick(a, aux, sub_lo, sub_hi, d + 1, threshold);
        }
    }
public:
    static void mergeSort(vector<string> &a) {
        if (a.empty()) return;
        vector<string> t(a.size());
        mergeRec(a, t, 0, (int)a.size() - 1);
    }
    static void quickSort(vector<string> &a) {
        if (!a.empty()) quickRec(a, 0, (int)a.size() - 1, 0);
    }
    static void msdRadixSort(vector<string> &a) {
        if (a.empty()) return;
        vector<string> aux(a.size());
        msdRec(a, aux, 0, (int)a.size() - 1, 0);
    }
    static void msdRadixQuickSort(vector<string> &a) {
        if (a.empty()) return;
        vector<string> aux(a.size());
        msdRecQuick(a, aux, 0, (int)a.size() - 1, 0, 74);
    }
};

struct Metrics {
    double time;
    long long comparisons;
};

static Metrics measureTime(const vector<string> &arr, function<void(vector<string>&)> sortFunc, int runs = 3) {
    double totalTime = 0.0;
    long long totalCmp = 0;
    for (int i = 0; i < runs; ++i) {
        vector<string> copy = arr;
        charComparisons = 0;
        auto start = chrono::high_resolution_clock::now();
        sortFunc(copy);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        totalTime += elapsed.count();
        totalCmp += charComparisons;
    }
    return {
        totalTime / runs,
        totalCmp / runs
    };
}

int main() {
    vector<size_t> sizes;
    for (size_t n = 100; n <= 3000; n += 100) sizes.push_back(n);
    ofstream csv("results.csv");
    vector<pair<string, function<void(vector<string>&)>>> algorithms = {
        {"MergeSort", SortingAlgorithms::mergeSort},
        {"QuickSort", SortingAlgorithms::quickSort},
        {"MSDRadix", SortingAlgorithms::msdRadixSort},
        {"MSDRadixQuick", SortingAlgorithms::msdRadixQuickSort}
    };
    csv << "Size";
    for (auto &alg : algorithms) {
        csv << ","
    << alg.first << "-Random-Time"
    << "," << alg.first << "-Random-Cmp"
    << "," << alg.first << "-Reversed-Time"
    << "," << alg.first << "-Reversed-Cmp"
    << "," << alg.first << "-Nearly-Time"
    << "," << alg.first << "-Nearly-Cmp";
    }
    csv << "\n";
    size_t maxSize = 3000;
    auto baseRandom = StringGenerator::generateBaseRandom(maxSize);
    auto baseReversedFull = StringGenerator::makeReversed(baseRandom);
    auto baseNearlyFull = StringGenerator::makeNearlySorted(baseRandom);
    for (size_t sz : sizes) {
        auto randSub = StringGenerator::subArray(baseRandom, sz);
        auto revSub = StringGenerator::subArray(baseReversedFull, sz);
        auto nearSub = StringGenerator::subArray(baseNearlyFull, sz);
        csv << sz;
        for (auto &alg : algorithms) {
            Metrics tr = measureTime(randSub, alg.second, 3);
            Metrics trev = measureTime(revSub, alg.second, 3);
            Metrics tnearly = measureTime(nearSub, alg.second, 3);
            csv << "," << tr.time << "," << tr.comparisons << "," << trev.time << "," << trev.comparisons << "," << tnearly.time << "," << tnearly.comparisons;
        }
        csv << "\n";
    }
    return 0;
}