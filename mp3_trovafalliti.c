#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <mpg123.h>
#include <unistd.h>

// Funzione per controllare se un file ha estensione ".mp3"
int is_mp3(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return 0;
    return strcmp(dot, ".mp3") == 0;
}

// Funzione per verificare la durata di un file MP3
int check_mp3_duration(const char *filename) {
    mpg123_handle *mh;
    int err;
    long rate;
    int channels, encoding;
    off_t length;  // Numero di frame nel file MP3

    // Inizializza mpg123
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    if (mh == NULL) {
        printf("Errore nell'inizializzare mpg123: %s\n", mpg123_plain_strerror(err));
        return 0;
    }

    // Tenta di aprire il file MP3
    if (mpg123_open(mh, filename) != MPG123_OK) {
        mpg123_close(mh);
        mpg123_delete(mh);
        mpg123_exit();
        return 0; // Non può aprire il file
    }

    // Ottieni il formato del file
    if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
        mpg123_close(mh);
        mpg123_delete(mh);
        mpg123_exit();
        return 0; // Errore nel formato
    }

    // Ottieni la lunghezza del file MP3 in frame
    length = mpg123_length(mh);
    if (length == MPG123_ERR) {
        mpg123_close(mh);
        mpg123_delete(mh);
        mpg123_exit();
        return 0; // Errore nel calcolo della lunghezza
    }

    // Calcola la durata del file in secondi
    double duration = (double)length / rate;

    // Chiudi e rilascia le risorse
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();

    // Controlla se la durata è inferiore a 1 secondo
    if (duration < 1.0) {
        return 0; // File fallito
    }

    return 1; // File valido
}

int main() {
    struct dirent* de;  // Puntatore a struttura che contiene i dati del file
    char cwd[1024];  // Buffer per la directory corrente

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

    // Ciclo per leggere tutti i file nella directory
    printf("Controllo file MP3...\n");
    int failed_count = 0;
    int song_number = 1;

    while ((de = readdir(dr)) != NULL) {
        if (is_mp3(de->d_name)) {
            // Verifica se il file MP3 ha una durata valida
            if (!check_mp3_duration(de->d_name)) {
                // Stampa solo i file falliti con numerazione
                printf("%d: %s (durata inferiore a 1 secondo)\n", song_number++, de->d_name);
                failed_count++;
            }
        }
    }

    // Chiudi la directory
    closedir(dr);

    // Stampa il totale dei file falliti
    if (failed_count > 0) {
        printf("\nTotale file falliti: %d\n", failed_count);
    } else {
        printf("Tutti i file MP3 sono validi!\n");
    }

    return 0;
}
