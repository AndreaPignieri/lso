# Dialogare con i Robot – Personality-Based Dialogue System

Sistema **client-server** per il robot Furhat che somministra un questionario di personalità (TIPI – Big Five) e adatta in tempo reale dialogo, voce ed espressioni del robot in base al profilo psicologico dell’utente.

---

## Architettura del sistema

Il progetto è composto da tre elementi principali:

1. **Robot Furhat (Emulatore)**  
   Interagisce vocalmente con l’utente (speech, lip-sync, gaze, gesture).

2. **Client Python**  
   - Fa da intermediario tra Furhat e il Server  
   - Gestisce il dialogo
   - (Opzionale) Usa **Google Gemini** per generare le frasi del robot in base alla personalità

3. **Server in C**  
   - Riceve le risposte al questionario TIPI
   - Calcola i tratti Big Five
   - Decide il comportamento da adottare (stile comunicativo, emozioni, strategia)

Client e Server comunicano tramite **socket TCP**.  
Client e Furhat comunicano tramite **Realtime API (WebSocket)**.

---

## Prerequisiti

Assicurarsi di avere installato:

- **Docker** e **Docker Compose**
- **Python 3.9+**
- **Furhat SDK / Furhat Emulator**
- Una **Google Gemini API Key**

Sistema operativo consigliato: **Linux**

---

## Configurazione ed Avvio

### Step 1: Creare il file `.env`

Nella root del progetto creare un file `.env` con le chiavi API:

```env
GOOGLE_API_KEY=la_tua_chiave_api_qui
FURHAT_API_KEY=la_tua_chiave_api_qui
```

### Step 2: Avviare l’Emulatore Furhat

Prima di avviare il progetto:

- Aprire Furhat Emulator (Virtual Furhat)
- Avviare il robot
- Assicurarsi che il Realtime API sia attivo su: ```ws://localhost:9000```

**Nota:** L’emulatore deve essere avviato prima dei container Docker.

### Step 3: Avviare Server e Client

Dalla root del progetto eseguire:

```bash
docker compose up --build
```

### Step 4: Interazione col Robot

Una volta avviato:

- Il robot Furhat inizierà il dialogo
- Somministrerà vocalmente il test TIPI
- Adatterà il comportamento in base alla personalità rilevata

### Step 5: Arresto del programma

Il sistema gestisce l’arresto in due modi distinti:

#### Client Python

Durante il dialogo, se l’utente comunica di voler terminare la conversazione
(es. “voglio smettere di parlare”), il client:

- chiude la connessione con il server
- chiude la connessione con il robot Furhat
- termina automaticamente la propria esecuzione

#### Server C

Il server è progettato per rimanere attivo e accettare nuove connessioni.
Non viene terminato automaticamente quando il client si chiude.

Per arrestare il server è necessario utilizzare:

```bash
CTRL + C
```

## Autori

Progetto di **Laboratorio di Sistemi Operativi**
Corso di Laurea in **Informatica**, Università **Federico II di Napoli**

- **Andrea Pignieri** - **N86004636** - <a.pignieri@studenti.unina.it>
