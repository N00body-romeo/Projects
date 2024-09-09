#include <stdio.h>
#include <stdlib.h>
#include <math.h>  // Per fabs()

typedef struct {
    char name[20];
    double money;
    double diff;  // Differenza tra quanto ha versato e quanto dovrebbe versare
} soggetto;

int main() {
    int num;
    double prize;

    printf("Inserisci il numero di partecipanti: ");
    scanf("%d", &num);

    printf("Inserisci il prezzo totale del regalo: ");
    scanf("%lf", &prize);

    soggetto *partecipanti = malloc(num * sizeof(soggetto));

    for (int i = 0; i < num; i++) {
        printf("Inserisci nome del partecipante: ");
        scanf("%s", partecipanti[i].name);

        printf("Inserisci quanto ha versato: ");
        scanf("%lf", &partecipanti[i].money);
    }

    double total = prize / num;

    printf("Costo del regalo: %.2f\n", prize);
    printf("Quindi ognuno deve %.2f euro per il regalo\n", total);

    // Calcoliamo la differenza tra quanto dovrebbero versare e quanto hanno versato
    for (int i = 0; i < num; i++) {
        partecipanti[i].diff = partecipanti[i].money - total; // Positivo se ha pagato di più, negativo se di meno
    }

    printf("Differenze rispetto alla quota dovuta:\n");
    for (int i = 0; i < num; i++) {
        printf("%s: %.2f euro\n", partecipanti[i].name, partecipanti[i].diff);
    }

    // Dobbiamo ora bilanciare i debitori e i creditori
    for (int i = 0; i < num; i++) {
        if (partecipanti[i].diff < 0) { // Il partecipante deve pagare di più
            double debt = fabs(partecipanti[i].diff); // Quantità che deve pagare

            for (int j = 0; j < num && debt > 0; j++) {
                if (partecipanti[j].diff > 0) { // Questo partecipante deve ricevere soldi
                    double credit = partecipanti[j].diff;

                    if (credit >= debt) { // Il creditore può coprire tutto il debito
                        printf("%s deve dare %.2f euro a %s\n", partecipanti[i].name, debt, partecipanti[j].name);
                        partecipanti[j].diff -= debt;
                        partecipanti[i].diff = 0;
                        debt = 0;
                    } else { // Il creditore non può coprire tutto il debito, quindi paga quanto può
                        printf("%s deve dare %.2f euro a %s\n", partecipanti[i].name, credit, partecipanti[j].name);
                        partecipanti[i].diff += credit; // Riduce il debito
                        partecipanti[j].diff = 0; // Il creditore ha ricevuto tutto
                        debt -= credit;
                    }
                }
            }
        }
    }

    free(partecipanti);  // Libera la memoria allocata
    return 0;
}
