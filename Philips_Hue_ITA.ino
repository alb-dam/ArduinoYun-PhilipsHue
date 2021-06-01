#include <Process.h>

static int sensore_luce = A0; //pin analogico che riceve l'indice di luminosità
static int sensore_movimento = 3; //pin digitale che riceve lo stato del sensore di movimento
static int minuti_attesa = 4; //specifica il numero di minuti dopo i quali la luce si spegnerà automaticamente
static int soglia = 150; //modificare questo valore per definire la soglia dopo la quale verra' accesa la luce

unsigned long attesa; //conterra' i minuti di attesa trasformati in millisecondi
int luminosita = 0; // conterra' l'indice di luminosità percepito
boolean movimento;  //conterra' lo stato del sensore di movimento
boolean stato_luce; // conterra' lo stato della luce, true = 1 = accesa, false = 0 = spenta
boolean ultimo_stato_luce; // conterra' l'ultimo stato della luce

// inserire INDIRIZZOIP e USERNAME Bridge philips Hue e modificare il numero della LAMPADINA che si vuole controllare (esempio .../lights/6/state)
String Spegni = "curl -X PUT -d '{\"on\":false}' http://INDIRIZZOIP/api/USERNAME/lights/LAMPADINA/state";
String Accendi = "curl -X PUT -d '{\"on\":true}' http://INDIRIZZOIP/api/USERNAME/lights/LAMPADINA/state";

void setup() {
  Bridge.begin();  // inizializzo il bridge per inviare i comandi via linux
  Serial.begin(9600); // inizializzo comunicazione seriale per mostrare a video quello che stà accadendo
  //definisco la modalità dei pin
  pinMode(sensore_luce, INPUT);
  pinMode(sensore_movimento, INPUT);
  attesa = ((minuti_attesa * 60UL ) * 1000UL); //trasformo i minuti d'attesa in millisecondi da dare in pasto alla funzione delay
  stato_luce = false;   //di default imposto lo stato della luce = spento
  ultimo_stato_luce = true;  //di default imposto l'ultimo stato della luce = acceso
}

void loop() {
  Process p;

  luminosita = analogRead(sensore_luce);   //leggo l'indice di luminosità dal pin A0 (sensore_luce)
  movimento = digitalRead(sensore_movimento);   //leggo lo stato del sensore di movimento dal pin 3 (sensore_movimento)

  if (soglia < luminosita && movimento == HIGH) { //se l'indice di luminosita' è maggiore alla soglia e c'è movimento
    stato_luce = true;                            //informo il programma che la luce è accesa
  }
  else {                                          //in ogni altro caso
    stato_luce = false;                           //informo il programma che la luce è spenta
  }
  //entro nel loop solo se c'è una variazione di stato
  if (stato_luce != ultimo_stato_luce) {
    //quindi se la condizione è vera accendo la luce
    if (stato_luce == true) {
      Serial.print(" - Accendo luce "); //informo tramite monitor seriale
      p.runShellCommand(Accendi); //accendo la luce inviando il comando all'hub Philips Hue
      delay(attesa);
      //se la condizione è falsa spengo la luce
    } else if (stato_luce == false) {
      Serial.print(" - Spengo luce "); //informo tramite monitor seriale
      p.runShellCommand(Spegni); //spengo la luce inviando il comando all'hub Philips Hue
    }
    delay(100);
  }
  ultimo_stato_luce = stato_luce;
  //invio le info al monitor seriale
  Serial.print("Luminosita' percepita : ");
  Serial.print(luminosita);
  Serial.print(" Movimento rilevato : ");
  Serial.print(movimento);
  Serial.print(" - Stato luce : ");
  Serial.println(stato_luce);
  delay(50);
}
