#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h> // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <Wire.h>
#include <Sodaq_SHT2x.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

unsigned int localPort = 16520; // UECS Broadcast port

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

  // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,16,2);
int  cp,dir;
boolean dsts;

int  sec10;
byte lip[4],bip[4]; // IP Address
byte nm[4]; // Netmask
char *uecs1txt,*uecs2txt,*uecs3txt,uecs2txtbuf[56],uecs3txtbuf[32];

void setup(void) {
  uint32_t ipa;
  int n;
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("t0e v 0.91");
  Ethernet.begin(mac);
  Udp.begin(localPort);
  delay(500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(Ethernet.localIP());
  sec10 = 0;
  ipa = Ethernet.localIP();
  for (n=0;n<4;n++) {
    lip[n] = (ipa >> (8*n))  & 0xFF;
  }
  ipa = Ethernet.subnetMask();
  for (n=0;n<4;n++) {
    nm[n] = (ipa >> (8*n))  & 0xFF;
  }
  for (n=0;n<4;n++) {
    bip[n] = lip[n] & nm[n];
    bip[n] = bip[n] | (~nm[n]);
  }
  uecs1txt = "<?xml version=\"1.0\"?><UECS ver=\"1.00-E10\">";
  uecs2txt = sprintf(uecs2txtbuf,"room=\"%d\" region=\"%d\" order=\"%d\" priority=\"%d\">",
		     4,5,1,28);
  uecs3txt = sprintf(uecs3txtbuf,"<IP>%d.%d.%d.%d</IP></UECS>",
		     lip[0],lip[1],lip[2],lip[3]);
}


void loop(void) {
  char tbuf[6],udpsendbuf[200];
  char *udptxt = "%s<DATA type=\"%s\" %s%s</DATA>%s";
  sprintf(udpsendbuf,udptxt,uecs1txt,"cnd.mNB",uecs2txtbuf,"D",uecs3txtbuf);
  Udp.beginPacket(bip,localPort);
  Udp.write(udpsendbuf);
  Udp.endPacket();
  if ((sec10 % 10)==0) { // every 10sec
    dsts = true;
    dtostrf((double)SHT2x.GetTemperature(),5,2,tbuf);
    sprintf(udpsendbuf,udptxt,uecs1txt,"InAirTemp.mNB",uecs2txtbuf,tbuf,uecs3txtbuf);
    Udp.beginPacket(bip,localPort);
    Udp.write(udpsendbuf);
    Udp.endPacket();
    dsp(0,tbuf);
    dsts = true;
    dtostrf((double)SHT2x.GetHumidity(),5,2,tbuf);
    sprintf(udpsendbuf,udptxt,uecs1txt,"InAirHumid.mNB",uecs2txtbuf,tbuf,uecs3txtbuf);
    Udp.beginPacket(bip,localPort);
    Udp.write(udpsendbuf);
    Udp.endPacket();
    dsp(6,tbuf);
    sec10 = 0;
  }
  sprintf(tbuf,"%1d",sec10);
  dsts = true;
  dsp(15,tbuf);
  sec10++;
  delay(1000);
}

void dsp(int p,char *s) {
  if (dsts) {
    lcd.setCursor(p,1);
    lcd.print(s);
    dsts=false;
  }
  return;
}
