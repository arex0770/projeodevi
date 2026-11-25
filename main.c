#include <stdio.h>

#define MAX_SUREC 20
#define MAX_BLOK 1000
#define SATIR_BLOK 6

int gId[MAX_BLOK];   
int gBas[MAX_BLOK];
int gBit[MAX_BLOK];
int gSay = 0;
int baglam = 0;
int sonSurec = -1;

void sistem_sifirla(int n, int cpu[], int kalan[], int bitis[])
{
    int i;
    gSay = 0;
    baglam = 0;
    sonSurec = -1;
    for (i = 0; i < n; i++) {
        kalan[i] = cpu[i];
        bitis[i] = 0;
    }
}

void gantt_ekle(int id, int bas, int bit)
{
    if (gSay >= MAX_BLOK) {
        printf("HATA: Gantt blok siniri asildi!\n");
        return;
    }
    gId[gSay] = id;
    gBas[gSay] = bas;
    gBit[gSay] = bit;
    gSay++;
    if (id > 0) {
        if (sonSurec != -1 && sonSurec != id)
            baglam++;
        sonSurec = id;
    }
}

int en_erken_gelis(int n, int gelis[], int kalan[])
{
    int i, en = -1;
    for (i = 0; i < n; i++) {
        if (kalan[i] > 0 && (en == -1 || gelis[i] < en))
            en = gelis[i];
    }
    return en;
}

void bosta_ilerle(int *t, int n, int gelis[], int kalan[])
{
    int e = en_erken_gelis(n, gelis, kalan);
    if (e > *t) {
        gantt_ekle(0, *t, e);
        *t = e;
    }
}

void fcfs(int n, int gelis[], int cpu[], int bitis[], int kalan[])
{
    int sira[MAX_SUREC];
    int i, j, anahtar, p, t = 0;

    for (i = 0; i < n; i++)
        sira[i] = i;
    for (i = 1; i < n; i++) {
        anahtar = sira[i];
        j = i - 1;
        while (j >= 0 && gelis[sira[j]] > gelis[anahtar]) {
            sira[j + 1] = sira[j];
            j--;
        }
        sira[j + 1] = anahtar;
    }

    for (i = 0; i < n; i++) {
        p = sira[i];
        bosta_ilerle(&t, n, gelis, kalan);
        gantt_ekle(p + 1, t, t + cpu[p]);
        t += cpu[p];
        bitis[p] = t;
        kalan[p] = 0;
    }
}

void sjf(int n, int gelis[], int cpu[], int bitis[], int kalan[])
{
    int biten = 0, t = 0, i, sec;

    while (biten < n) {
        sec = -1;
        for (i = 0; i < n; i++) {
            if (kalan[i] > 0 && gelis[i] <= t) {
                if (sec == -1 || cpu[i] < cpu[sec] ||
                    (cpu[i] == cpu[sec] && gelis[i] < gelis[sec]))
                    sec = i;
            }
        }
        if (sec == -1) {          
            bosta_ilerle(&t, n, gelis, kalan);
            continue;
        }
        gantt_ekle(sec + 1, t, t + cpu[sec]);
        t += cpu[sec];
        bitis[sec] = t;
        kalan[sec] = 0;
        biten++;
    }
}

void round_robin(int n, int gelis[], int cpu[], int bitis[], int kalan[],
                 int zd)
{
    int biten = 0, t = 0, i, calisti, sure;

    while (biten < n) {
        calisti = 0;
        for (i = 0; i < n; i++) {
            if (kalan[i] > 0 && gelis[i] <= t) {
                sure = (kalan[i] < zd) ? kalan[i] : zd;
                gantt_ekle(i + 1, t, t + sure);
                t += sure;
                kalan[i] -= sure;
                calisti = 1;
                if (kalan[i] == 0) {
                    bitis[i] = t;
                    biten++;
                }
            }
        }
        if (!calisti && biten < n)
            bosta_ilerle(&t, n, gelis, kalan);
    }
}

void sureleri_hesapla(int n, int gelis[], int cpu[], int bitis[],
                      int donus[], int bekleme[])
{
    int i;
    for (i = 0; i < n; i++) {
        donus[i] = bitis[i] - gelis[i];
        bekleme[i] = donus[i] - cpu[i];
    }
}

void ortalamalar(int n, int bekleme[], int donus[],
                 double *ortBek, double *ortDon)
{
    int i, topBek = 0, topDon = 0;
    for (i = 0; i < n; i++) {
        topBek += bekleme[i];
        topDon += donus[i];
    }
    *ortBek = (double)topBek / n;
    *ortDon = (double)topDon / n;
}

void gantt_yazdir(void)
{
    int i;
    printf("Gantt:\n");
    for (i = 0; i < gSay; i++) {
        if (gId[i] == 0)
            printf("[BOS %d-%d]", gBas[i], gBit[i]);
        else
            printf("[P%d %d-%d]", gId[i], gBas[i], gBit[i]);
        if ((i + 1) % SATIR_BLOK == 0 || i == gSay - 1)
            printf("\n");
        else
            printf(" ");
    }
}

void tablo_yazdir(int n, int gelis[], int cpu[], int bitis[],
                  int donus[], int bekleme[])
{
    int i;
    printf("\n%-5s %-6s %-5s %-6s %-6s %-7s\n",
           "No", "Gelis", "CPU", "Bitis", "Donus", "Bekleme");
    for (i = 0; i < n; i++)
        printf("P%-4d %-6d %-5d %-6d %-6d %-7d\n",
               i + 1, gelis[i], cpu[i], bitis[i], donus[i], bekleme[i]);
}

void baslik_yazdir(char ad[])
{
    static int sira = 0;
    sira++;
    printf("\n==================== %d) %s ====================\n", sira, ad);
}

void rapor_yaz(char ad[], int n, int gelis[], int cpu[], int bitis[],
               int donus[], int bekleme[], double ortBek, double ortDon)
{
    baslik_yazdir(ad);
    gantt_yazdir();
    tablo_yazdir(n, gelis, cpu, bitis, donus, bekleme);
    printf("\nOrtalama bekleme : %.2f\n", ortBek);
    printf("Ortalama donus   : %.2f\n", ortDon);
    printf("Baglam degisimi  : %d\n", baglam);
}

void karsilastir(double ort[])
{
    int i, en = 0;
    printf("\n==================== KARSILASTIRMA ====================\n");
    printf("%-12s %-12s %-12s\n", "FCFS", "SJF", "RoundRobin");
    printf("%-12.2f %-12.2f %-12.2f\n", ort[0], ort[1], ort[2]);
    for (i = 1; i < 3; i++)
        if (ort[i] < ort[en])
            en = i;
    if (en == 0)
        printf("En dusuk ortalama bekleme: FCFS (%.2f)\n", ort[en]);
    else if (en == 1)
        printf("En dusuk ortalama bekleme: SJF (%.2f)\n", ort[en]);
    else
        printf("En dusuk ortalama bekleme: Round Robin (%.2f)\n", ort[en]);
}

int main(void)
{
    int n, zd, i, a;
    int gelis[MAX_SUREC], cpu[MAX_SUREC];
    int bitis[MAX_SUREC], kalan[MAX_SUREC];
    int donus[MAX_SUREC], bekleme[MAX_SUREC];
    double ortBek[3], ortDon[3];

    printf("Surec sayisi ve zaman dilimi (n zd): ");
    if (scanf("%d %d", &n, &zd) != 2 || n < 1 || n > MAX_SUREC || zd < 1) {
        printf("Gecersiz giris! (1 <= n <= %d, zd >= 1)\n", MAX_SUREC);
        return 1;
    }
    for (i = 0; i < n; i++) {
        printf("P%d icin gelis ve CPU suresi: ", i + 1);
        if (scanf("%d %d", &gelis[i], &cpu[i]) != 2 ||
            gelis[i] < 0 || cpu[i] < 1) {
            printf("Gecersiz giris! (gelis >= 0, cpu >= 1)\n");
            return 1;
        }
    }

    for (a = 0; a < 3; a++) {
        sistem_sifirla(n, cpu, kalan, bitis);
        if (a == 0)
            fcfs(n, gelis, cpu, bitis, kalan);
        else if (a == 1)
            sjf(n, gelis, cpu, bitis, kalan);
        else
            round_robin(n, gelis, cpu, bitis, kalan, zd);

        sureleri_hesapla(n, gelis, cpu, bitis, donus, bekleme);
        ortalamalar(n, bekleme, donus, &ortBek[a], &ortDon[a]);

        if (a == 0)
            rapor_yaz("FCFS", n, gelis, cpu, bitis, donus, bekleme,
                      ortBek[a], ortDon[a]);
        else if (a == 1)
            rapor_yaz("SJF", n, gelis, cpu, bitis, donus, bekleme,
                      ortBek[a], ortDon[a]);
        else
            rapor_yaz("Round Robin", n, gelis, cpu, bitis, donus, bekleme,
                      ortBek[a], ortDon[a]);
    }

    karsilastir(ortBek);
    return 0;
}
