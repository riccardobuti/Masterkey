//______________________ DEFINIZIONI LIB ETC..  ______________________

#include <UIPEthernet.h>
#include <SPI.h>
#include <MFRC522.h>

#define RELAY           2 //PIN RELAY
#define SS_PIN          8         // Configurable, see typical pin layout above
#define RST_PIN         9          // Configurable, see typical pin layout above

EthernetServer server = EthernetServer(8000);
EthernetClient client;
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


//______________________ SETUP _____________________________
void setup()
{
  Serial.begin(9600);
  SPI.begin();            // Init SPI bus


  uint8_t mac[6] = {0x02, 0x09, 0x00, 0x04, 0x01, 0x03};
  IPAddress myIP(192, 168, 1, 213); //IP PORTA
  Ethernet.begin(mac, myIP);
  server.begin();

  mfrc522.PCD_Init();    // Init MFRC522


  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
  Serial.println("<<INIZIALIZZATO>>");

}

//_____________________ MAIN PROGRAM _________________________
void loop()
{
  
readData();

if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
}
//SE E' PRESENTE UNA CARD E NE LEGGO IL DATO ARRIVO QUI E INVIO

sendData(dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size));


}




//_____________________ METODI DA CHIAMARE DEFINITI PER FUNZIONI _______________________________
void readData(){
  if (EthernetClient client = server.available())
  {
    String cmd = client.readString();
    cmd.trim();
    Serial.println(cmd);
    Serial.println("<< DATI RICEVUTI ");
    client.stop();
    
    if (cmd == "Ack") 
    {
      Serial.println("ESEGUO OK");
      chiudoR();
    }
    else 
    {
      Serial.println("ESEGUO NULLA");

    }
  }
}

void chiudoR(){
  digitalWrite(RELAY, LOW);
  delay(500);
  digitalWrite(RELAY, HIGH);
}

String dump_byte_array(byte *buffer, byte bufferSize) {
  String card="";
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i], HEX);
    card+= String(buffer[i] < 0x10 ? " 0" : " ") + String(buffer[i], HEX);
  }
  card.toUpperCase();
  card.replace(" ", "");
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  return card;
}

void sendData(String data){
  if (client.connect(IPAddress(192,168,1,6),8000))
        {
          client.print(data);
          Serial.println(" DATI INVIATI >> ");
          client.stop();      
        }       
}
