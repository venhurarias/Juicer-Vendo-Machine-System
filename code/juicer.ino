#include <Streaming.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_PWMServoDriver.h>
#include <Chrono.h>
#include <EnableInterrupt.h>
#include <EEPROM.h>
#include <Bounce2.h>


#define STEP_PIN 4
#define DIR_PIN 3
#define EN_PIN 2
#define HOME_PIN 49
#define WITH_CUP_1_PIN 47  //12
#define WITH_CUP_2_PIN 46  //16
#define LDR_PIN A8
// #define JUICER_1_PIN 37
// #define JUICER_2_PIN 36
#define JUICER_1_PIN 8
#define JUICER_2_PIN 9

#define FLOW_SWTICH_1_PIN 42
#define FLOW_SWTICH_2_PIN 43
#define TEMP_1_PIN A11
#define TEMP_2_PIN A12
#define COOLER_PIN 7
#define COIN_SORTER_1_PIN 5
#define COIN_SORTER_2_PIN 6
#define BILL_ACCEPTOR_PIN 35
#define LED_1_PIN 34  //YELLOW
#define LED_2_PIN 33
#define LED_3_PIN 32  //BLUE
#define LED_4_PIN 31  //RED
#define LED_5_PIN 30  //GREEN
#define BILL_DIP_100_PIN 22
#define BILL_DIP_20_PIN 23
#define COIN_SLOT_PIN 25

#define BUZZER_PIN A0
#define STOP_PIN A1

#define BUTTON_4_PIN 38
#define BUTTON_3_PIN 39
#define BUTTON_2_PIN 40
#define BUTTON_1_PIN 41


//interupt
#define COIN_DESPENSER_1_PIN 52  //5pesos
#define COIN_DESPENSER_2_PIN 53  //10pesos
#define FLOWMETER_1_PIN 50
#define FLOWMETER_2_PIN 51
#define BILL_PULSE_PIN A14
#define COIN_PULSE_PIN A15


#define UPPER_SMALL 2
#define LOWER_SMALL 3
#define UPPER_BIG 1
#define LOWER_BIG 0

#define SERVO_MIN 150
#define SERVO_MAX 600

#define SERIES_RESISTOR 10000     // 10KΩ pull-down resistor
#define NOMINAL_RESISTANCE 50000  // 50KΩ at 25°C
#define NOMINAL_TEMPERATURE 25    // 25°C in Celsius
#define BETA_COEFFICIENT 3950

#define MAX_TEMP 20
#define ADDRESS5 10
#define ADDRESS10 20


LiquidCrystal_I2C lcd(0x27, 20, 4);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
byte c1[8] = { B10000, B10100, B01110, B10101, B01110, B10100, B10000, B00000 }; 

long step = 0;
int testMode = 0;
int mode = 0;
int initMode = 0;
int changeMode = 0;
int totalAmount = 0;
int quantity10 = 50;
int quantity5 = 50;
int choose = 0;
long myStep;

volatile unsigned long flowmeter1Count = 0;
volatile unsigned long flowmeter2Count = 0;
volatile unsigned long billPulseCount = 0;
volatile unsigned long coinPulseCount = 0;
volatile unsigned long dispenser1Count = 0;
volatile unsigned long dispenser2Count = 0;

Chrono myChrono;
Chrono adminChrono;
Chrono pulseChrono;
Chrono buttonTimer;
Chrono alarmChrono;

bool onPressed = false;
bool is12Oz = true;
bool isJuice1 = true;
int used10 = 0, used5 = 0;
bool juice1Available = true;
bool juice2Available = true;
bool buttonHeld = false;

uint16_t initialDelay = 300;
uint16_t repeatRate = 100;

Bounce flowSwitchDebouncer1 = Bounce();
Bounce flowSwitchDebouncer2 = Bounce();


// void flowmeter1_ISR() {
//   flowmeter1Count++;
// }

// void flowmeter2_ISR() {
//   flowmeter2Count++;
// }

void billPulse_ISR() {
  pulseChrono.restart();
  billPulseCount++;
}

void coinPulse_ISR() {
  pulseChrono.restart();
  coinPulseCount++;
}

void dispenser1_ISR() {
  dispenser1Count++;
}

void dispenser2_ISR() {
  dispenser2Count++;
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100); //

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT), digitalWrite(EN_PIN, HIGH);
  pinMode(JUICER_1_PIN, OUTPUT);
  pinMode(JUICER_2_PIN, OUTPUT);
  pinMode(HOME_PIN, INPUT_PULLUP);
  pinMode(WITH_CUP_1_PIN, INPUT);
  pinMode(WITH_CUP_2_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(LED_4_PIN, OUTPUT);
  pinMode(LED_5_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);



  pinMode(COOLER_PIN, OUTPUT);
  pinMode(COIN_SORTER_1_PIN, OUTPUT);
  pinMode(COIN_SORTER_2_PIN, OUTPUT);
  pinMode(BILL_ACCEPTOR_PIN, OUTPUT);
  pinMode(BILL_DIP_100_PIN, OUTPUT);
  pinMode(BILL_DIP_20_PIN, OUTPUT);
  pinMode(COIN_SLOT_PIN, OUTPUT);

  //interupt
  pinMode(COIN_DESPENSER_1_PIN, INPUT);
  pinMode(COIN_DESPENSER_2_PIN, INPUT);
  pinMode(FLOWMETER_1_PIN, INPUT_PULLUP);
  pinMode(FLOWMETER_2_PIN, INPUT_PULLUP);
  pinMode(BILL_PULSE_PIN, INPUT_PULLUP);
  pinMode(COIN_PULSE_PIN, INPUT_PULLUP);

  // enableInterrupt(FLOWMETER_1_PIN, flowmeter1_ISR, RISING);
  // enableInterrupt(FLOWMETER_2_PIN, flowmeter2_ISR, RISING);
  enableInterrupt(BILL_PULSE_PIN, billPulse_ISR, FALLING);
  enableInterrupt(COIN_PULSE_PIN, coinPulse_ISR, FALLING);
  enableInterrupt(COIN_DESPENSER_1_PIN, dispenser1_ISR, FALLING);
  enableInterrupt(COIN_DESPENSER_2_PIN, dispenser2_ISR, FALLING);

  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  pinMode(BUTTON_3_PIN, INPUT_PULLUP);
  pinMode(BUTTON_4_PIN, INPUT_PULLUP);
  pinMode(FLOW_SWTICH_1_PIN, INPUT_PULLUP);
  pinMode(FLOW_SWTICH_2_PIN, INPUT_PULLUP);
  pinMode(STOP_PIN, INPUT_PULLUP);
  pinMode(TEMP_1_PIN, INPUT);
  pinMode(TEMP_2_PIN, INPUT);

  allOff();

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0), lcd << F("====================");
  lcd.setCursor(0, 1), lcd << F("       JUICER       ");
  lcd.setCursor(0, 2), lcd << F("       MACHINE      ");
  lcd.setCursor(0, 3), lcd << F("====================");
  lcd.createChar(0, c1);

  pwm.begin();
  pwm.setPWMFreq(60);


  // EEPROM.put(ADDRESS5, 50);
  // EEPROM.put(ADDRESS10, 50);

  // delay(1000);
  EEPROM.get(ADDRESS5, quantity5);
  EEPROM.get(ADDRESS10, quantity10);

  flowSwitchDebouncer1.attach(FLOW_SWTICH_1_PIN);
  flowSwitchDebouncer1.interval(5000);

  flowSwitchDebouncer2.attach(FLOW_SWTICH_2_PIN);
  flowSwitchDebouncer2.interval(5000);
}


//
void loop() {
  // despenseChange();


  // Serial << "Home :: " << isInHome() << "\t\tCup1 :: " << withCup1() << "\t\tCup2 :: " << withCup2() << endl;
  // Serial<<"Button 1 :: "<<isButton1Pressed()<<"\t\tButton 2 :: "<<isButton2Pressed()<<"\t\tButton 3 :: "<<isButton3Pressed()<<"\t\tButton 4 :: "<<isButton4Pressed()<<endl;
  // Serial << "TEMP 1 :: " << getTemp1() << "\t\tTEMP 2 :: " << getTemp2() << endl;
  // getTemp1();
  // moveRetract();
  // testing();
  // Serial<<withCup()<<endl;

  // Serial<<"READING :: "<<analogRead(LDR_PIN)<<endl;


  // Serial<<"EMPTY 1 :: "<<isEmpty1Pressed()<<"\t\tEMPTY 2 :: "<<isEmpty2Pressed()<<endl;
  // Serial<<isStopPressed()<<endl;
  // Serial << "Float Meter 1 :: " << flowmeter1Count << "\t\tFloat Meter 2 :: " << flowmeter2Count << "\t\tBill :: " << billPulseCount << "\t\tCoin :: " << coinPulseCount << "\t\tDispenser 1 :: " << dispenser1Count << "\t\tDispenser 2 :: " << dispenser2Count << endl;
  normalProcess();
}

void normalProcess() {
  if (isStopPressed()) {
    mode = 200;
  }


  if (mode >= 30 && mode < 99) {
    despenseChange();
  }
  if (mode == 0 || mode == 1 || mode == 10) {
    if (isUpPressed() && !isDownPressed() && !isEnterPressed() && isBackPressed()) {
      if (adminChrono.hasPassed(5000)) {
        mode = 99;
      }
    } else {
      adminChrono.restart();
    }
  }
  switch (mode) {

    case 0:
      initMode = 0;
      mode = 1;
      break;

    case 1:
      // mode=10;
      if (initialize()) {
        mode = 10;
      }
      break;
    

    case 10:
      juice1Available = true;
      juice2Available = true;

      if (!withCup1()) {
        is12Oz = false;
        juice1Available = false;
      } else if (!withCup2()) {
        is12Oz = true;
        juice2Available = false;
      }
      if (getTemp1() > MAX_TEMP) {
        juice1Available = false;
      }
      if (getTemp2() > MAX_TEMP) {
        juice2Available = false;
      }

      if ((isEmpty1Pressed() && isEmpty2Pressed()) || (!juice1Available && !juice2Available)) {
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("       SORRY        ");
        lcd.setCursor(0, 2), lcd << F(" NO JUICE AVAILABLE ");
        lcd.setCursor(0, 3), lcd << F("====================");
      } else if (quantity5 < 1) {
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("       SORRY        ");
        lcd.setCursor(0, 2), lcd << F(" NO CHANGE AVAILABLE");
        lcd.setCursor(0, 3), lcd << F("====================");
      } else {
        if (isUpPressed() || isDownPressed()) {
          if (!onPressed) {
            is12Oz = !is12Oz;
            onPressed = true;
          }
        } else {
          onPressed = false;
        }

        lcd.setCursor(0, 0), lcd << F("=======CHOOSE=======");
        if (withCup1())
          lcd.setCursor(1, 1), lcd << F("12oz (Php15.00)    ");
        else
          lcd.setCursor(1, 1), lcd << F("12oz-Not Available ");
        if (withCup2())
          lcd.setCursor(1, 2), lcd << F("16oz (Php20.00)    ");
        else
          lcd.setCursor(1, 2), lcd << F("16oz-Not Available ");
        lcd.setCursor(0, 3), lcd << F("====================");

        if (is12Oz) {
          lcd.setCursor(0, 1), lcd.print(char(0));
          lcd.setCursor(0, 2), lcd.print(" ");
        } else {
          lcd.setCursor(0, 1), lcd.print(" ");
          lcd.setCursor(0, 2), lcd.print(char(0));
        }

        if (isEnterPressed()) {
          mode = 11;
        }
      }
      break;

    case 11:
      if (!isEnterPressed()) {
        mode = 20;
        lcd.clear();
        coinPulseCount = 0;
        totalAmount = 0;
        if (is12Oz) {
          if (totalAvailableChange() >= 85) {
            Accept20On();
            Accept100On();
            delay(200);
            delay(1000);
            billPulseCount = 0;
          } else if (totalAvailableChange() >= 35) {
            Accept20On();
            Accept100Off();
            delay(200);
            delay(1000);
            billPulseCount = 0;
          } else {
            Accept20Off();
            Accept100Off();
          }
        } else {
          if (totalAvailableChange() >= 80) {
            Accept20On();
            Accept100On();
            delay(200);
            delay(1000);
            billPulseCount = 0;

          } else if (totalAvailableChange() >= 30) {
            Accept20On();
            Accept100Off();
            delay(200);
            delay(1000);
            billPulseCount = 0;
          } else {
            Accept20Off();
            Accept100Off();
          }
        }
        billAcceptorOn();
        coinSlotOn();
      }
      break;

    case 20:
      lcd.setCursor(0, 0), lcd << F("====================");
      lcd.setCursor(0, 1), lcd << F("INSERT COIN OR BILL ");
      if (is12Oz) {
        lcd.setCursor(0, 2), lcd << F("Amount: Php 15.00   ");
      } else {
        lcd.setCursor(0, 2), lcd << F("Amount: Php 20.00   ");
      }
      lcd.setCursor(0, 3), lcd << F("Credit: Php ") << totalAmount << F(".00");
      totalAmount = (billPulseCount * 10) + coinPulseCount;

      if (pulseChrono.hasPassed(500)) {
        if (is12Oz) {
          if (totalAmount >= 15) {
            mode = 25;
            billAcceptorOff();
            coinSlotOff();
          }
        } else {
          if (totalAmount >= 20) {
            mode = 25;

            billAcceptorOff();
            coinSlotOff();
          }
        }
      }
      if (isBackPressed()) {
        if (totalAmount == 0) mode = 10;
      }

      break;

    case 21:
      lcd.setCursor(0, 0), lcd << F("You inserted:       ");
      lcd.setCursor(0, 1), lcd << F("Php ") << totalAmount << F(".00  ");
      lcd.setCursor(0, 2), lcd << F("You need Php") << 15 - totalAmount << F(".00 ");
      lcd.setCursor(0, 3), lcd << F("more to buy         ");

      if (isEnterPressed()) {
        mode = 22;
      }
      break;

    case 22:
      if (!isEnterPressed()) {
        mode = 20;
      }
      break;


    case 25:

      if (isUpPressed() || isDownPressed()) {
        if (!onPressed) {
          isJuice1 = !isJuice1;
          onPressed = true;
        }
      } else {
        onPressed = false;
      }

      if (isEmpty1Pressed()) {
        isJuice1 = false;
      } else if (isEmpty2Pressed()) {
        isJuice1 = true;
      }
      lcd.setCursor(0, 0), lcd << F("=======CHOOSE=======");
      if (isEmpty1Pressed()) {
        lcd.setCursor(1, 1), lcd << F("RED ICE TEA - N/A  ");
      } else {
        lcd.setCursor(1, 1), lcd << F("RED ICE TEA        ");
      }
      if (isEmpty2Pressed()) {
        lcd.setCursor(1, 2), lcd << F("CUCUMBER - N/A     ");
      } else {
        lcd.setCursor(1, 2), lcd << F("CUCUMBER           ");
      }

      lcd.setCursor(0, 3), lcd << F("====================");
      if (isJuice1) {
        lcd.setCursor(0, 1), lcd.print(char(0));
        lcd.setCursor(0, 2), lcd.print(" ");
      } else {
        lcd.setCursor(0, 1), lcd.print(" ");
        lcd.setCursor(0, 2), lcd.print(char(0));
      }

      if (isEnterPressed()) {
        mode = 30;
        changeMode = 1;
      }
      break;


    case 30:
      if (withCup()) {
        mode = 31;
        myChrono.restart();
      } else {
        mode = 32;
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("     PLEASE WAIT    ");
        lcd.setCursor(0, 2), lcd << F("                    ");
        lcd.setCursor(0, 3), lcd << F("====================");
      }
      break;

    case 31:

      if (withCup()) {
        myChrono.restart();
        lcd.setCursor(0, 0), lcd << F("========ERROR=======");
        lcd.setCursor(0, 1), lcd << F(" Remove Cup or any  ");
        lcd.setCursor(0, 2), lcd << F("object blocking the ");
        lcd.setCursor(0, 3), lcd << F("      despenser     ");
      }
      if (myChrono.hasPassed(3000)) {
        lcd.setCursor(0, 0), lcd << F("====================");
        lcd.setCursor(0, 1), lcd << F("     PLEASE WAIT    ");
        lcd.setCursor(0, 2), lcd << F("                    ");
        lcd.setCursor(0, 3), lcd << F("====================");
        mode = 32;
      }
      break;

    case 32:
      if (moveRetract()) {
        mode = 40;
      }
      break;


    case 40:
      if (is12Oz) {
        if (gotoPos(2000L)) {
          myChrono.restart();
          mode = 51;
        }
      } else {
        if (gotoPos(19200L)) {
          myChrono.restart();
          mode = 50;
        }
      }
      break;

    case 50:
      lowerBigStopperOpen();
      delay(1000);
      lowerBigStopperClose();
      delay(1000);
      upperBigStopperOpen();
      delay(1000);
      upperBigStopperClose();
      delay(1000);
      mode = 52;
      break;

    case 51:
      lowerSmallStopperOpen();
      delay(1000);
      lowerSmallStopperClose();
      delay(1000);
      upperSmallStopperOpen();
      delay(1000);
      upperSmallStopperClose();
      delay(1000);
      mode = 52;
      break;

    case 52:
      if (withCup()) {
        mode = 60;
      } else {
        mode = 300;
      }

      break;

    case 60:
      if (gotoPos(60500L)) {
        mode = 70;
        myChrono.restart();
        if (isJuice1) {
          juicer1On();
        } else {
          juicer2On();
        }
      }
      break;

    case 70:
      if (is12Oz) {
        if (myChrono.hasPassed(11000)) {
          myChrono.restart();
          juicer1Off();
          juicer2Off();
          mode = 80;
        }
      } else {
        if (myChrono.hasPassed(16000)) {
          myChrono.restart();
          juicer1Off();
          juicer2Off();
          mode = 80;
        }
      }

      break;

    case 80:
      lcd.setCursor(0, 0), lcd << F("====================");
      lcd.setCursor(0, 1), lcd << F("     JUICE READY    ");
      lcd.setCursor(0, 2), lcd << F("      THANK YOU     ");
      lcd.setCursor(0, 3), lcd << F("====================");
      BuzzerOn();
      delay(500);
      BuzzerOff();
      delay(250);
      BuzzerOn();
      delay(500);
      BuzzerOff();
      delay(250);
      mode = 81;
      break;

    case 81:
      if (!withCup()) {
        mode = 10;
      }


      break;

    case 99:
      lcd.setCursor(0, 0), lcd << F("====================");
      lcd.setCursor(0, 1), lcd << F("     ADMIN MODE     ");
      lcd.setCursor(0, 2), lcd << F("                    ");
      lcd.setCursor(0, 3), lcd << F("====================");
      if (!isBackPressed()) {
        mode = 100;
      }
      break;

    case 100:
      lcd.setCursor(0, 0), lcd << F("======CHOOSE========");
      lcd.setCursor(1, 1), lcd << F("Refill Coin        ");
      lcd.setCursor(1, 2), lcd << F("Maintenance        ");
      lcd.setCursor(0, 3), lcd << F("====================");
      if (isUpPressed() || isDownPressed()) {
        if (!onPressed) {
          if (choose == 0) {
            choose = 1;
          } else {
            choose = 0;
          }
          onPressed = true;
        }
      } else {
        onPressed = false;
      }
      if (choose == 0) {
        lcd.setCursor(0, 1), lcd.print(char(0));
        lcd.setCursor(0, 2), lcd.print(" ");
      } else {
        lcd.setCursor(0, 1), lcd.print(" ");
        lcd.setCursor(0, 2), lcd.print(char(0));
      }

      if (isEnterPressed()) {
        if (choose == 0) {
          mode = 101;
        } else {
          lcd.clear();
          mode = 102;
        }
      }

      if (isBackPressed()) {
        mode = 0;
      }

      break;

    case 101:
      if (!isEnterPressed()) {
        mode = 110;
      }
      break;

    case 102:
      if (!isEnterPressed()) {
        mode = 140;
      }
      break;

    case 110:
      lcd.setCursor(0, 0), lcd << F("======CHOOSE========");
      lcd.setCursor(1, 1), lcd << F("5 pesos            ");
      lcd.setCursor(1, 2), lcd << F("10 pesos           ");
      lcd.setCursor(0, 3), lcd << F("====================");
      if (isUpPressed() || isDownPressed()) {
        if (!onPressed) {
          if (choose == 0) {
            choose = 1;
          } else {
            choose = 0;
          }
          onPressed = true;
        }
      } else {
        onPressed = false;
      }
      if (choose == 0) {
        lcd.setCursor(0, 1), lcd.print(char(0));
        lcd.setCursor(0, 2), lcd.print(" ");
      } else {
        lcd.setCursor(0, 1), lcd.print(" ");
        lcd.setCursor(0, 2), lcd.print(char(0));
      }

      if (isEnterPressed()) {
        if (choose == 0) {
          mode = 111;
        } else {
          mode = 112;
        }
      }
      if (isBackPressed()) {
        mode = 113;
      }

      break;

    case 111:
      if (!isEnterPressed()) {
        mode = 120;
      }
      break;

    case 112:
      if (!isEnterPressed()) {
        mode = 130;
      }
      break;

    case 113:
      if (!isBackPressed()) {
        mode = 100;
      }
      break;

    case 120:
      lcd.setCursor(0, 0), lcd << F("======5 PESOS=======");
      lcd.setCursor(0, 1), lcd << quantity5 << F("pcs       ");
      lcd.setCursor(0, 2), lcd << F("                    ");
      lcd.setCursor(0, 3), lcd << F("====================");



      if (isUpPressed()) {
        if (!buttonHeld) {  // First press
          quantity5++;
          buttonHeld = true;
          buttonTimer.restart();
        } else if (buttonTimer.hasPassed(initialDelay)) {  // Start rapid increment
          if (buttonTimer.hasPassed(repeatRate)) {
            quantity5++;
            buttonTimer.restart();
            repeatRate = max(50, repeatRate - 10);  // Speed up
          }
        }
      } else {
        buttonHeld = false;
        repeatRate = 100;  // Reset speed when released
      }

      // Handle "Down" button
      if (isDownPressed()) {
        if (!buttonHeld) {
          quantity5--;
          buttonHeld = true;
          buttonTimer.restart();
        } else if (buttonTimer.hasPassed(initialDelay)) {
          if (buttonTimer.hasPassed(repeatRate)) {
            quantity5--;
            buttonTimer.restart();
            repeatRate = max(50, repeatRate - 10);  // Speed up
          }
        }
      } else {
        buttonHeld = false;
        repeatRate = 100;  // Reset speed when released
      }
      if (isBackPressed()) {
        mode = 121;

        EEPROM.put(ADDRESS5, quantity5);
      }

      delay(50);
      break;

    case 121:
      if (!isBackPressed()) {
        mode = 110;
      }
      break;

    case 130:
      lcd.setCursor(0, 0), lcd << F("======10 PESOS======");
      lcd.setCursor(0, 1), lcd << quantity10 << F("pcs       ");
      lcd.setCursor(0, 2), lcd << F("                    ");
      lcd.setCursor(0, 3), lcd << F("====================");


      if (isUpPressed()) {
        if (!buttonHeld) {  // First press
          quantity10++;
          buttonHeld = true;
          buttonTimer.restart();
        } else if (buttonTimer.hasPassed(initialDelay)) {  // Start rapid increment
          if (buttonTimer.hasPassed(repeatRate)) {
            quantity10++;
            buttonTimer.restart();
            repeatRate = max(50, repeatRate - 10);  // Speed up
          }
        }
      } else {
        buttonHeld = false;
        repeatRate = 100;  // Reset speed when released
      }

      // Handle "Down" button
      if (isDownPressed()) {
        if (!buttonHeld) {
          quantity10--;
          buttonHeld = true;
          buttonTimer.restart();
        } else if (buttonTimer.hasPassed(initialDelay)) {
          if (buttonTimer.hasPassed(repeatRate)) {
            quantity10--;
            buttonTimer.restart();
            repeatRate = max(50, repeatRate - 10);  // Speed up
          }
        }
      } else {
        buttonHeld = false;
        repeatRate = 100;  // Reset speed when released
      }
      if (isBackPressed()) {
        mode = 121;
        EEPROM.put(ADDRESS10, quantity10);
      }
      delay(50);
      break;

    case 140:
      lcd.setCursor(0, 0), lcd << F("======CHOOSE========");
      lcd.setCursor(1, 1), lcd << F("PUMP 1             ");
      lcd.setCursor(1, 2), lcd << F("PUMP 2             ");
      lcd.setCursor(1, 3), lcd << F("                   ");
      if (isDownPressed()) {
        if (!onPressed) {
          choose++;
          if (choose > 2) {
            choose = 0;
          }
          onPressed = true;
        }
      } else if (isUpPressed()) {
        if (!onPressed) {
          --choose;
          if (choose < 0) {
            choose = 1;
          }
        }

      } else {
        onPressed = false;
      }
      if (choose == 0) {
        lcd.setCursor(0, 1), lcd.print(char(0));
        lcd.setCursor(0, 2), lcd.print(" ");
        lcd.setCursor(0, 3), lcd.print(" ");
      } else if (choose == 1) {
        lcd.setCursor(0, 1), lcd.print(" ");
        lcd.setCursor(0, 2), lcd.print(char(0));
        lcd.setCursor(0, 3), lcd.print(" ");
      }

      if (isEnterPressed()) {
        if (choose == 0) {
          mode = 141;
        } else if (choose == 1) {
          mode = 142;
        }
      }

      if (isBackPressed()) {
        mode = 113;
      }
      break;

    case 141:
      if (!isEnterPressed()) {
        mode = 150;
        lcd.setCursor(0, 0), lcd << F("======PUMP 1========");
        lcd.setCursor(0, 1), lcd << F("UP -> ON            ");
        lcd.setCursor(0, 2), lcd << F("DOWN -> OFF         ");
        lcd.setCursor(0, 3), lcd << F("====================");
      }
      break;

    case 142:
      if (!isEnterPressed()) {
        mode = 160;
        lcd.setCursor(0, 0), lcd << F("======PUMP 2========");
        lcd.setCursor(0, 1), lcd << F("UP -> ON            ");
        lcd.setCursor(0, 2), lcd << F("DOWN -> OFF         ");
        lcd.setCursor(0, 3), lcd << F("====================");
      }
      break;

    case 143:
      if (!isEnterPressed()) {
        mode = 170;
        lcd.setCursor(0, 0), lcd << F("======COOLER========");
        lcd.setCursor(0, 1), lcd << F("UP -> ON            ");
        lcd.setCursor(0, 2), lcd << F("DOWN -> OFF         ");
        lcd.setCursor(0, 3), lcd << F("====================");
      }
      break;



    case 150:
      if (isUpPressed()) {
        juicer1On();

      } else if (isDownPressed()) {
        juicer1Off();
      } else if (isBackPressed()) {
        mode = 151;
      }
      break;

    case 151:
      if (!isBackPressed()) {
        mode = 140;
      }
      break;

    case 160:

      if (isUpPressed()) {
        juicer2On();
      } else if (isDownPressed()) {
        juicer2Off();
      }
      if (isBackPressed()) {
        mode = 151;
      }
      break;

    case 170:

      if (isUpPressed()) {
        coolerOn();
      } else if (isDownPressed()) {
        coolerOff();
      }
      if (isBackPressed()) {
        mode = 151;
      }
      break;

    case 200:
      lcd.setCursor(0, 0), lcd << F("====================");
      lcd.setCursor(0, 1), lcd << F("   EMERGENCY STOP   ");
      lcd.setCursor(0, 2), lcd << F("      PRESSED       ");
      lcd.setCursor(0, 3), lcd << F("====================");
      if (!isStopPressed()) {
        mode = 0;
      }
      break;

    case 300:
      lcd.setCursor(0, 0), lcd << F("====================");
      lcd.setCursor(0, 1), lcd << F("   NO CUP DETECTED  ");
      lcd.setCursor(0, 2), lcd << F("  PLEASE CALL STAFF ");
      lcd.setCursor(0, 3), lcd << F("====================");

      break;
  }
  if (alarmChrono.hasPassed(5000)) {
    alarmChrono.restart();
    if (!withCup1() || !withCup2()) {
      led4On();
    } else {
      led4Off();
    }
    if (totalAvailableChange() < 85) {
      led3On();
    } else {
      led3Off();
    }
    if (isEmpty1Pressed()) {
      led1On();
    } else {
      led1Off();
    }
    if (isEmpty2Pressed()) {
      led5On();
    } else {
      led5Off();
    }
  }
}


double Thermistor(int RawADC) {
  double Temp;

  Temp = log(10240000 / RawADC - 10000);
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // This will Convert Kelvin to Celcius
  return Temp;
}

int totalAvailableChange() {
  return ((quantity10 * 10) + (quantity5 * 5));
}
bool hasChange(int amount) {
  return ((quantity10 * 10) + (quantity5 * 5)) > amount;
}

void calculateChange(int amount) {
  int remainingAmount = amount;
  used10 = min(remainingAmount / 10, quantity10);
  remainingAmount -= used10 * 10;

  if (remainingAmount > 0) {
    used5 = min(remainingAmount / 5, quantity5);
    remainingAmount -= used5 * 5;
  }

  if (remainingAmount == 0) {
    quantity10 -= used10;
    quantity5 -= used5;

  } else {
    Serial.println("Insufficient coins to provide exact change.");
  }
}

bool despenseChange() {
  switch (changeMode) {
    case 1:
      used10 = 0;
      used5 = 0;
      changeMode = 2;
      break;

    case 2:
      if (is12Oz) {
        calculateChange(totalAmount - 15);
      } else {
        calculateChange(totalAmount - 20);
      }
      changeMode = 3;
      if (used5 > 0) {
        coinSorter1On();
        dispenser1Count = 0;
      }

      break;

    case 3:
      if (dispenser1Count != 0 && dispenser1Count % 5 == 0) {
        coinSorter1Off();
        delay(1000);
        coinSorter1On();
      }
      if (dispenser1Count >= used5) {
        coinSorter1Off();
        if (used10 > 0) {
          coinSorter2On();
          dispenser2Count = 0;
        }
        changeMode = 4;
      }
      break;

    case 4:
      if (dispenser2Count != 0 && dispenser2Count % 5 == 0) {
        coinSorter2Off();
        delay(1000);
        coinSorter2On();
      }
      if (dispenser2Count >= used10) {

        coinSorter2Off();
        changeMode = 0;
        EEPROM.put(ADDRESS5, quantity5);
        EEPROM.put(ADDRESS10, quantity10);

        return true;
      }
      break;
  }
  return false;
}
bool initialize() {
  switch (initMode) {

    case 0:
      lcd.setCursor(0, 0), lcd << F("====================");
      lcd.setCursor(0, 1), lcd << F("    INITIALIZING    ");
      lcd.setCursor(0, 2), lcd << F("                    ");
      lcd.setCursor(0, 3), lcd << F("====================");
      allOff();
      initMode = 1;

      break;

    case 1:
      upperBigStopperClose();
      lowerBigStopperClose();
      initMode = 2;
      break;

    case 2:
      if (moveRetract()) {
        initMode = 3;
      }
      break;

    case 3:
      if (gotoPos(60994L)) {
        initMode = 4;
      }
      break;

    case 4:
      coolerOn();
      lcd.setCursor(0, 0), lcd << F("====================");
      lcd.setCursor(0, 1), lcd << F("    COOLING JUICE   ");
      lcd.setCursor(0, 2), lcd << F("                    ");
      lcd.setCursor(0, 3), lcd << F("====================");
      if (getTemp1() <= MAX_TEMP || getTemp2() <= MAX_TEMP) {
        initMode = 5;
      }
      break;

    case 5:
      initMode = 0;
      return true;
      break;
  }



  return false;
}

float getTemp1() {
  return Thermistor(map(analogRead(TEMP_1_PIN), 0, 1023, 0, 1023));
}

bool withCup() {
  return analogRead(LDR_PIN) > 950;
}



void testing2() {
  if (Serial.available()) {
    String reading = Serial.readString();
    reading.trim();
    Serial << reading << endl;
    myStep = reading.toInt();  // myLong will be 1234567890
  }
  gotoPos(myStep);
}
void testing() {

  if (Serial.available()) {
    String reading = Serial.readString();
    reading.trim();
    Serial << reading << endl;
    if (reading == "0") {
      allOff();
      testMode = 0;
    } else if (reading == "1") {
      Serial << "Upper Big Open" << endl;
      upperBigStopperOpen();
    } else if (reading == "2") {
      Serial << "Upper Big Close" << endl;
      upperBigStopperClose();
    } else if (reading == "3") {
      Serial << "Lower Big Open" << endl;
      lowerBigStopperOpen();
    } else if (reading == "4") {
      Serial << "Lower Big Close" << endl;
      lowerBigStopperClose();
    } else if (reading == "5") {
      Serial << "Upper Small Open" << endl;
      upperSmallStopperOpen();
    } else if (reading == "6") {
      Serial << "Upper Small Close" << endl;
      upperSmallStopperClose();
    } else if (reading == "7") {
      Serial << "Lower Small Open" << endl;
      lowerSmallStopperOpen();
    } else if (reading == "8") {
      Serial << "Lower Small Close" << endl;
      lowerSmallStopperClose();
    } else if (reading == "q") {
      Serial << "START Big" << endl;
      lowerBigStopperOpen();
      delay(1000);
      lowerBigStopperClose();
      delay(1000);
      upperBigStopperOpen();
      delay(1000);
      upperBigStopperClose();
      delay(1000);
    } else if (reading == "w") {
      Serial << "START Small" << endl;
      lowerSmallStopperOpen();
      delay(1000);
      lowerSmallStopperClose();
      delay(1000);
      upperSmallStopperOpen();
      delay(1000);
      upperSmallStopperClose();
      delay(1000);
    } else if (reading == "a") {
      Serial << "Cooler On" << endl;
      coolerOn();
    } else if (reading == "s") {
      Serial << "Juicer 1 On" << endl;
      juicer1On();
    } else if (reading == "d") {
      Serial << "Juicer 2 On" << endl;
      juicer2On();
    } else if (reading == "f") {
      Serial << "Coin Sorter 1 On" << endl;
      coinSorter1On();
      dispenser1Count = 0;
    } else if (reading == "g") {
      Serial << "Coin Sorter 2 On" << endl;
      coinSorter2On();
      dispenser2Count = 0;
    } else if (reading == "h") {
      Serial << "Bill Acceptor On" << endl;
      billAcceptorOn();
      billPulseCount = 0;
    } else if (reading == "n") {
      Serial << "MOVE TO RETRACT" << endl;
      testMode = 1;
    } else if (reading == "m") {
      Serial << "MOVE TO EXTEND" << endl;
      testMode = 2;
    } else if (reading == "c") {
      Serial << "MOVE TO 16oz" << endl;
      testMode = 3;
    } else if (reading == "v") {
      Serial << "MOVE TO 12oz" << endl;
      testMode = 4;
    } else if (reading == "k") {
      Serial << "MOVE TO despense" << endl;
      testMode = 5;
    } else if (reading == "u") {
      Serial << "ACCEPT 20" << endl;
      Accept20Off();
    } else if (reading == "i") {
      Serial << "ACCEPT 20" << endl;
      Accept20On();
    } else if (reading == "o") {
      Serial << "ACCEPT 100" << endl;
      Accept100Off();
    } else if (reading == "p") {
      Serial << "ACCEPT 100" << endl;
      Accept100On();
    } else if (reading == "t") {
      Serial << "Buzzer On" << endl;
      BuzzerOn();
    } else if (reading == ",") {
      Serial << "Despense 12oz" << endl;
      juicer2On();
      myChrono.restart();
      flowmeter1Count = 0;
      testMode = 6;
    } else if (reading == ".") {
      Serial << "Despense 16oz" << endl;
      juicer2On();
      myChrono.restart();
      flowmeter2Count = 0;
      testMode = 7;
    } else if (reading == "/") {
      Serial << "Coin SLot off" << endl;
      coinSlotOn();
    } else if (reading == "Q") {
      Serial << "LEd 1 on" << endl;
      led1On();
    } else if (reading == "W") {
      Serial << "led 2 on" << endl;
      led2On();
    } else if (reading == "E") {
      Serial << "led 3 on" << endl;
      led3On();
    } else if (reading == "R") {
      Serial << "led 4 ON" << endl;
      led4On();
    } else if (reading == "T") {
      Serial << "led 5 on" << endl;
      led5On();
    } else if (reading == "Y") {
      Serial << "led 5 on" << endl;
      for (int i = 0; i < 10; i++) {  // Loop 5 times
        led1On();
        delay(100);
        led1Off();
        delay(100);
        led3On();
        delay(100);
        led3Off();
        delay(100);
        led4On();
        delay(100);
        led4Off();
        delay(100);
        led5On();
        delay(100);
        led5Off();
        delay(100);
      }
    }
  }

  switch (testMode) {
    case 0:
      Serial << step << endl;
      break;

    case 1:
      moveRetract();
      break;

    case 2:

      moveExtend();
      break;

    case 3:
      gotoPos(19200L);  //16oz
      break;

    case 4:
      gotoPos(2000L);  //12oz
      break;

    case 5:
      gotoPos(60500L);  //despense
      break;

    case 6:
      if (myChrono.hasPassed(10000)) {
        myChrono.restart();
        juicer2Off();
        testMode = 0;
      }
      break;

    case 7:
      if (myChrono.hasPassed(15000)) {
        myChrono.restart();
        juicer2Off();
        testMode = 0;
      }
      break;
  }
}

void allOff() {
  juicer1Off();
  juicer2Off();
  led1Off();
  led2Off();
  led3Off();
  led4Off();
  led5Off();
  coolerOff();
  coinSorter1Off();
  coinSorter2Off();
  billAcceptorOff();
  Accept20Off();
  Accept100Off();
  coinSlotOff();
  BuzzerOff();
}



float getTemp2() {
  return Thermistor(map(analogRead(TEMP_2_PIN), 0, 1023, 0, 1023));
}

void BuzzerOn() {
  digitalWrite(BUZZER_PIN, LOW);
}

void BuzzerOff() {
  digitalWrite(BUZZER_PIN, HIGH);
}

void Accept100On() {
  digitalWrite(BILL_DIP_100_PIN, HIGH);
}

void Accept100Off() {
  digitalWrite(BILL_DIP_100_PIN, LOW);
}

void Accept20On() {
  digitalWrite(BILL_DIP_20_PIN, HIGH);
}

void Accept20Off() {
  digitalWrite(BILL_DIP_20_PIN, LOW);
}

void billAcceptorOn() {
  digitalWrite(BILL_ACCEPTOR_PIN, LOW);
}

void billAcceptorOff() {
  digitalWrite(BILL_ACCEPTOR_PIN, HIGH);
}

void coinSorter1On() {
  digitalWrite(COIN_SORTER_1_PIN, HIGH);
}

void coinSorter1Off() {
  digitalWrite(COIN_SORTER_1_PIN, LOW);
}

void coinSorter2On() {
  digitalWrite(COIN_SORTER_2_PIN, HIGH);
}

void coinSorter2Off() {
  digitalWrite(COIN_SORTER_2_PIN, LOW);
}

void coinSlotOn() {
  digitalWrite(COIN_SLOT_PIN, LOW);
}

void coinSlotOff() {
  digitalWrite(COIN_SLOT_PIN, HIGH);
}


void coolerOn() {
  digitalWrite(COOLER_PIN, HIGH);
}


void coolerOff() {
  digitalWrite(COOLER_PIN, LOW);
}

void led1On() {
  digitalWrite(LED_1_PIN, LOW);
}

void led1Off() {
  digitalWrite(LED_1_PIN, HIGH);
}

void led2On() {
  digitalWrite(LED_2_PIN, LOW);
}

void led2Off() {
  digitalWrite(LED_2_PIN, HIGH);
}

void led3On() {
  digitalWrite(LED_3_PIN, LOW);
}

void led3Off() {
  digitalWrite(LED_3_PIN, HIGH);
}

void led4On() {
  digitalWrite(LED_4_PIN, LOW);
}

void led4Off() {
  digitalWrite(LED_4_PIN, HIGH);
}

void led5On() {
  digitalWrite(LED_5_PIN, LOW);
}

void led5Off() {
  digitalWrite(LED_5_PIN, HIGH);
}

void juicer1On() {
  digitalWrite(JUICER_1_PIN, HIGH);
}

void juicer1Off() {
  digitalWrite(JUICER_1_PIN, LOW);
}

void juicer2On() {
  digitalWrite(JUICER_2_PIN, HIGH);
}

void juicer2Off() {
  digitalWrite(JUICER_2_PIN, LOW);
}

bool isEmpty1Pressed() {
  flowSwitchDebouncer1.update();
  return flowSwitchDebouncer1.read();
}

bool isEmpty2Pressed() {
  flowSwitchDebouncer2.update();
  return flowSwitchDebouncer2.read();
}

bool isStopPressed() {
  return !digitalRead(STOP_PIN);
}



bool isUpPressed() {
  return !digitalRead(BUTTON_1_PIN);
}
bool isDownPressed() {
  return !digitalRead(BUTTON_2_PIN);
}

bool isEnterPressed() {
  return !digitalRead(BUTTON_3_PIN);
}

bool isBackPressed() {
  return !digitalRead(BUTTON_4_PIN);
}

bool isInHome() {
  return !digitalRead(HOME_PIN);
}

bool withCup1() {
  return !digitalRead(WITH_CUP_1_PIN);
}

bool withCup2() {
  return !digitalRead(WITH_CUP_2_PIN);
}



bool gotoPos(long juicerPos) {
  if (step > juicerPos) {
    moveRetract();
  } else if (step < juicerPos) {
    moveExtend();
  } else {
    digitalWrite(EN_PIN, HIGH);
    return true;
  }
  return false;
}


void moveExtend() {
  digitalWrite(EN_PIN, LOW);
  digitalWrite(DIR_PIN, HIGH);
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(10);  //15
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(10);
  step++;
}

bool moveRetract() {
  if (isInHome()) {
    step = 0;
    return true;
  } else {
    digitalWrite(EN_PIN, LOW);
    digitalWrite(DIR_PIN, LOW);
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(10);
    --step;
    return false;
  }
}


void upperBigStopperOpen() {
  moveServo(UPPER_BIG, 10);
}

void upperBigStopperClose() {
  moveServo(UPPER_BIG, 60);
}

void lowerBigStopperOpen() {
  moveServo(LOWER_BIG, 15);
}

void lowerBigStopperClose() {
  moveServo(LOWER_BIG, 140);
}

void upperSmallStopperOpen() {
  moveServo(UPPER_SMALL, 0);
}

void upperSmallStopperClose() {
  moveServo(UPPER_SMALL, 60);
}

void lowerSmallStopperOpen() {
  moveServo(LOWER_SMALL, 10);
}

void lowerSmallStopperClose() {
  moveServo(LOWER_SMALL, 180);
}

void moveServo(uint8_t servo, uint16_t degree) {
  uint16_t pulseLength = map(degree, 0, 180, SERVO_MIN, SERVO_MAX);
  pwm.setPWM(servo, 0, pulseLength);
}
