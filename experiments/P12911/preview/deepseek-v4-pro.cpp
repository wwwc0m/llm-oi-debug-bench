#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long n;
    if(!(cin >> n)) return 0;

    if(n == 3){
        cout << "3\n";
        cout << "##.\n";
        cout << "...\n";
        cout << ".#.\n";
        return 0;
    }

    if(n % 2 == 0){
        long long m = n / 2;
        long long k = m + 1;
        cout << k << "\n";

        string row0(k, '.');
        row0[(size_t)m] = '#';
        cout << row0 << "\n";

        for(long long r = 1; r <= m - 2; ++r){
            cout << string(k, '.') << "\n";
        }

        string rowA(k, '.');
        for(long long c = 0; c <= m - 2; ++c) rowA[(size_t)c] = '#';
        cout << rowA << "\n";

        string rowB(k, '.');
        for(long long c = 0; c <= m - 1; ++c) rowB[(size_t)c] = '#';
        cout << rowB << "\n";
    } else {
        // n odd, n >= 5
        long long m = (n + 1) / 2;
        long long k = m + 1;
        cout << k << "\n";

        string row0(k, '.');
        row0[(size_t)m] = '#';
        cout << row0 << "\n";

        for(long long r = 1; r <= m - 2; ++r){
            cout << string(k, '.') << "\n";
        }

        string rowA(k, '.');
        for(long long c = 0; c <= m - 2; ++c) rowA[(size_t)c] = '#';
        cout << rowA << "\n";

        string rowB(k, '.');
        for(long long c = 1; c <= m - 1; ++c) rowB[(size_t)c] = '#';
        cout << rowB << "\n";
    }

    return 0;
}
