#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <math.h>

// Funzione per controllare se un file ha estensione ".mp3"
int is_mp3(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return 0;
    return strcmp(dot, ".mp3") == 0;
}

// Funzione per rimuovere una sottostringa da una stringa
void remove_substring(char *str, const char *sub) {
    char *pos;
    while ((pos = strstr(str, sub)) != NULL) {
        memmove(pos, pos + strlen(sub), strlen(pos + strlen(sub)) + 1);
    }
}

// Funzione per pulire il nome del file rimuovendo stringhe specifiche
void clean_filename(char *filename) {
    remove_substring(filename, "spotifydown.com");
    remove_substring(filename, "[SPOTIFY-DOWNLOADER.COM]");
    remove_substring(filename, "SpotifyMate.com");
}

// Funzione Levenshtein per calcolare la distanza tra due stringhe
int levenshtein(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int matrix[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            if (i == 0) {
                matrix[i][j] = j;
            } else if (j == 0) {
                matrix[i][j] = i;
            } else {
                int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
                matrix[i][j] = fmin(fmin(
                    matrix[i - 1][j] + 1,  // Cancellazione
                    matrix[i][j - 1] + 1), // Inserimento
                    matrix[i - 1][j - 1] + cost);  // Sostituzione
            }
        }
    }

    return matrix[len1][len2];
}

// Funzione per chiedere quale duplicato cancellare
void delete_duplicate(const char *file1, const char *file2) {
    int choice;
    printf("Possibili duplicati trovati:\n");
    printf("1: %s\n", file1);
    printf("2: %s\n", file2);
    printf("Quale vuoi cancellare (1 o 2)? ");
    scanf("%d", &choice);

    if (choice == 1) {
        remove(file1);
        printf("%s eliminato.\n", file1);
    } else if (choice == 2) {
        remove(file2);
        printf("%s eliminato.\n", file2);
    } else {
        printf("Scelta non valida.\n");
    }
}

int main() {
    struct dirent* de;  // Puntatore a struttura che contiene i dati del file
    char cwd[1000024];  // Buffer per la directory corrente

    // Ottieni e stampa la directory corrente
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Directory corrente: %s\n", cwd);
    } else {
        perror("getcwd() errore");
        return 1;
    }

    // Apri la directory corrente
    DIR* dr = opendir(".");
    if (dr == NULL) {
        printf("Errore nell'aprire la directory.\n");
        return 1;
    }

    // Elenco di file mp3
    char *mp3_files[1000];  // Array per memorizzare fino a 100 file mp3
    int mp3_count = 0;     // Conta i file mp3

    // Ciclo per leggere tutti i file nella directory
    while ((de = readdir(dr)) != NULL) {
        if (is_mp3(de->d_name)) {
            mp3_files[mp3_count] = strdup(de->d_name);     // Memorizza il nome del file
            mp3_count++;
        }
    }

    // Chiudi la directory dopo averla letta
    closedir(dr);

    // Confronto dei file per trovare duplicati
    for (int i = 0; i < mp3_count; i++) {
        // Pulisci i nomi dei file rimuovendo le stringhe specifiche
        char clean_file1[1024];
        strcpy(clean_file1, mp3_files[i]);
        clean_filename(clean_file1);

        for (int j = i + 1; j < mp3_count; j++) {
            // Pulisci il secondo nome del file
            char clean_file2[1024];
            strcpy(clean_file2, mp3_files[j]);
            clean_filename(clean_file2);

            // Calcola la distanza di Levenshtein tra i nomi dei file puliti
            int dist = levenshtein(clean_file1, clean_file2);
            int len1 = strlen(clean_file1);
            int len2 = strlen(clean_file2);
            
            // Stampa di debug per la distanza calcolata
            printf("Distanza tra %s e %s: %d\n", clean_file1, clean_file2, dist);

            // Se la distanza è inferiore al 90% della lunghezza del nome, considerali duplicati
            if (dist <= (fmin(len1, len2) * 0.1)) {  // 90% di similarità
                // Chiedi all'utente quale duplicato eliminare
                delete_duplicate(mp3_files[i], mp3_files[j]);
            }
        }
    }

    // Libera la memoria allocata
    for (int i = 0; i < mp3_count; i++) {
        free(mp3_files[i]);
    }

    return 0;
}
