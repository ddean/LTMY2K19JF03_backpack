#include <TinyWireS.h>

unsigned int characterMap[] = {
  0x3f44, 0x1800, 0x3622, 0x3c20, 0x1922, 0x2512, 0x2f22, 0x3800, 0x3f22, 0x3d22, 0x3b22, 0x3ca8, 0x2700, 0x3c88, 0x2722, 0x2302,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x24aa, 0x6b, 0x27aa, 0x3122, 0x341, 0x4aa, 0x9c, 0xc9,
  0x0, 0x1800, 0x180, 0x3fff, 0x2daa, 0x977, 0x2c55, 0x40, 0x50, 0x5, 0xff, 0xaa, 0x4, 0x22, 0x60a, 0x44,
  0x3f44, 0x1800, 0x3622, 0x3c20, 0x1922, 0x2512, 0x2f22, 0x3800, 0x3f22, 0x3d22, 0x88, 0x84, 0x50, 0x422, 0x5, 0x3028,
  0x37a0, 0x3b22, 0x3ca8, 0x2700, 0x3c88, 0x2722, 0x2302, 0x2f20, 0x1b22, 0x88, 0x1e00, 0x352, 0x700, 0x1b41, 0x1b11, 0x3f00,
  0x3322, 0x3f10, 0x3332, 0x2d22, 0x2088, 0x1f00, 0x344, 0x1b14, 0x55, 0x49, 0x2444, 0x2700, 0x11, 0x3c00, 0x3044, 0x400,
  0x1, 0x3b22, 0x3ca8, 0x2700, 0x3c88, 0x2722, 0x2302, 0x2f20, 0x1b22, 0x88, 0x1e00, 0x352, 0x700, 0x1b41, 0x1b11, 0x3f00,
  0x3322, 0x3f10, 0x3332, 0x2d22, 0x2088, 0x1f00, 0x344, 0x1b14, 0x55, 0x49, 0x2444, 0x52, 0x88, 0x25, 0x1222, 0x3f00,
}; 

byte digitsMap[] = {
  B01111110,  //0
  B00110000,
  B01101101,
  B01111001,
  B00110011,
  B01011011,
  B01011111,
  B01110000,
  B01111111,
  B01110011,
  B00000000
};

//=========================

unsigned int alphaNum[7] = {0, 0, 0, 0, 0, 0, 0};
boolean alphaColonUpper;
boolean alphaColonLower;

boolean mailIcon;
boolean blinkMailIcon;

boolean phoneIcon;
boolean blinkPhoneIcon;

boolean diskIcon;
boolean blinkDiskIcon;

boolean netIcon;
boolean blinkNetIcon;

byte digits[7] = {0, 0, 0, 0};    // three extra
boolean digitColonUpper;
boolean digitColonLower;
boolean blinkDigits;
boolean blinkColon;

boolean alphaNumColon;
boolean blinkAlphaNumColon;

boolean on = true;

/*=========================

Aabbcdefg
Nnn[:.*]
IMTDN
C

===========================*/

int dataPin = 3;      // arduino = 3
int clockPin = 4;     // arduino = 4

void setup(void) {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(clockPin, LOW);
  
  TinyWireS.begin(4);                // join i2c bus with address #4
  TinyWireS.onReceive(handleCommand);
}

void handleCommand(byte nBytes)
{
  if (TinyWireS.available() > 0) {
    char cmd = TinyWireS.receive();
    
    switch( cmd ) {
    case 'A':
      for( int i = 0; i < 7; i++) {
        alphaNum[i] = characterMap[TinyWireS.available() > 0 ? TinyWireS.receive() : ' '];
      }
      break;
    
    case 'N':
    case 'n':
      {
        int ix = 0;
        
        blinkDigits = cmd == 'n';
        blinkColon = false;
        
        digitColonUpper = digitColonLower = false;
        
        for (int i = 0; i < 5; i++) {
          char ch = TinyWireS.available() > 0 ? TinyWireS.receive() : ' ';
          
          switch(ch) {
            case ' ':
              digits[ix++] = 0;
              break;
              
            case '.':
              digitColonUpper = false;
              digitColonLower = true;
              break;
              
            case '*':
              digitColonUpper = true;
              digitColonLower = false;
              break;
              
            case ':':
            case ';':
              digitColonUpper = true;
              digitColonLower = true;
              blinkColon = ch == ';';
              break;
              
            default:
              if (ch >= '0' && ch <= '9') {
                digits[ix++] = digitsMap[ch - '0'];
              } else {
                digits[ix++] = 0;
              }
              break;
          }
          
          if (ix > 3) {
            break;
          }
        }
      }
      break;
    
    case 'I':
      mailIcon = phoneIcon = diskIcon = netIcon = false;
      blinkMailIcon = blinkPhoneIcon = blinkDiskIcon = blinkNetIcon = false;

      while( TinyWireS.available() ) {
        char icon = TinyWireS.receive();
        
        switch(icon) {
          case 'M':
          case 'm':
            mailIcon = true;
            blinkMailIcon = icon == 'm';
            break;
            
          case 'T':
          case 't':
            phoneIcon = true;
            blinkPhoneIcon = icon == 't';
            break;

          case 'D':
          case 'd':
            diskIcon = true;
            blinkDiskIcon = icon == 'd';
            break;

          case 'N':
          case 'n':
            netIcon = true;
            blinkNetIcon = icon == 'n';
            break;
        }
      }
    break;
    
    case 'C':
      alphaNumColon = blinkAlphaNumColon = false;
      if (TinyWireS.available()) {
        char ch = TinyWireS.receive();
        alphaNumColon = ch == ':' || ch == ';';
        blinkAlphaNumColon = ch == ';';
      }
      break;
      
    case 'X':
      mailIcon = phoneIcon = diskIcon = netIcon = false;
      blinkMailIcon = blinkPhoneIcon = blinkDiskIcon = blinkNetIcon = false;
      alphaNumColon = blinkAlphaNumColon = false;
      digitColonUpper = digitColonLower = false;
      for(int i = 0; i < 7; i++) {
        digits[i] = 0;
        alphaNum[i] = 0;
      } 
      break;
    
    case 'T':
      mailIcon = phoneIcon = diskIcon = netIcon = true;
      blinkMailIcon = blinkDiskIcon = true;
      blinkPhoneIcon = blinkNetIcon = false;
      digitColonUpper = digitColonLower = true;
      alphaNumColon = blinkAlphaNumColon = true;
      for(int i = 0; i < 7; i++) {
        alphaNum[i] = characterMap['A' + i];
        digits[i] = digitsMap[i];
      } 
      break;
    }

    while (TinyWireS.available()) {
      TinyWireS.receive();
    }
  }
}


void refresh() {
  byte transistor, tmask;
  boolean b;

  for(int r = 0; r < 5; r++) {
    digitalWrite(clockPin, LOW);

    writeBit(true);  // start bit
    
    writeAlphaNum(alphaNum[4 - r]);

    if(r < 2) {
      writeAlphaNum(alphaNum[6 - r]);
    }
    
    switch( r ) {
      case 2:
        writeDigit(1);
        writeDigit(3);
        break;
        
      case 3:
        writeDigit(0);
        writeDigit(2);
        break;
        
      case 4:
        writeBit(digitColonUpper && (!blinkColon || on));
        writeBit(digitColonLower && (!blinkColon || on));

        writeBit(false);

        b = mailIcon && (!blinkMailIcon || on);
        writeBit(b);
        writeBit(b);

        b = phoneIcon && (!blinkPhoneIcon || on);
        writeBit(b);
        writeBit(b);

        b = diskIcon && (!blinkDiskIcon || on);
        writeBit(b);
        writeBit(b);

        b = netIcon && (!blinkNetIcon || on);
        writeBit(b);
        writeBit(b);

        b = alphaNumColon && (!blinkAlphaNumColon || on);
        writeBit(b);
        writeBit(b);
        
        writeBit(false);  // unusedAA
        
        break;        
    }    
    
    writeBit(false);  // "don't care" bit
    
    tmask = 0x10;
    transistor = 1 << r;
    
    while (tmask > 0) {
      writeBit((transistor & tmask) > 0);
      tmask >>= 1;
    }
    
    writeBit(false);
  }
}

void writeBit(bool b) {
    digitalWrite(dataPin, b ? HIGH : LOW);
    delayMicroseconds(5);
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(clockPin, LOW);
}

void writeAlphaNum(unsigned int val) {
  unsigned int mask = 0x2000;
  
  while (mask > 0) {
    writeBit((val & mask) != 0);
    
    mask >>= 1;
  }  
}

void writeDigit(int ix) {
  byte mask = 0x40;
  byte val = (!blinkDigits || on) ? digits[ ix ] : B0000;
  
  while (mask > 0) {
    writeBit((val & mask) != 0);
    
    mask >>= 1;
  }  
}

unsigned long prevTime = millis();
unsigned long currTime;

void loop(void) {
  refresh();
  
  currTime = millis();
  if (currTime - prevTime > 666) {
    on = !on;
    prevTime = currTime;
  }
  
  TinyWireS_stop_check();
}
