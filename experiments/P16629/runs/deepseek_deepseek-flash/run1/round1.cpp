#include <bits/stdc++.h>
using namespace std;

struct Vec {
    double x, y, z;
    Vec() {}
    Vec(double x, double y, double z): x(x), y(y), z(z) {}
    Vec operator+(const Vec& o) const { return Vec(x+o.x, y+o.y, z+o.z); }
    Vec operator-(const Vec& o) const { return Vec(x-o.x, y-o.y, z-o.z); }
    Vec operator*(double t) const { return Vec(x*t, y*t, z*t); }
};

double dot(const Vec& a, const Vec& b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
double norm(const Vec& a) {
    return sqrt(dot(a,a));
}

double distPointToLens(const Vec& C, const Vec& A, double ra, const Vec& B, double rb) {
    Vec AB = B - A;
    double D = norm(AB);
    if (D < 1e-12) {
        double d = norm(C - A);
        double r = min(ra, rb);
        return max(0.0, d - r);
    }
    Vec e = AB * (1.0 / D);
    Vec v = C - A;
    double cx = dot(v, e);
    double cy2 = dot(v,v) - cx*cx;
    if (cy2 < 0) cy2 = 0;
    double cy = sqrt(cy2);
    
    double dA = sqrt(cx*cx + cy*cy);
    double dB = sqrt((cx-D)*(cx-D) + cy*cy);
    double eps = 1e-10;
    bool inA = dA <= ra + eps;
    bool inB = dB <= rb + eps;
    if (inA && inB) return 0.0;
    double best = 1e100;
    
    // Projection on circle A
    if (dA > 1e-12) {
        double px = ra * cx / dA;
        double py = ra * cy / dA;
        double distB = sqrt((px-D)*(px-D) + py*py);
        if (distB <= rb + eps) {
            double distP = sqrt((px-cx)*(px-cx) + (py-cy)*(py-cy));
            if (distP < best) best = distP;
        }
    } else {
        double px = ra, py = 0;
        double distB = sqrt((px-D)*(px-D) + py*py);
        if (distB <= rb + eps) {
            if (ra < best) best = ra;
        }
    }
    
    // Projection on circle B
    if (dB > 1e-12) {
        double ux = cx - D, uy = cy;
        double px = D + rb * ux / dB;
        double py = rb * uy / dB;
        double distA = sqrt(px*px + py*py);
        if (distA <= ra + eps) {
            double distP = sqrt((px-cx)*(px-cx) + (py-cy)*(py-cy));
            if (distP < best) best = distP;
        }
    } else {
        double px = D - rb, py = 0;
        double distA = sqrt(px*px + py*py);
        if (distA <= ra + eps) {
            if (rb < best) best = rb;
        }
    }
    
    // Circle intersections
    if (D <= ra + rb + eps && D >= fabs(ra - rb) - eps) {
        double a = (ra*ra - rb*rb + D*D) / (2*D);
        double h2 = ra*ra - a*a;
        if (h2 < 0) h2 = 0;
        double h = sqrt(h2);
        double px = a, py = h;
        double distP = sqrt((px-cx)*(px-cx) + (py-cy)*(py-cy));
        if (distP < best) best = distP;
    }
    return best;
}

bool intersect3(const Vec& A, double ra, const Vec& B, double rb, const Vec& C, double rc) {
    double d = distPointToLens(C, A, ra, B, rb);
    return d <= rc + 1e-11;
}

int main() {
    int T;
    cin >> T;
    for (int tc=1; tc<=T; tc++) {
        Vec P[3];
        for (int i=0;i<3;i++) {
            cin >> P[i].x >> P[i].y >> P[i].z;
        }
        double lo = 0.0, hi = 10000.0;
        for (int iter=0; iter<70; iter++) {
            double mid = (lo+hi)/2;
            bool ok = false;
            // A: one sphere covers all three points
            if (intersect3(P[0], mid, P[1], mid, P[2], mid)) ok = true;
            // B: one sphere covers a pair, one covers the remaining point, third bridges
            if (!ok) {
                for (int k=0;k<3;k++) {
                    int i=(k+1)%3, j=(k+2)%3;
                    if (intersect3(P[i], mid, P[j], mid, P[k], 5*mid)) { ok=true; break; }
                }
            }
            // C: one sphere per point, connected in a path
            if (!ok) {
                for (int m=0;m<3;m++) {
                    int i=(m+1)%3, j=(m+2)%3;
                    if (intersect3(P[m], mid, P[i], 3*mid, P[j], 3*mid)) { ok=true; break; }
                }
            }
            if (ok) hi = mid;
            else lo = mid;
        }
        printf("Case #%d: %.10f\n", tc, (lo+hi)/2);
    }
    return 0;
}