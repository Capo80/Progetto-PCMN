//
// Created by ezio on 20/08/20.
//

#include "funzioni_per_simulazioni.h"
#include "rngs.h"
#include <math.h>

#define validation_poche_casse 0

//inizializza simulazione (eg. setta i semi, tipi di configurazioni di cassa in base ai cassieri....)
// num_simulazione ne definisca il tipo di cassa, num_casse il numero di casse

//ritorna il tipo di configurazione usato, in formato stringa
char *inizializza(int num_simulazione, int num_casse, int seed){

    PlantSeeds(seed);


    switch (num_simulazione) {

        case validation_poche_casse:
            aggiungi_configurazione_cassa(pseudo_casuale, num_casse, 0);
        return "poche_casse";


    }


    //creazione delle configurazioni di cassa
    //Testato OK (singolarmente)
    //aggiungi_configurazione_cassa(condivisa, 3, 0);
    aggiungi_configurazione_selettiva_custom(1, 1, 1, 0, 1, 0);
    //aggiungi_configurazione_cassa(condivisa, 4, 0);
    //aggiungi_configurazione_cassa(pseudo_casuale, 1, 0);
    //aggiungi_configurazione_cassa(pseudo_casuale, 2, 0);
    //aggiungi_configurazione_cassa(mista, 2, 1);
    //aggiungi_configurazione_cassa(mista, 3, 1);
    //aggiungi_configurazione_cassa(mista, 4, 1);

    //Testato (a gruppi di configurazioni)
    //leggera media pesante + 1 condivisa
    //aggiungi_configurazione_cassa(mista, 3, 0);
    //aggiungi_configurazione_cassa(condivisa, 1, 0);

    //leggera media e pesante, di cui la media condivisa
    //aggiungi_configurazione_selettiva_custom(1, 1, 1, 0, 1, 0);

    //leggera media e pesante, di cui la leggera condivisa e con 2 casse
    //aggiungi_configurazione_selettiva_custom(2, 1, 1, 1, 0, 0);

    //sbizzarrirsi...e automatizzare

    //aggiungi_configurazione_cassa(condivisa, 3, 0);

    //aggiungi_configurazione_cassa(pseudo_casuale, 3, 0);

    //aggiungi_configurazione_selettiva_custom(3, 2, 1, 1, 0, 0);

    //aggiungi_configurazione_cassa(pseudo_casuale, 6, 0);

    //aggiungi_configurazione_cassa(condivisa, 6, 0);

    //aggiungi_configurazione_selettiva_custom(1, 3, 1, 0, 1, 0);

    return "undefined";

}


int main() {


    FILE *ff = fopen("simulation_results_ezio.csv", "a");
    if(ff == NULL)
    {
        printf("Impossibile creare/aprire file!");
        exit(1);
    }

    super_supermarket = 1;
    super_factor = 10;

    double alpha = 0.05;

    int num_cassieri = 1;

    giorno_corrente = lun;
    do{

        double medie[num_simulazioni];
        double varianze[num_simulazioni];
        double slowdowns[num_simulazioni];
        int abbandoni_tot[num_simulazioni];
        int arrivi_tot[num_simulazioni];

        char *tipo_config_str;

        int seed = SEED;

        //fare num_ismulazioni per ogni giorno, commentare i risultati in base al giorno
        for(int i = 0; i < num_simulazioni; ++i) {

            eventi = NULL;
            config_attive = NULL;
            clienti_serviti = NULL;
            arrivi_totali = 0;
            abbandoni = 0;
            attesa_media_corrente = 0;
            varianza_tempo_attesa = 0;
            slowdown_medio_corrente = 0;


            tipo_config_str = inizializza(validation_poche_casse, num_cassieri, seed);

            seed++;

            //printf("Inizializzazione %d completa per giorno %d\n", i, giorno_corrente);
            start();

            /*
            printf("Simulazione giorno %d terminata\nMedia attesa registrata: %s\nSlowdown medio: %f\n",
                   giorno_corrente, secondi_ora((int) attesa_media_corrente), slowdown_medio_corrente);
            printf("Deviazione std tempo d'attesa: %s\n", secondi_ora(sqrt(varianza_tempo_attesa)));
            printf("Abbandoni: %d\n", abbandoni);
            printf("Arrivi totali: %d\n", arrivi_totali);
            */

            //appendo dati
            medie[i] = attesa_media_corrente;
            varianze[i] = varianza_tempo_attesa;
            slowdowns[i] = slowdown_medio_corrente;
            arrivi_tot[i] = arrivi_totali;
            abbandoni_tot[i] = abbandoni;

        }

        double ic_att_l = 0;
        double ic_att_r = 0;

        double ic_var_l = 0;
        double ic_var_r = 0;

        double ic_slow_l = 0;
        double ic_slow_r = 0;

        double ic_arr_l = 0;
        double ic_arr_r = 0;

        double ic_abb_l = 0;
        double ic_abb_r = 0;

        //Calcolo medie
        for (int j = 0; j < num_simulazioni; j++) {

            ic_att_l += medie[j];
            ic_var_l += varianze[j];
            ic_slow_l += slowdowns[j];
            ic_arr_l += arrivi_tot[j];
            ic_abb_l += abbandoni_tot[j];

        }

        ic_att_l = ic_att_l / num_simulazioni;
        ic_var_l = ic_var_l / num_simulazioni;
        ic_slow_l = ic_slow_l / num_simulazioni;
        ic_arr_l = ic_arr_l / num_simulazioni;
        ic_abb_l = ic_abb_l / num_simulazioni;

        //Calcolo Deviazione standard
        for (int j = 0; j < num_simulazioni; j++) {
            ic_att_r += pow(medie[j] - ic_att_l, 2);     
            ic_var_r += pow(varianze[j] - ic_var_l, 2);
            ic_slow_r += pow(slowdowns[j] - ic_slow_l, 2);
            ic_arr_r += pow(arrivi_tot[j] - ic_arr_l, 2);
            ic_abb_r += pow(abbandoni_tot[j] - ic_abb_l, 2);
        }

        ic_att_r = sqrt(ic_att_r / num_simulazioni);
        ic_var_r = sqrt(ic_var_r / num_simulazioni);
        ic_slow_r = sqrt(ic_slow_r / num_simulazioni);
        ic_arr_r = sqrt(ic_arr_r / num_simulazioni);
        ic_abb_r = sqrt(ic_abb_r / num_simulazioni);

        fprintf(ff, "%d, %s, %d, %s, \"[%f, %f]\", \"[%f, %f]\", \"[%f, %f]\", \"[%f, %f]\", \"[%f, %f]\", %d, %d, %f\n", validation_poche_casse, tipo_config_str, num_cassieri, giorno_str(giorno_corrente), ic_att_l, ic_att_r, ic_slow_l, ic_slow_r, ic_var_l, ic_var_r, ic_abb_l, ic_abb_r, ic_arr_l, ic_arr_r, massima_lunghezza_fila_tollerata, num_simulazioni, alpha);


        giorno_corrente++;
    } while(giorno_corrente <= dom);

    fclose(ff);
}