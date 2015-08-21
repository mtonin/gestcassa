CREATE DOMAIN BOOLEAN
AS SMALLINT
CHECK (value is null or value in (0,1));

-- BEGIN TABLE articoli
CREATE TABLE articoli
(
   idarticolo    INTEGER PRIMARY KEY,
   descrizione   VARCHAR(1000),
   prezzo        DECIMAL(5,2),
   destinazione  VARCHAR(100),
   gestioneMenu  BOOLEAN   DEFAULT 0 NOT NULL
);

-- END TABLE articoli

-- BEGIN TABLE articolimenu
CREATE TABLE articolimenu
(
   idarticolo      INTEGER,
   idarticolomenu  INTEGER
);

-- END TABLE articolimenu

-- BEGIN TABLE configurazione
CREATE TABLE configurazione
(
   chiave  VARCHAR(100) primary key,
   valore  VARCHAR(1000)
);

-- END TABLE configurazione

-- BEGIN TABLE destinazionistampa
CREATE TABLE destinazionistampa
(
   nome                    varchar(100) primary key,
   intestazione            VARCHAR(1000),
   stampaflag              BOOLEAN   DEFAULT 1 NOT NULL,
   stampanumeroritiroflag  BOOLEAN   DEFAULT 0 NOT NULL
);

-- END TABLE destinazionistampa

-- BEGIN TABLE ordini
CREATE TABLE ordini
(
   idcassa       VARCHAR(38),
   numero    INTEGER,
   tsstampa  TIMESTAMP,
   importo   DECIMAL(5,2)       DEFAULT 0,
   PRIMARY KEY ( idcassa, numero ) 
);

-- END TABLE ordini

-- BEGIN TABLE ordinirighe
CREATE TABLE ordinirighe
(
   idcassa       VARCHAR(38),
   numeroordine  INTEGER,
   idarticolo    INTEGER,
   quantita      INTEGER
);
-- END TABLE ordinirighe

-- BEGIN TABLE pulsanti
CREATE TABLE pulsanti
(
   idreparto   INTEGER,
   riga        INTEGER,
   colonna     INTEGER,
   idarticolo  INTEGER,
   abilitato   BOOLEAN,
   PRIMARY KEY ( idreparto, riga, colonna ) 
);

-- END TABLE pulsanti

-- BEGIN TABLE reparti
CREATE TABLE reparti
(
   idreparto        INTEGER primary key,
   descrizione      VARCHAR(1000)    NOT NULL,
   font             VARCHAR(1000),
   coloresfondo     VARCHAR(1000),
   coloreCarattere  VARCHAR(1000),
   abilitato        BOOLEAN          DEFAULT 1 NOT NULL
);

-- END TABLE reparti

-- BEGIN TABLE sessione
CREATE TABLE sessione
(
   idsessione  INTEGER primary key,
   tsinizio    TIMESTAMP
);
-- END TABLE sessione

-- BEGIN TABLE storicoordinidett
CREATE TABLE storicoordinidett
(
   idsessione    INTEGER,
   idcassa       VARCHAR(38),
   numeroordine  INTEGER,
   descrizione   VARCHAR(1000),
   quantita      INTEGER,
   destinazione  VARCHAR(100),
   prezzo        DECIMAL(5,2),
   tipoArticolo  CHAR      NOT NULL
);
-- END TABLE storicoordinidett

-- BEGIN TABLE storicoordinitot
CREATE TABLE storicoordinitot
(
   idsessione    INTEGER,
   idcassa       VARCHAR(38),
   nomecassa     VARCHAR(10),
   numeroordine  INTEGER,
   tsstampa      TIMESTAMP,
   importo       DECIMAL(5,2),
   flagstorno    BOOLEAN    DEFAULT 0 NOT NULL,
   primary key (idsessione, idcassa, numeroordine)
);
-- END TABLE storicoordinitot

-- BEGIN TABLE postazioni
CREATE TABLE postazioni
(
   ID         VARCHAR(38)   NOT NULL primary key,
   NOME       VARCHAR(10)   default '' NOT NULL,
   IPADDRESS  VARCHAR(15)   NOT NULL
);
-- END TABLE postazioni

-- BEGIN FOREIGN KEYS --
ALTER TABLE articoli
  ADD CONSTRAINT ARTICOLIFK FOREIGN KEY (destinazione)
  REFERENCES destinazionistampa (nome)
  ON UPDATE NO ACTION
  ON DELETE NO ACTION
;

ALTER TABLE articolimenu
  ADD CONSTRAINT articolimenuFK1 FOREIGN KEY (idarticolomenu)
  REFERENCES articoli (idarticolo)
  ON UPDATE NO ACTION
  ON DELETE NO ACTION
;
ALTER TABLE articolimenu
  ADD CONSTRAINT articolimenuFK2 FOREIGN KEY (idarticolo)
  REFERENCES articoli (idarticolo)
  ON UPDATE NO ACTION
  ON DELETE NO ACTION
;

ALTER TABLE ordinirighe
  ADD CONSTRAINT ordinirighefk FOREIGN KEY (idcassa,numeroordine)
  REFERENCES ordini (idcassa,numero)
  ON UPDATE NO ACTION
  ON DELETE NO ACTION
;

ALTER TABLE pulsanti
  ADD CONSTRAINT pulsantifk FOREIGN KEY (idarticolo)
  REFERENCES articoli (idarticolo)
  ON UPDATE NO ACTION
  ON DELETE NO ACTION
;
-- END FOREIGN KEYS --

-- BEGIN VIEW dettagliordine
CREATE VIEW dettagliordine AS
       SELECT ordinirighe.idcassa,
              ordinirighe.numeroordine,
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
       SELECT ordinirighe.idcassa,
              ordinirighe.numeroordine,
              ordinirighe.quantita,
              articoli.descrizione,
              articoli.destinazione,
              articoli.prezzo,
              'S'
         FROM ordinirighe, 
              articoli
        WHERE ordinirighe.idarticolo = articoli.idarticolo 
              AND
              articoli.gestioneMenu = 0
       UNION ALL
       SELECT ordinirighe.idcassa,
              ordinirighe.numeroordine,
              ordinirighe.quantita,
              articoli.descrizione,
              articoli.destinazione,
              articoli.prezzo,
              'M'
         FROM ordinirighe, 
              articoli
        WHERE ordinirighe.idarticolo = articoli.idarticolo 
              AND
              articoli.gestioneMenu = 1;
-- END VIEW dettagliordine

-- BEGIN VIEW storicoordini
CREATE VIEW storicoordini AS
          SELECT tot.idsessione, 
                 tot.idcassa, 
                 tot.numeroordine, 
                 tot.tsstampa, 
                 tot.importo, 
                 dett.descrizione, 
                 dett.prezzo, 
                 dett.quantita, 
                 dett.destinazione, 
                 dett.tipoArticolo                     
          FROM storicoordinitot tot, 
               storicoordinidett dett                     
          WHERE tot.idsessione = dett.idsessione       
            AND tot.idcassa = dett.idcassa
            AND tot.numeroordine = dett.numeroordine
            AND tot.flagstorno = 0;
-- END VIEW storicoordini

insert into configurazione (chiave,valore) values ('versione',8);