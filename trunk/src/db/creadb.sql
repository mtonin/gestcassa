
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
    destinazione TEXT    REFERENCES destinazionistampa ( nome ),
    gestioneMenu BOOLEAN NOT NULL
                         DEFAULT ( 'false' ) 
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


-- Table: pulsanti
CREATE TABLE pulsanti ( 
    idreparto  INTEGER,
    riga       INTEGER,
    colonna    INTEGER,
    idarticolo INTEGER REFERENCES articoli ( idarticolo ),
    abilitato  BOOLEAN,
    PRIMARY KEY ( idreparto, riga, colonna ) 
);


-- Table: articolimenu
CREATE TABLE articolimenu ( 
    idarticolo     INT REFERENCES articoli ( idarticolo ),
    idarticolomenu INT REFERENCES articoli ( idarticolo ) 
);


-- View: dettagliordine
--righeordine.numeroordine=142 and
--and articoli.gestioneMenu='true'
--righeordine.numeroordine=142 and
CREATE VIEW dettagliordine AS
       SELECT righeordine.numeroordine,
              righeordine.quantita,
              articoli.descrizione,
              articoli.destinazione,
              articoli.prezzo
         FROM righeordine, 
              articoli, 
              articolimenu
        WHERE righeordine.idarticolo = articolimenu.idarticolo 
              AND
              articolimenu.idarticolomenu = articoli.idarticolo
       UNION ALL
       SELECT righeordine.numeroordine,
              righeordine.quantita,
              articoli.descrizione,
              articoli.destinazione,
              articoli.prezzo
         FROM righeordine, 
              articoli
        WHERE righeordine.idarticolo = articoli.idarticolo 
              AND
              articoli.gestioneMenu = 'false';
;

