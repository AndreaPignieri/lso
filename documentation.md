# Dialogare con i Robot – Furhat Embodied AI

Corso: Laboratorio di Sistemi Operativi
Corso di Laurea: Informatica
Progetto: 4 – Dialogare con i robot

## 1. Introduzione

Il presente progetto realizza un sistema client-server che consente a un robot sociale (Furhat) di dialogare con un utente umano adattando il proprio comportamento in base alla personalità dell’interlocutore.

Il sistema è stato sviluppato in conformità alla traccia del corso di Laboratorio di Sistemi Operativi, che richiede:

- un server implementato in linguaggio C
- un client sviluppato in un linguaggio a scelta
- comunicazione tramite socket
- gestione della concorrenza
- dimostrazione pratica del funzionamento

Il progetto utilizza il questionario TIPI (Ten Item Personality Inventory) per stimare i tratti di personalità secondo il modello Big Five e adatta dialogo, tono ed espressività del robot di conseguenza.

## 2. Architettura del Sistema

L’architettura è composta da tre componenti principali:

- Server in C
- Client in Python
- Robot Furhat (Emulatore)

### 2.1 Ruoli dei componenti

#### Server

Responsabile della logica applicativa:

- ricezione dei dati del questionario
- calcolo dei tratti di personalità
- decisione del comportamento del robot

#### Client

Funziona da intermediario:

- gestisce il dialogo con l’utente
- comunica con il server
- controlla il robot Furhat

#### Furhat

Furhat è utilizzato tramite il suo **emulatore**, che rappresenta l’interfaccia
fisica e multimodale del sistema.

Il robot si occupa di:

- pronunciare le frasi generate dinamicamente dal client tramite un LLM
- acquisire l’input vocale dell’utente tramite riconoscimento vocale
- eseguire gesti, movimenti del capo e direzione dello sguardo in base
  alle istruzioni ricevute dal client

Le risposte dell’utente vengono rilevate dal robot e inoltrate al client,
che provvede alla loro elaborazione e alla generazione delle successive
azioni e dialoghi.

## 3. Scelte Tecnologiche

### 3.1 Linguaggio C per il Server

Il linguaggio C è stato scelto per il server in quanto:

- richiesto esplicitamente dalla traccia
- consente un controllo diretto su:
  - socket
  - memoria
  - thread

### 3.2 Python per il Client

Python è stato utilizzato per il client perché:

- semplifica l’uso delle WebSocket
- permette una facile integrazione con API esterne
- riduce la complessità nella gestione del dialogo

### 3.3 Comunicazione tramite Socket TCP

La comunicazione client-server avviene tramite socket TCP, garantendo:

- affidabilità
- consegna ordinata dei messaggi
- semplicità di implementazione

### 3.4 Uso di JSON come protocollo

La comunicazione tra **client e server** e tra **client e LLM**
avviene utilizzando messaggi in formato **JSON**.

Questa scelta è motivata dalle seguenti caratteristiche:

- formato auto-descrittivo
- facilmente estendibile
- leggibile e facilmente debuggabile
- supportato nativamente dal linguaggio Python
- coerente con le Realtime API di Furhat, basate su messaggi JSON

Nel progetto, il client utilizza un **LLM secondario** per la generazione dinamica
del contenuto del dialogo.  
Le risposte prodotte dal modello includono non solo il **testo da pronunciare**,
ma anche **istruzioni strutturate** relative al comportamento del robot, come:

- gesti da eseguire
- direzione dello sguardo
- stile comunicativo ed espressività

Il formato JSON consente di rappresentare queste informazioni in modo strutturato,
permettendo al client di interpretare l’output del modello linguistico
e tradurlo in comandi espliciti verso il robot Furhat.

Questa soluzione permette di integrare modelli di linguaggio all’interno del sistema
mantenendo una separazione chiara tra la logica applicativa, la generazione del dialogo
e il controllo del comportamento del robot.

#### 3.4.1 Struttura dei JSON

Nella comunicazione con i modelli di linguaggio (LLM), i messaggi JSON
seguono una struttura ben definita.

Un esempio di output JSON è il seguente:

```JSON
{
    "text": "Ciao! È un piacere conoscerti.",
    "gesture": "BigSmile",
    "end_conversation": false
}
```

Dove:

- text: rappresenta il testo che il robot Furhat deve pronunciare
- gesture: indica la gesture o l’espressione facciale da eseguire
- end_conversation: valore booleano che indica se l’utente ha richiesto
la chiusura della conversazione

Questa struttura consente al client di interpretare in modo univoco
le decisioni prodotte dall’LLM e di tradurle in comandi espliciti verso
il robot Furhat, mantenendo il dialogo coerente sia dal punto di vista
verbale che non verbale.

## 4. Server

### 4.1 Struttura generale

Il server segue il classico modello TCP:

- Creazione del socket
- Binding su una porta
- Messa in ascolto (listen)
- Accettazione connessioni (accept)
- Gestione concorrente dei client

### 4.2 Concorrenza e Thread

Il server è multi-threaded:

- ogni connessione client è gestita da un thread dedicato
- i thread sono creati tramite pthread_create
- viene utilizzato pthread_detach per evitare thread zombie

Questa scelta permette:

- gestione simultanea di più client
- isolamento delle connessioni
- continuità del servizio

### 4.3 Gestione delle connessioni

Il server utilizza:

- SO_REUSEADDR per consentire il riavvio rapido
- un ciclo infinito di accept per rimanere sempre disponibile

La terminazione del server avviene manualmente tramite CTRL+C, come previsto per un processo server.

### 4.4 Gestione dei JSON

Per la gestione dei messaggi in formato JSON, il server sviluppato in linguaggio C
utilizza la libreria **cJSON**.

La scelta di questa libreria è motivata dal fatto che:

- è leggera (lightweight) e adatta a sistemi a basso overhead
- è distribuita come singolo file `.c` e `.h`, facilitando l’integrazione nel progetto
- supporta tutte le operazioni necessarie per il parsing e la costruzione di strutture JSON

## 5. Gestione della Personalità

### 5.1 Questionario TIPI

Il questionario TIPI valuta cinque dimensioni:

- Estroversione
- Amicalità
- Coscienziosità
- Stabilità emotiva
- Apertura all’esperienza

Il client raccoglie le risposte e le invia al server.

### 5.2 Calcolo dei tratti

Il server:

- riceve i valori numerici
- calcola i punteggi medi
- costruisce un profilo di personalità

### 5.3 Decisione del comportamento

In base al profilo, il server decide:

- stile comunicativo
- livello di espressività
- tipo di atteggiamento del robot

Il server non genera direttamente il testo, ma fornisce indicazioni strategiche al client.

## 6. Client

### 6.1 Ruolo del Client

Il client:

- guida il dialogo con l’utente
- somministra il test TIPI tramite il robot
- invia i dati al server
- applica il comportamento deciso

### 6.2 Comunicazione con Furhat

Il client utilizza le Realtime API di Furhat:

- connessione WebSocket
- invio di messaggi JSON
- comandi speak, gesture

### 6.3 Integrazione con LLM

Il progetto include l’integrazione con Google Gemini:

- il server decide il comportamento
- il client costruisce un prompt coerente
- il modello genera il testo da pronunciare

Il modello non ha accesso diretto al robot, ma produce un output strutturato interpretato dal client.

## 7. Protocollo di Comunicazione

La comunicazione client-server avviene tramite messaggi JSON.

### 7.1 Esempio di messaggio client → server

```JSON
{
  "type": "personality_data",
  "traits": {
    "extraversion": 4.5,
    "agreeableness": 5.2,
    "conscientiousness": 6.1,
    "emotional_stability": 3.9,
    "openness": 6.8
  }
}
```

### 7.2 Esempio di risposta server → client

```JSON
{
  "behavior": "introvert_calm",
  "dialog_strategy": "soft_smalltalk",
  "prompt_hint": "Usare un tono rassicurante e non invadente."
}
```

## 8. Gestione del Ciclo di Vita

- Il client termina automaticamente quando l’utente decide di interrompere il dialogo.
- Il server rimane attivo per accettare nuove connessioni.
- L’arresto del server avviene manualmente tramite segnale di terminazione.

## 9. Conclusioni

Il progetto ha permesso di applicare concretamente i concetti di:

- programmazione di rete
- gestione dei thread
- progettazione di protocolli
- integrazione tra sistemi eterogenei

## 10. Autori

Progetto realizzato per il corso di Laboratorio di Sistemi Operativi

**Andrea Pignieri** – **N86004636** - <a.pignieri@studenti.unina.it>
