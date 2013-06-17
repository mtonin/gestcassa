
-- Table: destinazionistampa
CREATE TABLE destinazionistampa ( 
    nome         TEXT    PRIMARY KEY,
    intestazione TEXT,
    stampaflag   BOOLEAN NOT NULL
                         DEFAULT ( 'true' ) 
);


-- Table: ordini
CREATE TABLE ordini ( 
    numero   INTEGER  PRIMARY KEY,
    tsstampa DATETIME,
    importo  REAL     DEFAULT ( 0 ) 
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


-- Table: ordinirighe
CREATE TABLE ordinirighe ( 
    numeroordine INTEGER REFERENCES ordini ( numero ),
    idarticolo   INTEGER REFERENCES articoli ( idarticolo ),
    quantita     INTEGER 
);


-- Table: reparti
CREATE TABLE reparti ( 
    idreparto       INTEGER         PRIMARY KEY AUTOINCREMENT,
    descrizione     VARCHAR( 50 )   NOT NULL,
    font            VARCHAR( 100 ),
    coloresfondo    VARCHAR( 50 ),
    coloreCarattere VARCHAR( 50 ) 
);


-- Table: articolimenu
CREATE TABLE articolimenu ( 
    idarticolo     INT REFERENCES articoli ( idarticolo ),
    idarticolomenu INT REFERENCES articoli ( idarticolo ),
    PRIMARY KEY ( idarticolo, idarticolomenu ) 
);


-- Table: sessione
CREATE TABLE sessione ( 
    idsessione INTEGER,
    tsinizio   DATETIME 
);


-- Table: ordinicontenuto
CREATE TABLE ordinicontenuto ( 
    idsessione   INTEGER,
    numeroordine INTEGER,
    tsstampa     DATETIME,
    importo      REAL,
    descrizione  VARCHAR,
    quantita     INTEGER,
    destinazione VARCHAR,
    prezzo       REAL,
    tipoArticolo CHAR     NOT NULL 
);


-- View: dettagliordine
CREATE VIEW dettagliordine AS
       SELECT ordinirighe.numeroordine,
              ordinirighe.quantita,
              articoli.descrizione,
              articoli.destinazione,
              0 AS prezzo,
              'C' AS tipoArticolo
         FROM ordinirighe, 
              articoli, 
              articolimenu
        WHERE ordinirighe.idarticolo = articolimenu.idarticolo 
              AND
              articolimenu.idarticolomenu = articoli.idarticolo
       UNION ALL
       SELECT ordinirighe.numeroordine,
              ordinirighe.quantita,
              articoli.descrizione,
              articoli.destinazione,
              articoli.prezzo,
              'S'
         FROM ordinirighe, 
              articoli
        WHERE ordinirighe.idarticolo = articoli.idarticolo 
              AND
              articoli.gestioneMenu = 'false'
       UNION ALL
       SELECT ordinirighe.numeroordine,
              ordinirighe.quantita,
              articoli.descrizione,
              articoli.destinazione,
              articoli.prezzo,
              'M'
         FROM ordinirighe, 
              articoli
        WHERE ordinirighe.idarticolo = articoli.idarticolo 
              AND
              articoli.gestioneMenu = 'true';

insert into configurazione (chiave,valore) values ('versione','2');