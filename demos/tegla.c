#include <stdio.h>
#include <math.h>

typedef struct {
    double a;
    double b;
    double c;
} Teglatest;

double terfogat(Teglatest t) {
    return t.a * t.b * t.c;
}

double felszin(Teglatest t) {
    return 2 * (t.a * t.b + t.a * t.c + t.b * t.c);
}

double atlo(Teglatest t) {
    return sqrt(t.a * t.a + t.b * t.b + t.c * t.c);
}

int main() {
    Teglatest t;
    int valasztas;

    printf("Add meg a teglatest 3 elet:\n");
    printf("a = ");
    scanf("%lf", &t.a);
    printf("b = ");
    scanf("%lf", &t.b);
    printf("c = ");
    scanf("%lf", &t.c);
    printf("A teglatest 3 ele:\n %.2lf, %.2lf, %.2lf",t.a,t.b,t.c);

    do {
        printf("\n--- Menu ---\n");
        printf("1 - terfogat\n");
        printf("2 - Felszin\n");
        printf("3 - Atlo\n");
        printf("0 - Kilepes\n");
        printf("Choose: ");
        scanf("%d", &valasztas);

        switch(valasztas) {
            case 1:
                printf("Terfogat: %.2lf\n", terfogat(t));
                break;
            case 2:
                printf("Felszin: %.2lf\n", felszin(t));
                break;
            case 3:
                printf("Atlo: %.2lf\n", atlo(t));
                break;
            case 0:
                printf("Quite...\n");
                break;
            default:
                printf("Miss!\n");
        }

    } while(valasztas != 0);

    return 0;
}