#define M 20000

volatile double m[M][M]; 

double sumMatrix(volatile double m[M][M]) {
    double sum = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            sum += m[j][i];
        }
    }
    return sum;
}

int main() {
    return sumMatrix(m) > 42.0;
}