
//
// Created by ezio on 19/08/20.
//

#include "strutture_dati.h"
#include "rngs.h"
#include <math.h>
#include <string.h>

void stampa_num_eventi(int num);

//-------------------------------------------------------
//funzioni utili
//-------------------------------------------------------

double Exponential(double media, int shift){

    if(shift){
        //1, 3, 5, 7, 9...
        SelectStream(0);
        return -media*log(1.0-Random()) +1;
    }
    //0, 2, 4, 6, 8, 10...
    SelectStream(1);
    float rand = Random();

    //printf("%f\n", -media*log(1.0-Random()));
    return -media*log(1.0-rand);
}


char *secondi_ora(int secondi){

    int ora = secondi/(60*60);
    int minuti = (secondi-ora*60*60)/60;
    int secs = secondi-ora*60*60-minuti*60;


    char *ora_str = malloc(3);
    char *min_str = malloc(3);
    char *sec_str = malloc(3);
    char *all_str = malloc(10);

    if(minuti == 0){
        sprintf(min_str, "00");
    }else if(minuti < 10){
        sprintf(min_str, "0%d", minuti);
    }else{
        sprintf(min_str, "%d", minuti);
    }

    if(secs == 0){
        sprintf(sec_str, "00");
    }else if(secs < 10){
        sprintf(sec_str, "0%d", secs);
    }else{
        sprintf(sec_str, "%d", secs);
    }

    if(ora < 10){
        sprintf(ora_str, "0%d", ora);
    }else{
        sprintf(ora_str, "%d", ora);
    }

    sprintf(all_str, "%s:%s:%s", ora_str, min_str, sec_str);

    free(ora_str);
    free(min_str);
    free(sec_str);

    return all_str;
}



//-------------------------------------------------------
//funzioni per la gestione dei clienti
//-------------------------------------------------------

//genera il numero di oggetti secondo un esponenziale
//a media.. vedere relazione
int get_num_oggetti(){

    int exp = Exponential(Xn, 1);

    return exp;
}

//un cliente è generato a partire da un evento di tipo arrivo,
//per la generazione è passata l'ora associato all'evento di arrivo.
struct cliente* genera_cliente(int ora_evento_arrivo){

    struct cliente *cli = (struct cliente *)malloc(sizeof(struct cliente));
    cli->num_oggetti = get_num_oggetti();
    cli->in_fila = ora_evento_arrivo;

    cli->id = arrivi_totali;

    return cli;
}


//-------------------------------------------------------
//funzione per la gestione delle file (fila_cassa)
//-------------------------------------------------------

struct fila_cassa *crea_fila(){

    struct fila_cassa *fc = (struct fila_cassa *)malloc(sizeof(struct fila_cassa));
    fc->cliente_in_fila = NULL;
    fc->next = NULL;
    fc->id = file_ids;
    file_ids ++;
    return fc;
}


int lunghezza_fila(struct fila_cassa *fila){

    int l = 0;
    struct fila_cassa *f = fila;

    while(f->cliente_in_fila != NULL){
        l++;
        if(f->next == NULL){
            break;
        }
        f = f->next;
    }

    return l;
}


//-------------------------------------------------------
//funzione per la gestione delle casse
//-------------------------------------------------------

//ritorna delle casse, ovvero un determinato numero di file
struct casse *genera_set_casse(int num_casse){

    struct casse *casse = (struct casse *)malloc(sizeof(struct casse));

    //crea le file per ogni cassa
    int i = 0;
    struct casse *c;
    for(c = casse; ;c = c->next) {
        c->fila_cassa = crea_fila();
        i++;
        if (i < num_casse) {
            c->next = (struct casse *) malloc(sizeof(struct casse));
        }else{
            c->next = NULL;
            break;
        }

    }

    return casse;
}



int numero_casse(struct casse *casse){

    int l = 0;
    for(struct casse *c = casse; c != NULL; c = c->next ){
        l++;
    }
    return l;
}


//possibilità di rimuovere una cassa ad una configurazione esistente
void rimuovi_cassa(struct casse *casse){
    //necessario ?
}


//-------------------------------------------------------
//funzione per la gestione delle configurazioni di cassa
//-------------------------------------------------------

//aggiunge una configurazione di un dato tipo alle configurazioni attive (VARIABILE GLOBALE )
//casse_casuali_in_mista rappresenta quante casse delle 'numero_casse' sono dedicate alla
//configurazione pseudo casuale, il resto andranno alla selettiva.

//usando tale funzione possiamo 'eliminare la configurazione di cassa mista', in quanto basterebbe aggiungere
//una configurazione pseudocasuale e poi una selettiva. Potremmo addirittura fare configurazioni
//non esposte nella relazione, come pseudocasuale + selettiva, pseudo casuale + random.....
int aggiungi_configurazione_cassa(int tipo, int numero_casse, int casse_casuali_in_mista){

    if((tipo != condivisa && tipo != pseudo_casuale && tipo != selettiva && tipo != mista) || numero_casse == 0){
        printf("Input errati\n");
        return -1;
    }

    if( tipo == mista ){

        if(numero_casse-casse_casuali_in_mista <= 0){
            printf("Input errati\n");
            return -1;
        }

        if(casse_casuali_in_mista > 0){
            aggiungi_configurazione_cassa(pseudo_casuale, casse_casuali_in_mista, -1);
        }

        aggiungi_configurazione_cassa(selettiva, numero_casse-casse_casuali_in_mista, -1);

        D(printf("Configurazione di cassa mista aggiunta\n"));

        return 0;

    }

    struct config_cassa_attive **current = &config_attive;

    if(*current != NULL) {
        do{
            current = &(((struct config_cassa_attive *)*current)->next);
        }while(*current != NULL);
    }

    *current = (struct config_cassa_attive *)malloc(sizeof(struct config_cassa_attive));

    ((struct config_cassa_attive *)*current)->next = NULL;
    ((struct config_cassa_attive *)*current)->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
    struct config_cassa **nuova_configurazione = &((struct config_cassa_attive *)*current)->configurazione_cassa;


    int legg = 0, med = 0, pes = 0;

    switch(tipo){
        case condivisa:

            ((struct config_cassa *)*nuova_configurazione)->tipo = condivisa;
            ((struct config_cassa *)*nuova_configurazione)->casse = genera_set_casse(numero_casse);
            ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = crea_fila();

            D(printf("Configurazione di cassa condivisa aggiunta\n"));

            break;
        case pseudo_casuale:

            //gestire poi in modo che ogni cassa generata gestisca un cliente alla volta,
            //prelevandolo dalla fila condivisa
            ((struct config_cassa *)*nuova_configurazione)->tipo = pseudo_casuale;
            ((struct config_cassa *)*nuova_configurazione)->casse = genera_set_casse(numero_casse);
            ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = NULL;

            D(printf("Configurazione di cassa pseudo-casuale aggiunta\n"));

            break;

        case selettiva:
            //incremento in modo prioridatio dalla cassa leggera alla pesante
            //il numero di casse date loro a disposizione
            do{
                legg++;
                if(numero_casse-legg-med-pes == 0){
                    break;
                }
                med++;
                if(numero_casse-legg-med-pes == 0){
                    break;
                }
                pes++;
                if(numero_casse-legg-med-pes == 0) {
                    break;
                }
            }while(1);

            ((struct config_cassa *)*nuova_configurazione)->tipo = selettiva;
            ((struct config_cassa *)*nuova_configurazione)->sotto_tipo = selettiva_leggera;
            ((struct config_cassa *)*nuova_configurazione)->casse = genera_set_casse(legg);
            ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = NULL;

            D(printf("Configurazione di cassa selettiva-leggera aggiunta\n"));

            if(med > 0) {
                (((struct config_cassa_attive *)*current)->next) = (struct config_cassa_attive *) malloc(sizeof(struct config_cassa_attive));
                current = &(((struct config_cassa_attive *)*current)->next);

                ((struct config_cassa_attive *)*current)->next = NULL;
                ((struct config_cassa_attive *)*current)->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
                nuova_configurazione = &((struct config_cassa_attive *)*current)->configurazione_cassa;

                ((struct config_cassa *)*nuova_configurazione)->tipo = selettiva;
                ((struct config_cassa *)*nuova_configurazione)->sotto_tipo = selettiva_media;
                ((struct config_cassa *)*nuova_configurazione)->casse = genera_set_casse(med);
                ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = NULL;

                D(printf("Configurazione di cassa selettiva-media aggiunta\n"));
            }

            if(pes > 0){
                (((struct config_cassa_attive *)*current)->next) = (struct config_cassa_attive *) malloc(sizeof(struct config_cassa_attive));
                current = &(((struct config_cassa_attive *)*current)->next);

                ((struct config_cassa_attive *)*current)->next = NULL;
                ((struct config_cassa_attive *)*current)->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
                nuova_configurazione = &((struct config_cassa_attive *)*current)->configurazione_cassa;

                ((struct config_cassa *)*nuova_configurazione)->tipo = selettiva;
                ((struct config_cassa *)*nuova_configurazione)->sotto_tipo = selettiva_pesante;
                ((struct config_cassa *)*nuova_configurazione)->casse = genera_set_casse(med);
                ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = NULL;

                D(printf("Configurazione di cassa selettiva-pesante aggiunta\n"));
            }
            break;

    }

    return 0;

}


//ESPERIMENTO, se non funge, eliminare
//Funzione che permette di introdurre una cassa selettiva con specifiche a grana fina, ovvero (eg.) possibilità di inserire
//3 casse leggere con una fila condivisa e 2 pesanti con file separate ecc....
//qui ci si può sbizzarrire nella fase di sperimentazione
int aggiungi_configurazione_selettiva_custom(int num_casse_leggere, int num_casse_medie, int num_casse_pesanti,
                                                int fila_leggera_condivisa, int fila_media_condivisa, int fila_pesante_condivisa){

    struct config_cassa_attive **current = &config_attive;

    if(*current != NULL) {
        do{
            current = &(((struct config_cassa_attive *)*current)->next);
        }while(*current != NULL);
    }

    *current = (struct config_cassa_attive *)malloc(sizeof(struct config_cassa_attive));

    ((struct config_cassa_attive *)*current)->next = NULL;
    ((struct config_cassa_attive *)*current)->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
    struct config_cassa **nuova_configurazione = &((struct config_cassa_attive *)*current)->configurazione_cassa;

    if(num_casse_leggere != 0){

        ((struct config_cassa *)*nuova_configurazione)->tipo = selettiva;
        ((struct config_cassa *)*nuova_configurazione)->sotto_tipo = selettiva_leggera;
        ((struct config_cassa *)*nuova_configurazione)->casse = genera_set_casse(num_casse_leggere);
        if(fila_leggera_condivisa) {
            ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = crea_fila();
        }else {
            ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = NULL;
        }
    }

    if(num_casse_medie != 0){
        if (num_casse_leggere != 0) {
            (((struct config_cassa_attive *)*current)->next) = (struct config_cassa_attive *) malloc(sizeof(struct config_cassa_attive));
            current = &(((struct config_cassa_attive *)*current)->next);

            ((struct config_cassa_attive *)*current)->next = NULL;
            ((struct config_cassa_attive *)*current)->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
            nuova_configurazione = &((struct config_cassa_attive *)*current)->configurazione_cassa;
        }
        ((struct config_cassa *)*nuova_configurazione)->tipo = selettiva;
        ((struct config_cassa *)*nuova_configurazione)->sotto_tipo = selettiva_media;
        ((struct config_cassa *)*nuova_configurazione)->casse = genera_set_casse(num_casse_medie);
        if(fila_media_condivisa) {
            ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = crea_fila();
        }else {
            ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = NULL;
        }
    }

    if(num_casse_pesanti != 0){
        if (num_casse_leggere != 0 || num_casse_medie != 0) {
            (((struct config_cassa_attive *)*current)->next) = (struct config_cassa_attive *) malloc(sizeof(struct config_cassa_attive));
            current = &(((struct config_cassa_attive *)*current)->next);

            ((struct config_cassa_attive *)*current)->next = NULL;
            ((struct config_cassa_attive *)*current)->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
            nuova_configurazione = &((struct config_cassa_attive *)*current)->configurazione_cassa;
        }
        ((struct config_cassa *)*nuova_configurazione)->tipo = selettiva;
        ((struct config_cassa *)*nuova_configurazione)->sotto_tipo = selettiva_pesante;
        ((struct config_cassa *)*nuova_configurazione)->casse = genera_set_casse(num_casse_pesanti);
        if(fila_pesante_condivisa) {
            ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = crea_fila();
        }else {
            ((struct config_cassa *)*nuova_configurazione)->fila_condivisa = NULL;
        }
    }

    return 0;

}



//-------------------------------------------------------
//funzione per la gestione degli eventi
//-------------------------------------------------------
#include <unistd.h>
int aggiungi_evento(int tipo, int ora_evento, struct fila_cassa *fila){

    struct lista_eventi **current = &eventi;

    struct evento *nuovo_evento = (struct evento *)malloc(sizeof(struct evento));
    nuovo_evento->tipo = tipo;
    if(tipo == servito){
        nuovo_evento->fila = fila;
    }
    nuovo_evento->tempo = ora_evento;


    //se la lista di eventi è vuota, mettilo per primo.
    if(eventi == NULL){
        eventi = (struct lista_eventi *)malloc(sizeof(struct lista_eventi));
        eventi->evento = nuovo_evento;
        eventi->next = NULL;
        eventi->prev = NULL;

        return 0;
    }

    //altrimenti cerca dove metterlo.
    struct lista_eventi *new_el = (struct lista_eventi *)malloc(sizeof(struct lista_eventi));
    new_el->next = NULL;
    new_el->prev = NULL;
    new_el->evento = nuovo_evento;
    //----------------------
    //se la lista di eventi è vuota, mettilo per primo.
    if(eventi == NULL) {
        eventi = (struct lista_eventi *)malloc(sizeof(struct lista_eventi));
        eventi->evento = nuovo_evento;
        eventi->next = NULL;
        eventi->prev = NULL;

        return 0;
    } else if(ora_evento <= eventi->evento->tempo) {
        //aggiorna la testa della lista di eventi
        new_el->next = eventi;
        eventi->prev = new_el;
        eventi = new_el;
    } else {
        //inseriscilo dopo la testa
        struct lista_eventi *curr = eventi;
        struct lista_eventi *prev = NULL;
        while (curr != NULL && curr->evento->tempo <= ora_evento) {
            prev = curr;
            curr = curr->next;
        }
        new_el->next = curr;
        new_el->prev = prev;
        prev->next = new_el;
        if (curr != NULL) {
            curr->prev = new_el;
        }
    }

    return 0;
}

//gerera ora di arrivo, usato per generare a monte tutti gli arrivi dei clienti,
//e dunque tutti gli eventi di tipo 'arrivo'. Ciascun evento sarà associato ad un orario
//che verrà usato per la creazione del cliente.
double genera_arrivo(int ora, int giorno_settimana){

    double media_arr = 0;

    switch (giorno_settimana){

        case lun:
            if ( ore_6 <= ora && ora <= ore_7){
                media_arr = arrivi_lun_6_7;
            } else if ( ore_7 < ora && ora <= ore_8){
                media_arr = arrivi_lun_7_8;
            } else if ( ore_8 < ora && ora <= ore_9){
                media_arr = arrivi_lun_8_9;
            } else if ( ore_9 < ora && ora <= ore_10){
                media_arr = arrivi_lun_9_10;
            } else if ( ore_10 < ora && ora <= ore_11){
                media_arr = arrivi_lun_10_11;
            } else if ( ore_11 < ora && ora <= ore_12){
                media_arr = arrivi_lun_11_12;
            } else if ( ore_12 < ora && ora <= ore_13){
                media_arr = arrivi_lun_12_13;
            } else if ( ore_13 < ora && ora <= ore_14){
                media_arr = arrivi_lun_13_14;
            } else if ( ore_14 < ora && ora <= ore_15){
                media_arr = arrivi_lun_14_15;
            } else if ( ore_15 < ora && ora <= ore_16){
                media_arr = arrivi_lun_15_16;
            } else if ( ore_16 < ora && ora <= ore_17){
                media_arr = arrivi_lun_16_17;
            } else if ( ore_17 < ora && ora <= ore_18){
                media_arr = arrivi_lun_17_18;
            } else if ( ore_18 < ora && ora <= ore_19){
                media_arr = arrivi_lun_18_19;
            } else if ( ore_19 < ora && ora <= ore_20){
                media_arr = arrivi_lun_19_20;
            } else if ( ore_20 < ora && ora <= ore_21){
                media_arr = arrivi_lun_20_21;
            } else if ( ore_21 < ora && ora <= ore_22){
                media_arr = arrivi_lun_21_22;
            } else if ( ore_22 < ora && ora <= ore_23){
                media_arr = arrivi_lun_22_23;
            }
            break;

        case mar:
            if ( ore_6 <= ora && ora <= ore_7){
                media_arr = arrivi_mar_6_7;
            } else if ( ore_7 < ora && ora <= ore_8){
                media_arr = arrivi_mar_7_8;
            } else if ( ore_8 < ora && ora <= ore_9){
                media_arr = arrivi_mar_8_9;
            } else if ( ore_9 < ora && ora <= ore_10){
                media_arr = arrivi_mar_9_10;
            } else if ( ore_10 < ora && ora <= ore_11){
                media_arr = arrivi_mar_10_11;
            } else if ( ore_11 < ora && ora <= ore_12){
                media_arr = arrivi_mar_11_12;
            } else if ( ore_12 < ora && ora <= ore_13){
                media_arr = arrivi_mar_12_13;
            } else if ( ore_13 < ora && ora <= ore_14){
                media_arr = arrivi_mar_13_14;
            } else if ( ore_14 < ora && ora <= ore_15){
                media_arr = arrivi_mar_14_15;
            } else if ( ore_15 < ora && ora <= ore_16){
                media_arr = arrivi_mar_15_16;
            } else if ( ore_16 < ora && ora <= ore_17){
                media_arr = arrivi_mar_16_17;
            } else if ( ore_17 < ora && ora <= ore_18){
                media_arr = arrivi_mar_17_18;
            } else if ( ore_18 < ora && ora <= ore_19){
                media_arr = arrivi_mar_18_19;
            } else if ( ore_19 < ora && ora <= ore_20){
                media_arr = arrivi_mar_19_20;
            } else if ( ore_20 < ora && ora <= ore_21){
                media_arr = arrivi_mar_20_21;
            } else if ( ore_21 < ora && ora <= ore_22){
                media_arr = arrivi_mar_21_22;
            } else if ( ore_22 < ora && ora <= ore_23){
                media_arr = arrivi_mar_22_23;
            }
            break;

        case mer:
            if ( ore_6 <= ora && ora <= ore_7){
                media_arr = arrivi_mer_6_7;
            } else if ( ore_7 < ora && ora <= ore_8){
                media_arr = arrivi_mer_7_8;
            } else if ( ore_8 < ora && ora <= ore_9){
                media_arr = arrivi_mer_8_9;
            } else if ( ore_9 < ora && ora <= ore_10){
                media_arr = arrivi_mer_9_10;
            } else if ( ore_10 < ora && ora <= ore_11){
                media_arr = arrivi_mer_10_11;
            } else if ( ore_11 < ora && ora <= ore_12){
                media_arr = arrivi_mer_11_12;
            } else if ( ore_12 < ora && ora <= ore_13){
                media_arr = arrivi_mer_12_13;
            } else if ( ore_13 < ora && ora <= ore_14){
                media_arr = arrivi_mer_13_14;
            } else if ( ore_14 < ora && ora <= ore_15){
                media_arr = arrivi_mer_14_15;
            } else if ( ore_15 < ora && ora <= ore_16){
                media_arr = arrivi_mer_15_16;
            } else if ( ore_16 < ora && ora <= ore_17){
                media_arr = arrivi_mer_16_17;
            } else if ( ore_17 < ora && ora <= ore_18){
                media_arr = arrivi_mer_17_18;
            } else if ( ore_18 < ora && ora <= ore_19){
                media_arr = arrivi_mer_18_19;
            } else if ( ore_19 < ora && ora <= ore_20){
                media_arr = arrivi_mer_19_20;
            } else if ( ore_20 < ora && ora <= ore_21){
                media_arr = arrivi_mer_20_21;
            } else if ( ore_21 < ora && ora <= ore_22){
                media_arr = arrivi_mer_21_22;
            } else if ( ore_22 < ora && ora <= ore_23){
                media_arr = arrivi_mer_22_23;
            }
            break;

        case gio:
            if ( ore_6 <= ora && ora <= ore_7){
                media_arr = arrivi_gio_6_7;
            } else if ( ore_7 < ora && ora <= ore_8){
                media_arr = arrivi_gio_7_8;
            } else if ( ore_8 < ora && ora <= ore_9){
                media_arr = arrivi_gio_8_9;
            } else if ( ore_9 < ora && ora <= ore_10){
                media_arr = arrivi_gio_9_10;
            } else if ( ore_10 < ora && ora <= ore_11){
                media_arr = arrivi_gio_10_11;
            } else if ( ore_11 < ora && ora <= ore_12){
                media_arr = arrivi_gio_11_12;
            } else if ( ore_12 < ora && ora <= ore_13){
                media_arr = arrivi_gio_12_13;
            } else if ( ore_13 < ora && ora <= ore_14){
                media_arr = arrivi_gio_13_14;
            } else if ( ore_14 < ora && ora <= ore_15){
                media_arr = arrivi_gio_14_15;
            } else if ( ore_15 < ora && ora <= ore_16){
                media_arr = arrivi_gio_15_16;
            } else if ( ore_16 < ora && ora <= ore_17){
                media_arr = arrivi_gio_16_17;
            } else if ( ore_17 < ora && ora <= ore_18){
                media_arr = arrivi_gio_17_18;
            } else if ( ore_18 < ora && ora <= ore_19){
                media_arr = arrivi_gio_18_19;
            } else if ( ore_19 < ora && ora <= ore_20){
                media_arr = arrivi_gio_19_20;
            } else if ( ore_20 < ora && ora <= ore_21){
                media_arr = arrivi_gio_20_21;
            } else if ( ore_21 < ora && ora <= ore_22){
                media_arr = arrivi_gio_21_22;
            } else if ( ore_22 < ora && ora <= ore_23){
                media_arr = arrivi_gio_22_23;
            }
            break;

        case ven:
            if ( ore_6 <= ora && ora <= ore_7){
                media_arr = arrivi_ven_6_7;
            } else if ( ore_7 < ora && ora <= ore_8){
                media_arr = arrivi_ven_7_8;
            } else if ( ore_8 < ora && ora <= ore_9){
                media_arr = arrivi_ven_8_9;
            } else if ( ore_9 < ora && ora <= ore_10){
                media_arr = arrivi_ven_9_10;
            } else if ( ore_10 < ora && ora <= ore_11){
                media_arr = arrivi_ven_10_11;
            } else if ( ore_11 < ora && ora <= ore_12){
                media_arr = arrivi_ven_11_12;
            } else if ( ore_12 < ora && ora <= ore_13){
                media_arr = arrivi_ven_12_13;
            } else if ( ore_13 < ora && ora <= ore_14){
                media_arr = arrivi_ven_13_14;
            } else if ( ore_14 < ora && ora <= ore_15){
                media_arr = arrivi_ven_14_15;
            } else if ( ore_15 < ora && ora <= ore_16){
                media_arr = arrivi_ven_15_16;
            } else if ( ore_16 < ora && ora <= ore_17){
                media_arr = arrivi_ven_16_17;
            } else if ( ore_17 < ora && ora <= ore_18){
                media_arr = arrivi_ven_17_18;
            } else if ( ore_18 < ora && ora <= ore_19){
                media_arr = arrivi_ven_18_19;
            } else if ( ore_19 < ora && ora <= ore_20){
                media_arr = arrivi_ven_19_20;
            } else if ( ore_20 < ora && ora <= ore_21){
                media_arr = arrivi_ven_20_21;
            } else if ( ore_21 < ora && ora <= ore_22){
                media_arr = arrivi_ven_21_22;
            } else if ( ore_22 < ora && ora <= ore_23){
                media_arr = arrivi_ven_22_23;
            }
            break;

        case sab:
            if ( ore_6 <= ora && ora <= ore_7){
                media_arr = arrivi_sab_6_7;
            } else if ( ore_7 < ora && ora <= ore_8){
                media_arr = arrivi_sab_7_8;
            } else if ( ore_8 < ora && ora <= ore_9){
                media_arr = arrivi_sab_8_9;
            } else if ( ore_9 < ora && ora <= ore_10){
                media_arr = arrivi_sab_9_10;
            } else if ( ore_10 < ora && ora <= ore_11){
                media_arr = arrivi_sab_10_11;
            } else if ( ore_11 < ora && ora <= ore_12){
                media_arr = arrivi_sab_11_12;
            } else if ( ore_12 < ora && ora <= ore_13){
                media_arr = arrivi_sab_12_13;
            } else if ( ore_13 < ora && ora <= ore_14){
                media_arr = arrivi_sab_13_14;
            } else if ( ore_14 < ora && ora <= ore_15){
                media_arr = arrivi_sab_14_15;
            } else if ( ore_15 < ora && ora <= ore_16){
                media_arr = arrivi_sab_15_16;
            } else if ( ore_16 < ora && ora <= ore_17){
                media_arr = arrivi_sab_16_17;
            } else if ( ore_17 < ora && ora <= ore_18){
                media_arr = arrivi_sab_17_18;
            } else if ( ore_18 < ora && ora <= ore_19){
                media_arr = arrivi_sab_18_19;
            } else if ( ore_19 < ora && ora <= ore_20){
                media_arr = arrivi_sab_19_20;
            } else if ( ore_20 < ora && ora <= ore_21){
                media_arr = arrivi_sab_20_21;
            } else if ( ore_21 < ora && ora <= ore_22){
                media_arr = arrivi_sab_21_22;
            } else if ( ore_22 < ora && ora <= ore_23){
                media_arr = arrivi_sab_22_23;
            }
            break;

        case dom:
            if ( ore_6 <= ora && ora <= ore_7){
                media_arr = arrivi_dom_6_7;
            } else if ( ore_7 < ora && ora <= ore_8){
                media_arr = arrivi_dom_7_8;
            } else if ( ore_8 < ora && ora <= ore_9){
                media_arr = arrivi_dom_8_9;
            } else if ( ore_9 < ora && ora <= ore_10){
                media_arr = arrivi_dom_9_10;
            } else if ( ore_10 < ora && ora <= ore_11){
                media_arr = arrivi_dom_10_11;
            } else if ( ore_11 < ora && ora <= ore_12){
                media_arr = arrivi_dom_11_12;
            } else if ( ore_12 < ora && ora <= ore_13){
                media_arr = arrivi_dom_12_13;
            } else if ( ore_13 < ora && ora <= ore_14){
                media_arr = arrivi_dom_13_14;
            } else if ( ore_14 < ora && ora <= ore_15){
                media_arr = arrivi_dom_14_15;
            } else if ( ore_15 < ora && ora <= ore_16){
                media_arr = arrivi_dom_15_16;
            } else if ( ore_16 < ora && ora <= ore_17){
                media_arr = arrivi_dom_16_17;
            } else if ( ore_17 < ora && ora <= ore_18){
                media_arr = arrivi_dom_17_18;
            } else if ( ore_18 < ora && ora <= ore_19){
                media_arr = arrivi_dom_18_19;
            } else if ( ore_19 < ora && ora <= ore_20){
                media_arr = arrivi_dom_19_20;
            } else if ( ore_20 < ora && ora <= ore_21){
                media_arr = arrivi_dom_20_21;
            } else if ( ore_21 < ora && ora <= ore_22){
                media_arr = arrivi_dom_21_22;
            } else if ( ore_22 < ora && ora <= ore_23){
                media_arr = arrivi_dom_22_23;
            }
            break;

    }

    if(validazione){
        media_arr = lambda_valid;
    }

    if(super_supermarket){
        media_arr = media_arr*super_factor;
    }

    return Exponential((double)1/((double)media_arr/60/60), 0);
}

void genera_evento_servito(struct cliente *c){

    int n = c->num_oggetti;

    double tempo_di_servizio = A*n+B;

    //tempo di servizio esponenziale in caso di test manuale
    if(validazione){
        tempo_di_servizio = Exponential((double)1/((double)mu_valid/60/60), 0);
        aggiungi_evento(servito, (int)tempo_di_servizio + c->iniziato_a_servire , *(c->fila_scelta));
        return;
    }

    //passo l'approssimazione intera in secondi del tempo di servizio
    aggiungi_evento(servito, (int)tempo_di_servizio + c->iniziato_a_servire , *(c->fila_scelta));
}



//-------------------------------------------------------
//funzione per la stampa degli eventi e configurazioni di cassa attive
//-------------------------------------------------------


char *tipo_config(int tipo){

    switch(tipo){
        case selettiva:
            return selettiva_str;
        case pseudo_casuale:
            return pseudo_casuale_str;
        case condivisa:
            return condivisa_str;
        case mista:
            return mista_str;
        case selettiva_leggera:
            return selettiva_leggera_str;
        case selettiva_media:
            return selettiva_media_str;
        case selettiva_pesante:
            return selettiva_pesante_str;
        default:
            return "Non Registrata";
    }

}

char *giorno_str(int gg){

    switch(gg){
        case lun:
            return "lunedì";
        case mar:
            return "martedì";
        case mer:
            return "mercoledì";
        case gio:
            return "giovedì";
        case ven:
            return "venerdì";
        case sab:
            return "sabato";
        case dom:
            return "domenica";
        default:
            return "giorno non definito";
    }

}

void info_su_configurazioni_attive(){

    int i = 1;
    for(struct config_cassa_attive *current = config_attive; current != NULL; current = current->next){
        printf("Configurazioni attive:\n");

        char *tipo_str = tipo_config(current->configurazione_cassa->tipo);

        printf("%d]-%s\n", i, tipo_str);

        if(strcmp(tipo_str, selettiva_str) == 0){
            printf("Sottotipo: %s\n", tipo_config(current->configurazione_cassa->sotto_tipo));
        }

        printf("Fila condivisa: ");
        if(current->configurazione_cassa->fila_condivisa == NULL) {
            printf("NO\n");
        }else{
            printf("SI\n");

            int j = 0;
            struct fila_cassa *fc = current->configurazione_cassa->fila_condivisa;
            do{
                if(j == 0){
                    printf("Clienti in fila condivisa '%d': ", fc->id);
                }

                if(lunghezza_fila(fc) == 0){
                    printf("x");
                    break;
                }

                printf("%d, ", fc->cliente_in_fila->id);
                j++;

                fc = fc ->next;

            }while(fc != NULL);
            printf("\n");
        }

        int nc = numero_casse(current->configurazione_cassa->casse);
        printf("Numero di casse: %d\n", nc);

        //------------------
        int j = 1;

        for(struct casse *css = current->configurazione_cassa->casse; css != NULL; css = css->next){

            printf("Clienti in fila alla cassa '%d': ", j);
            struct  fila_cassa *temp = css->fila_cassa;
            do{

                if(lunghezza_fila(temp) == 0){
                    printf("x");
                    break;
                }

                printf("%d (num.ogg:%d), ", temp->cliente_in_fila->id, temp->cliente_in_fila->num_oggetti);

                temp = temp ->next;

            }while(temp != NULL);
            printf("\n");

            ++j;

        }

        printf("\n");

        i++;

    }
}




char* tipo_evento_str(int tipo){

    switch (tipo){

        case arrivo:
            return arrivo_str;
        case servito:
            return servito_str;
        default:
            return "Non gestito";
    }

}

void stampa_tutti_eventi(){

    int i = 0;
    for(struct lista_eventi *le = eventi; le != NULL; le = le->next){

        printf("[%d] - %s - %s\n", i, tipo_evento_str(le->evento->tipo), secondi_ora(le->evento->tempo));
        i++;
    }

}

void stampa_num_eventi(int num){

    int i = 0;
    for(struct lista_eventi *le = eventi; le != NULL && i < num; le = le->next){

        printf("[%d] - %s - %s\n", i, tipo_evento_str(le->evento->tipo), secondi_ora(le->evento->tempo));
        i++;
    }

}

void free_eventi(){
    struct lista_eventi *le = eventi;
    struct lista_eventi *temp;
    while (le != NULL){
        
        if (le->evento != NULL)
            free(le->evento);

        temp = le->next;
        free(le);

        le = temp;
    }

}


void stampa_evento(struct evento *e){

    printf("[-] - %s - %s\n", tipo_evento_str(e->tipo), secondi_ora(e->tempo));

}

//-------------------------------------------------------
//funzione per generazione di intervalli di confidenza
//-------------------------------------------------------

double* get_chi(int alpha){

    double *critial = (double *)malloc(2*sizeof(double));

    //perchè i valori seguenti sono stati calcolati per una chi quadro con 19
    //gradi di libertà
    if(num_simulazioni != 20){
        return NULL;
    }

    switch (alpha){
        case 1:
            critial[0] = 6.844;
            critial[1] = 38.658;
            break;
        case 2:
            critial[0] = 7.633;
            critial[1] = 36.191;
            break;
        case 5:
            critial[0] = 8.907;
            critial[1] = 32.825;
            break;
        case 10:
            critial[0] = 10.117;
            critial[1] = 30.144;
            break;
        default:
            break;
    }

    return critial;

}


double get_stud(int alpha){

    double critial = -1;

    //perchè i valori seguenti sono stati calcolati per una student con 19
    //gradi di libertà
    if(num_simulazioni != 20){
        return -1;
    }

    switch (alpha){
        case 1:
            critial = 2.861;
            break;
        case 2:
            critial = 2.539;
            break;
        case 5:
            critial = 2.093;
            break;
        case 10:
            critial = 1.729;
            break;
        default:
            break;
    }

    return critial;

}

void free_configurazioni(){


    struct config_cassa_attive *current = config_attive;
    struct casse *casse;
    struct fila_cassa *fila;

    struct config_cassa_attive *temp_attive;
    struct casse *temp_casse;
    struct fila_cassa *temp_fila;
    //info_su_configurazioni_attive();
    char *tipo_str = tipo_config(current->configurazione_cassa->tipo);

    while(current != NULL){
        
        if (current->configurazione_cassa != NULL && strcmp(tipo_config(current->configurazione_cassa->tipo), "Non Registrata") != 0)
            casse = current->configurazione_cassa->casse;
        else 
            casse = NULL;

        while( casse != NULL ){
            
            fila = casse->fila_cassa;

            fflush(stdout);
            while(fila != NULL){
                temp_fila = fila->next;
                free(fila);
                fila = temp_fila;
            }

            fflush(stdout);
            temp_casse = casse->next;
            free(casse);
            casse = temp_casse;
        }

        fflush(stdout);
        if (current->configurazione_cassa != NULL && strcmp(tipo_config(current->configurazione_cassa->tipo), "Non Registrata") != 0 && current->configurazione_cassa->fila_condivisa != NULL) {
            fila = current->configurazione_cassa->fila_condivisa;
            while(fila != NULL){
                temp_fila = fila->next;
                free(fila);
                fila = temp_fila;
            }
        }

        if (strcmp(tipo_config(current->configurazione_cassa->tipo), "Non Registrata") != 0)    
            free(current->configurazione_cassa);

        temp_attive = current->next;
        free(current);
        current = temp_attive;
    }

}














