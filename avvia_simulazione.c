//
// Created by ezio on 20/08/20.
//

#include "funzioni_per_simulazioni.h"
#include "rngs.h"
#include <math.h>
#include <string.h>

#define pseudo_casuale_s 0



int minuti_sotto(int minuti){

    return (double)(minuti_sopportati-minuti)/60;
}

// Ricava numero di casse in base alla percentuale
int* get_split(int total, int first, int second, int third) {

    int* perc = malloc(3*sizeof(int));

    perc[0] = (int) total * (first*1.0 / 100);
    perc[1] = (int) total * (second*1.0 / 100);
    perc[2] = (int) total * (third*1.0 / 100);

    int curr = 2;


    while (perc[0]+perc[1]+perc[2] < total) {
        perc[curr]++;
        curr--;
        curr = curr % 3;
    }

    //printf("%d, %d, %d\n", perc[0], perc[1], perc[2]);
    return perc;
}

//inizializza simulazione (eg. setta i semi, tipi di configurazioni di cassa in base ai cassieri....)
// num_simulazione ne definisca il tipo di cassa, num_casse il numero di casse
//ritorna il tipo di configurazione usato, in formato stringa
char *inizializza(int num_simulazione, int num_casse){

    int* nums;

    int legg = 0, med = 0, pes = 0;
    char *str = malloc(200);

    switch (num_simulazione) {


        case pseudo_casuale_s:
            aggiungi_configurazione_cassa(pseudo_casuale, num_casse, 0);
            sprintf(str, "pseudo_casuale");
            return str;

        case sperimentale_60_20_20:
            nums = get_split(num_casse, 60, 20, 20);
            aggiungi_configurazione_selettiva_custom(nums[0], nums[1], nums[2], 1, 1, 1);
            free(nums);
            sprintf(str, "sperimentale_60_20_20");
            return str;

        case sperimentale_20_60_20:
            nums = get_split(num_casse, 60, 20, 20);
            aggiungi_configurazione_selettiva_custom(nums[1], nums[0], nums[2], 1, 1, 1);
            free(nums);
            sprintf(str, "sperimentale_20_60_20");
            return str;

        case sperimentale_20_20_60:
            nums = get_split(num_casse, 60, 20, 20);
            aggiungi_configurazione_selettiva_custom(nums[2], nums[1], nums[0], 1, 1, 1);
            free(nums);
            sprintf(str, "sperimentale_20_20_60");
            return str;

        case sperimentale_leggera_condivisa:
            nums = get_split(num_casse, 60, 20, 20);
            aggiungi_configurazione_selettiva_custom(nums[0], 0, 0, 1, 1, 1);
            aggiungi_configurazione_cassa(condivisa, nums[1]+nums[2], 0);
            free(nums);
            sprintf(str, "sperimentale_leggera_condivisa");
            return str;

        case sperimentale_media_condivisa:
            nums = get_split(num_casse, 60, 20, 20);
            info_su_configurazioni_attive();
            aggiungi_configurazione_selettiva_custom(0, nums[0], 0, 1, 1, 1);
            aggiungi_configurazione_cassa(condivisa, nums[1]+nums[2], 0);
            //info_su_configurazioni_attive();
            free(nums);
            sprintf(str, "sperimentale_media_condivisa");
            return str;

        case sperimentale_pesante_condivisa:
            nums = get_split(num_casse, 60, 20, 20);
            aggiungi_configurazione_selettiva_custom(0, 0, nums[0], 1, 1, 1);
            aggiungi_configurazione_cassa(condivisa, nums[1]+nums[2], 0);
            free(nums);
            sprintf(str, "sperimentale_pesante_condivisa");
            return str;

        case incr_2_10_cond:
            aggiungi_configurazione_cassa(condivisa, num_casse, 0);
            sprintf(str, "Condivisa");
            return str;
        case incr_2_10_pc:
            aggiungi_configurazione_cassa(pseudo_casuale, num_casse, 0);
            sprintf(str, "Pseudo-Casuale");
            return str;
        case incr_3_10_sel:
            aggiungi_configurazione_cassa(selettiva, num_casse, 0);
            sprintf(str, "Selettiva");
            return str;
        case incr_3_10_sel_cond:
            //num_casse parte da 3
            do{
                legg++;
                if(num_casse-legg-med-pes == 0){
                    break;
                }
                med++;
                if(num_casse-legg-med-pes == 0){
                    break;
                }
                pes++;
                if(num_casse-legg-med-pes == 0) {
                    break;
                }
            }while(1);
            aggiungi_configurazione_selettiva_custom(legg, med, pes, 1, 1, 1);
            sprintf(str, "Selettiva con fila condivisa");
            return str;
        case mista_3_x:
            //num_casse sono le condivise oltre le 3 selettive
            aggiungi_configurazione_cassa(selettiva, 3+num_casse, num_casse);
            sprintf(str, "3 selettive più casse a file condivisa");
            return str;
        case mista_1_x:
            aggiungi_configurazione_selettiva_custom(1, 0, 0, 1, 0, 0);
            aggiungi_configurazione_cassa(condivisa,num_casse, 0);
            sprintf(str, "1 selettiva (leggera) più casse a fila condivisa");
            return str;
    }

    //lasciato per test manuale
    aggiungi_configurazione_cassa(condivisa, num_casse, 0);

    sprintf(str, "undefined");
    return str;

}

void simulazioni(int tipo_simulazione, int numero_iniziale_cassieri, int max_num_cassieri){

    super_supermarket = 1;
    super_factor = 10;

    int alpha = 5;

    for(int num_cassieri = numero_iniziale_cassieri; num_cassieri <= max_num_cassieri; num_cassieri++) {

        giorno_corrente = lun;
        do {

            double medie_att[num_simulazioni];
            double slowdowns[num_simulazioni];
            int abbandoni_tot[num_simulazioni];
            int arrivi_tot[num_simulazioni];


            char *tipo_config_str = malloc(200);

            //fare num_ismulazioni per ogni giorno, commentare i risultati in base al giorno
            for (int i = 0; i < num_simulazioni; ++i) {

                if(config_attive != NULL && !(giorno_corrente == lun && i == 0)){
                    free_configurazioni();
                    free_eventi();
                }

                eventi = NULL;
                config_attive = NULL;

                arrivi_totali = 0;
                abbandoni = 0;
                attesa_media_corrente = 0;
                varianza_tempo_attesa = 0;
                slowdown_medio_corrente = 0;

                char* temp = inizializza(tipo_simulazione, num_cassieri);

                strcpy(tipo_config_str, temp);
                free(temp); 
                //printf("Inizializzazione %d completa per giorno %d\n", i, giorno_corrente);
                start();

                //appendo dati
                medie_att[i] = attesa_media_corrente;
                slowdowns[i] = slowdown_medio_corrente;
                arrivi_tot[i] = arrivi_totali;
                abbandoni_tot[i] = abbandoni;

            }

            double ic_att_l;
            double ic_att_r;
            double Xnatt = 0;
            double Snatt = 0;

            double ic_var_l;
            double ic_var_r;

            double ic_slow_l;
            double ic_slow_r;
            double Xnslow = 0;
            double Snslow = 0;

            double ic_arr_l;
            double ic_arr_r;
            double Xnarr = 0;
            double Snarr = 0;

            double ic_abb_l;
            double ic_abb_r;
            double Xnabb = 0;
            double Snabb = 0;

            double t_alpha_mezzi = get_stud(alpha);
            double *chi = get_chi(alpha);


            if(t_alpha_mezzi == -1){
                printf("Alpha value not supported\n");
                return;
            }

            for(int i = 0; i < num_simulazioni; i++){
                Xnatt +=  medie_att[i];
                Xnslow += slowdowns[i];
                Xnarr +=  arrivi_tot[i];
                Xnabb += abbandoni_tot[i];
            }

            Xnatt = (double)Xnatt/num_simulazioni;
            Xnslow = (double)Xnslow/num_simulazioni;
            Xnarr = (double)Xnarr/num_simulazioni;
            Xnabb = (double)Xnabb/num_simulazioni;

            for(int i = 0; i < num_simulazioni; i++){
                Snatt += (medie_att[i] - Xnatt)*(medie_att[i] - Xnatt);
                Snslow += (slowdowns[i]-Xnslow)*(slowdowns[i]-Xnslow);
                Snarr += (arrivi_tot[i]- Xnarr)*(arrivi_tot[i]- Xnarr);
                Snabb += (abbandoni_tot[i]-Xnabb)*(abbandoni_tot[i]-Xnabb);
            }

            //varienze
            Snatt = (double)Snatt/(num_simulazioni -1);
            Snslow = (double)Snslow/(num_simulazioni -1);
            Snarr = (double)Snarr/(num_simulazioni -1);
            Snabb = (double)Snabb/(num_simulazioni -1);

            //deviazioni standard
            Snatt = sqrt(Snatt);
            Snarr = sqrt(Snarr);
            Snslow = sqrt(Snslow);
            Snabb = sqrt(Snabb);

            //varianza media attesa
            ic_var_r = ((num_simulazioni -1)*(Snatt*Snatt))/chi[0];
            ic_var_l = ((num_simulazioni -1)*(Snatt*Snatt))/chi[1];

            //media attesa
            ic_att_l = Xnatt - (double)t_alpha_mezzi*Snatt/sqrt(num_simulazioni);
            ic_att_r = Xnatt + (double)t_alpha_mezzi*Snatt/sqrt(num_simulazioni);

            //media slowdown
            ic_slow_l = Xnslow - (double)t_alpha_mezzi*Snslow/sqrt(num_simulazioni);
            ic_slow_r = Xnslow + (double)t_alpha_mezzi*Snslow/sqrt(num_simulazioni);

            //media arrivi
            ic_arr_l = Xnarr - (double)t_alpha_mezzi*Snarr/sqrt(num_simulazioni);
            ic_arr_r = Xnarr + (double)t_alpha_mezzi*Snarr/sqrt(num_simulazioni);

            //media abbandoni
            ic_abb_l = Xnabb - (double)t_alpha_mezzi*Snabb/sqrt(num_simulazioni);
            ic_abb_r = Xnabb + (double)t_alpha_mezzi*Snabb/sqrt(num_simulazioni);

            FILE *ff = fopen("simulation_results_all.csv","a");
            if(ff == NULL)
            {
                printf("Impossibile creare/aprire file!");
                exit(1);
            }

            //calcolo del costo mensile del supermercato.
            double costo_mensile = num_cassieri*2*guadagno_mensile_cassieri - guadagno_attesa_cliente*minuti_sotto(Xnatt)*4*Xnarr + costo_abbandono_cliente*Xnabb*4;

            char* ic_att_l_so = secondi_ora(ic_att_l);
            char* ic_att_r_so = secondi_ora(ic_att_r);
            char* ic_var_l_so = secondi_ora(ic_var_l);
            char* ic_var_r_so = secondi_ora(ic_var_r);

            fprintf(ff, "%s, %d, %s, \"[%s; %s]\", \"[%f; %f]\", \"[%s; %s]\", \"[%f; %f]\", \"[%f; %f]\", %d, %f\n",
                    tipo_config_str, num_cassieri, giorno_str(giorno_corrente), ic_att_l_so,
                    ic_att_r_so, ic_slow_l, ic_slow_r, ic_var_l_so, ic_var_r_so, ic_abb_l, ic_abb_r, ic_arr_l, ic_arr_r,
                    massima_lunghezza_fila_tollerata, costo_mensile);

            free(ic_att_l_so);
            free(ic_att_r_so);
            free(ic_var_l_so);
            free(ic_var_r_so);
            fclose(ff);
            free(chi);

            /*printf("%s, %d, %s, [%s; %s], [%f; %f], [%s; %s], [%f; %f], [%f; %f], %d, %f\n",
                   tipo_config_str, num_cassieri, giorno_str(giorno_corrente), secondi_ora(ic_att_l),
                   secondi_ora(ic_att_r), ic_slow_l, ic_slow_r, secondi_ora(ic_var_l), secondi_ora(ic_var_r), ic_abb_l, ic_abb_r, ic_arr_l, ic_arr_r,
                   massima_lunghezza_fila_tollerata, costo_mensile);
            */

            printf("Tipo test: %s, Giorno: %s fatto, con %d num_casse\n", tipo_config_str, giorno_str(giorno_corrente), num_cassieri);

            free(tipo_config_str);

            giorno_corrente++;

        } while (giorno_corrente <= dom);
    }

    if(config_attive != NULL){
        free_configurazioni();
        free_eventi();
    }
}


void test_manuale(){

    //imponendo super_supermarket = 1 si aumenta di un fattore
    //pari a super_factor il flusso di arrivo dei clienti
    super_supermarket = 1;
    super_factor = 10;

    //imponendo validazione = 1, si fissano costanti mu_valid e lambda_valid.
    //(usato per validazione)
    validazione = 0;
    mu_valid = 2400;
    lambda_valid = 1600;

    //coda 'infinita'
    massima_lunghezza_fila_tollerata = 1000000;

    //coda non infinita
    massima_lunghezza_fila_tollerata = 10;

    eventi = NULL;
    config_attive = NULL;
    arrivi_totali = 0;
    abbandoni = 0;
    attesa_media_corrente = 0;
    varianza_tempo_attesa = 0;
    slowdown_medio_corrente = 0;

    //inserire qui la configurazione che si vuol testare
    //o decommentare una di queste

    //CONDIVISA
    //aggiungi_configurazione_cassa(condivisa, 1, 0);
    //aggiungi_configurazione_cassa(condivisa, 2, 0);
    //aggiungi_configurazione_cassa(condivisa, 4, 0);

    //PSEUDO-CASUALE
    //aggiungi_configurazione_cassa(pseudo_casuale, 1, 0);
    //aggiungi_configurazione_cassa(pseudo_casuale, 2, 0);
    //aggiungi_configurazione_cassa(pseudo_casuale, 4, 0);

    //SOLO SELETTIVE
    //aggiungi_configurazione_cassa(mista, 3, 0);

    //MISTA e una condivisa
    //con solo leggera
    //aggiungi_configurazione_cassa(mista, 2, 1);
    //media e pesante
    //aggiungi_configurazione_cassa(mista, 3, 1);
    //tutte le selettive
    aggiungi_configurazione_cassa(mista, 4, 1);


    start();

    printf("Simulazione giorno %s terminata\nMedia attesa registrata: %s\nSlowdown medio: %f\n",
           giorno_str(giorno_corrente), secondi_ora((int) attesa_media_corrente), slowdown_medio_corrente);
    printf("Deviazione std tempo d'attesa: %s\n", secondi_ora(sqrt(varianza_tempo_attesa)));
    printf("Abbandoni: %d\n", abbandoni);
    printf("Arrivi totali: %d\n", arrivi_totali);

}

int main(int argc, char **argv) {

    if(argc == 2){
        PlantSeeds(SEED);
        test_manuale();
        return 0;
    }

    if(argc == 4){
        PlantSeeds(SEED);
    }else if(argc == 5){
        //estraggo il seme passato
        long stato = atol(argv[4]);
        PlantSeeds(stato);

        printf("Stato caricato: %ld\n", stato);

    }else{
        printf("Input errati\nUso: ./simu test-manuale\n./simu tipo_sperimentazione min_cass max_cass [seme]\n");
        return -1;
    }

    simulazioni(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));

    //salvo lo stato del generatore
    long stato_gen = 0;

    GetSeed(&stato_gen);
    FILE *fp;

    fp = fopen(filename_stato, "w+");
    fprintf(fp, "%ld", stato_gen);
    fclose(fp);

    return 0;

}