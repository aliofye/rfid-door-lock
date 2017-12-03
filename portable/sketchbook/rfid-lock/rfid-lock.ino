/*
 RFID-Operated Door Lock
 */
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>  

#define SERVO_PIN       2
#define RST_PIN         5               // RFID-RC522 RST pin mapping
#define SDA_PIN          10             // RFID-RC522 SDA pin mapping

MFRC522 mfrc522(SDA_PIN, RST_PIN);      // Create RFID-RC522 instance
Servo motor;                            // Create a Servo motor instance
LiquidCrystal lcd(7, 8, 9, 6, 4, 3);    // Create a LCD instance with mappings(RS,E,D4,D5,D6,D7) 

void spi_init();
void lcd_init();
void mfrc522_init();
void servo_init();

void lcd_welcome();
void lcd_access_granted();
void lcd_access_denied();

String read_tag();
bool authenticate(String tag);


void setup() {
  spi_init();
  lcd_init();
  mfrc522_init();
  servo_init();

  lcd_welcome();

  Serial.println(F("Scan card."));
}

void loop() {
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  //  Get the card UID
  String tag = read_tag();
  
  //  Print UID to Serial
  Serial.println("UID tag: " + tag);

  //  Check if tag matches and move the servo motor
  if(authenticate(tag)){
    Serial.println("Authorized access");
    lcd_access_granted();
    motor.write( 90 );
    delay(2000);        //simulated delay, simillar to how you would lock your door after unlocking it and leaving
    motor.write( 0 );
  } else {
    Serial.println("Access denied");
    lcd_access_denied();
  }

  delay(1000);

}

/*
 * Initializes serial communications with the PC
 */
void spi_init(){
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);      // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();          // Init SPI bus
}

/*
 * Initializes the 16 x 2 LCD display
 */
void lcd_init(){
  lcd.begin(16, 2);
}

/*
 *  Initializes the RFID-RC522 module
 */
void mfrc522_init(){
  mfrc522.PCD_Init();                     // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();      // Show details of PCD - MFRC522 Card Reader details
}

/*
 *  Initializes the Servo motor module
 */
void servo_init(){
  motor.attach(SERVO_PIN);
  motor.write(0); //sets initial position of servo motor
}


/*
 *  Greets the user
 */
void lcd_welcome(){  
  lcd.setCursor(0,0);  //indicates what line to print the following word
  lcd.write("    Welcome!");  
  lcd.setCursor(0,1);  
  lcd.write(" Scan Your Card");  
}

/*
 *  Notifies the user they have been granted access
 */
void lcd_access_granted(){
  lcd.clear(); 
  lcd.setCursor(0,0);  
  lcd.write("Ali");  
  lcd.setCursor(0,1);  
  lcd.write("Access Granted");
  delay(1500);
  lcd.clear(); 
  lcd_welcome();
}

/*
 *  Notifies the user they have NOT been granted access
 */
void lcd_access_denied(){
  lcd.clear(); 
  lcd.setCursor(0,0);  
  lcd.write("Lucifer");  
  lcd.setCursor(0,1);  
  lcd.write("Access Denied");
  delay(1500);
  lcd.clear();
  lcd_welcome();  
}

/*
 *  Authenticate user
 *  @param card/tag UID
 *  @return returns true/false if card is authorized or not
 */
bool authenticate(String tag){
  const int N = 2;  //change this to be the number of authorized users
  
  String users[N] = {
                      "C0-F-75-89",
                      "CHANGE THIS TO ADD MORE CARDS"
                    };
  for(int i=0; i<N; i++){
    if(tag == users[i]){
      return true;
    } else {
      return false;
    }
  }
}

/*
 * Read the UID of an RFID card/tag
 * @return returns UID of card or tag
 */
String read_tag(){
  
  // Dump debug info about the card; PICC_HaltA() is automatically called
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  
  String tag = "";
  byte letter;

  for( byte i = 0; i < mfrc522.uid.size; i++ ){
     tag.concat(String(mfrc522.uid.uidByte[i], HEX));
     if( i < mfrc522.uid.size-1 ) tag+="-";
  }

  tag.toUpperCase();

  return tag;
}

