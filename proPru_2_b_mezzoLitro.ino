/*file originale per l'impianto di irrigazione
*/
//variabili


  int debug = true;
  int relayPin_irrigazione = 9; // pin relativo al relay dell'irrigazione
  int relayPin_estrattore = 8; // pin ventola temp
  int sensore0 = 2;
  int sensore1 = 4;
  int sensore2 = 6;
  int ledPin = 13;

/*Tl è ora impostato per indicare 1/2 litri */
// nuove variabili relative all'
  long  G = 86400000;//
  float Tl = 46666.666666666664; //70/3*1000 70/3 è il quantitativo di secondi per erigare un l
  int l = 1;
  /*l'è quantità di litri da erogarre ogni giorno, sarà aggiornata da una
  funzione che trasforma l'analog input in litri, tale quantità sarà espressa
  relativamente a una sola pianta, il fattore di correzzione è presente nella
  variabile Tl.
  */
  unsigned long ciclo = G/l; //durata del ciclo comprensivo di attesa e erogazione
  unsigned long intervallo = ciclo-Tl; //durata della fase di attesa
  bool statoIrrigazione = false;
  unsigned long intervalloCorrenteIrrigazione = 0; //salva su che intervallo lavora il sistema irrigazione
  unsigned long ultimaAzioneIrrigazione = 0;//variabile che salva il momento dell'ultima azione dell'sistema
  //relativo al funzionamento del led
  unsigned long lastLedActivation = 0;
  bool ledState = false;

//test impianto sensori
bool shutDown = false; //quando true stacca il relay
long shutDownInterval = 60000;
unsigned long shutDownStart = 0;
//per debug
unsigned long cCycles = 0;



void setup(/* arguments */) {
  //inizio comunicazioni con il computer
  Serial.begin(9600);
  Serial.println("inizio programma");
  //pin complementari irrigazione
  pinMode(ledPin,OUTPUT);
  //relay irrigazione
  pinMode(relayPin_irrigazione, OUTPUT);
  digitalWrite(relayPin_irrigazione, HIGH);//CONTROLLARE gli imput sono invertiti?
  //relay insonorizzazione
  pinMode(relayPin_estrattore, OUTPUT);
  digitalWrite(relayPin_estrattore, LOW);
  //sensori insonorizzazione
  pinMode(sensore0, INPUT);
  pinMode(sensore1, INPUT);
  pinMode(sensore2, INPUT);
  //setto in modo verboso il valore iniziale dell'analog

  //primo settaggio dei tempi
  Serial.println(Tl);
}

void loop(/* arguments */) {
  if (debug) {
    delay(500);
    cCycles ++;
    Serial.println(cCycles);
  }

  unsigned long currentMillis = millis();//rinomino millis in currentMillis


  //parte relativa all'insonorizzazione
  if (true) {

    int in0 = digitalRead(sensore0);
    int in1 = digitalRead(sensore1);
    int in2 = digitalRead(sensore2);

    if (debug) {
    Serial.print("in0:");
    Serial.println(in0);
    Serial.print("in1:");
    Serial.println(in1);
    Serial.print("in2:");
    Serial.println(in2);
    }

    if(in0 == HIGH || in1 == HIGH || in2 == HIGH) {//se uno dei sensori è triggherato
      /*l'aggiornamento del momento di attivazione  del sensore viene aggiornato
      ogni volta che lo stesso si attiva, il processo di cambio di stato del relay
      etc avvengono solo alla prima attivazione.
      */
      shutDownStart = millis();//si segna il momento di inizio
      digitalWrite(13,HIGH);//attiva il led onboard
      if (shutDown == false) {
        shutDown = true;//si attiva lo stato di shutdown
        digitalWrite(relayPin_estrattore, HIGH);//cambio stato relay
        Serial.println("Spegnimento estrazione");
      }


    }
    else if (shutDown == true && currentMillis - shutDownStart >= shutDownInterval) {
      shutDown = false;
      digitalWrite(relayPin_estrattore, LOW);
      Serial.println("Attivazione");
    }
    else {
      digitalWrite(13,LOW);//spegne led onboard se non ci sono segnali
    }
  }

    //core della funzione di irrigazione
  if (true) {
    if(currentMillis - ultimaAzioneIrrigazione >= intervalloCorrenteIrrigazione) {
      ultimaAzioneIrrigazione = currentMillis;
      if(statoIrrigazione == false) {
        statoIrrigazione = true;
        digitalWrite(relayPin_irrigazione, LOW);//il relay si attiva
        intervalloCorrenteIrrigazione = round(Tl);
        Serial.println("go");
        Serial.println(statoIrrigazione);
      }
      else if(statoIrrigazione == true) { //statoIrrigazione true
        statoIrrigazione = false;
        digitalWrite(relayPin_irrigazione, HIGH);//il relay si spegne
        intervalloCorrenteIrrigazione = intervallo;
        Serial.println("stop");
      }
    }
  }
}
