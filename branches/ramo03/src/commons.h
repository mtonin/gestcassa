#ifndef COMMONS_H
#define COMMONS_H
#include <QString>


enum modalitaType {
    CASSA,
    GESTIONE,
    TEST
} ;

const int ID_SESSIONE_TEST = 999999;
const QString dbFileName = "GestioneCassa/GCAS.fdb";

const int NUM_REPARTI = 8;
const int NUM_RIGHE_ART = 5;
const int NUM_COLONNE_ART = 6;


#endif // COMMONS_H
