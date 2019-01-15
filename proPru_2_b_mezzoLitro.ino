/*file originale per l'impianto di irrigazione
*/
//variabili

int relayPin_irrigazione = 9; // pin relativo al relay dell'irrigazione
  int relayPin_estrattore = 8; // pin ventola temp
  int sensore0 = 2;
  int sensore1 = 4;
  int sensore2 = 6;
  int resistenzaLitri = A0;
  int ledPin = 13;

/*Tl è ora impostato per indicare 1/2 litri */
// nuove variabili relative all'
  long  G = 86400000;//
  float Tl = 46666.666666666664; //70/3*1000 70/3 è il quantitativo di secondi per erigare un l
  float l = 1;
  /*l'è quantità di litri da erogarre ogni giorno, sarà aggiornata da una
  funzione che trasforma l'analog input in litri, tale quantità sarà espressa
  relativamente a una sola pianta, il fattore di correzzione è presente nella
  variabile Tl.
  */
  unsigned long ciclo = G/l; //durata del ciclo comprensivo di attesa e erogazione
  unsigned long intervallo = ciclo-Tl; //durata della fase di attesa
  bool statoIrrigazione = false;
  unsigned long ultimaAzioneIrrigazione = 0;//variabile che salva il momento dell'ultima azione dell'sistema
  unsigned long intervalloCorrenteIrrigazione = 0; //salva su che intervallo lavora il sistema irrigazione
  int analogSignal = 0; //salva il valore dell'analogread per confrontarlo con i nuovi
  int analogSignal_stabile =0; //valore dell'analogread stabilizzato
  int minimoLitri = 1;
  //relativo al funzionamento del led
  unsigned long lastLedActivation = 0;
  bool ledState = false;


  //definizione funzione che segna converte in litri
  int toLiter(int analog) {
    return minimoLitri + (analog*5/1023);//converte da analogread a litri e aggiunge una costante
  }

  void aggiornamentoTempi() {
    /* aggiorna gli intervalli relativi al sistema di irrigazione
    */
     ciclo = round(G/l); //durata del ciclo comprensivo di attesa e erogazione
     intervallo = round(ciclo-Tl); //durata della fase di attesa
     Serial.print("ciclo:");
     Serial.println(ciclo);
     Serial.print("intervallo: ");
     Serial.println(intervallo);
  }


//test impianto sensori
bool shutDown = false; //quando true stacca il relay
long shutDownInterval = 60000;
unsigned long shutDownStart = 0;



void setup(/* arguments */) {
  //inizio comunicazioni con il computer
  Serial.begin(9600);
  Serial.println("inizio programma");
  //pin complementari irrigazione
  pinMode(A0, INPUT);//dichiarazione pin reststenza variabile
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
  analogSignal = analogRead(resistenzaLitri);
  analogSignal_stabile = analogSignal;
  //primo settaggio dei tempi
  aggiornamentoTempi();
  Serial.println(Tl);
}

void loop(/* arguments */) {

unsigned long currentMillis = millis();//rinomino millis in currentMillis


//parte relativa all'insonorizzazione
if (true) {
  if(digitalRead(sensore0) == HIGH || digitalRead(sensore1) == HIGH || digitalRead(sensore2) == HIGH) {//se uno dei sensori è triggherato
    if (shutDown == false) {
      shutDown = true;//si attiva lo stato di shutdown
      digitalWrite(relayPin_estrattore, HIGH);//cambio stato relay
      Serial.println("Spegnimento");

    }
    /*l'aggiornamento del momento di attivazione  del sensore viene aggiornato
    ogni volta che lo stesso si attiva, il processo di cambio di stato del relay
    etc avvengono solo alla prima attivazione.
    */
    shutDownStart = millis();//si segna il momento di inizio

    Serial.println("att");
  }
  else if (shutDown == true && currentMillis - shutDownStart >= shutDownInterval) {
    shutDown = false;
    digitalWrite(relayPin_estrattore, LOW);
    Serial.println("Attivazione");
  }

}

  //parte relativa all'analog INPUT
  analogSignal = analogRead(resistenzaLitri);
  if(analogSignal <= analogSignal_stabile-5 || analogSignal >= analogSignal_stabile+5) {
    analogSignal_stabile = analogSignal;
    l = toLiter(analogSignal_stabile);//dunque non deve fare tutti i calcoli ogni volta
    aggiornamentoTempi();
    Serial.println("l = ");
    Serial.print(l);
  }

//controllo del led
  if (true) {
    if (currentMillis - lastLedActivation >= l*1000 && ledState == true){
      ledState = false;
      digitalWrite(ledPin, LOW);
      lastLedActivation = currentMillis;
      //Serial.println("ledOff");
    }
    if (currentMillis - lastLedActivation >= 500 && ledState == false) {
      ledState = true;
      digitalWrite(ledPin, HIGH);
      lastLedActivation = currentMillis;
      //Serial.println("ledOn");
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
