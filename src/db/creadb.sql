
-- Table: destinazionistampa
CREATE TABLE destinazionistampa ( 
    nome         TEXT PRIMARY KEY,
    intestazione TEXT 
);


-- Table: articoli
CREATE TABLE articoli ( 
    idarticolo   INTEGER PRIMARY KEY AUTOINCREMENT,
    descrizione  VARCHAR,
    prezzo       REAL,
    idreparto    INTEGER,
    riga         INTEGER,
    colonna      INTEGER,
    abilitato    BOOLEAN,
    destinazione TEXT    REFERENCES destinazionistampa ( nome ),
    gestioneMenu BOOLEAN NOT NULL
                         DEFAULT ( 'false' ),
    UNIQUE ( idreparto, riga, colonna ) 
);


-- Table: reparti
CREATE TABLE reparti ( 
    idreparto    INTEGER         PRIMARY KEY AUTOINCREMENT,
    descrizione  VARCHAR( 50 )   NOT NULL,
    font         VARCHAR( 100 ),
    coloresfondo VARCHAR( 50 ) 
);


-- Table: ordini
CREATE TABLE ordini ( 
    numero   INTEGER  PRIMARY KEY,
    tsstampa DATETIME 
);


-- Table: righeordine
CREATE TABLE righeordine ( 
    numeroordine INTEGER REFERENCES ordini ( numero ),
    idarticolo   INTEGER REFERENCES articoli ( idarticolo ),
    quantita     INTEGER 
);


-- Table: configurazione
CREATE TABLE configurazione ( 
    chiave TEXT PRIMARY KEY,
    valore TEXT 
);


-- Table: articolimenu
CREATE TABLE articolimenu ( 
    idpulsante INT,
    idarticolo INT 
);

