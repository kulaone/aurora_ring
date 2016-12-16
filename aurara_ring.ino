/*
 *  Aurora Ring
 */

#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <SimpleTimer.h>

// Wi-Fi info
const char* ssid = "XXXXXX";
const char* password = "XXXXXX";

const int NUM_PIXELS 24;

// Host info
const char* host = "aurorawatch-api.lancs.ac.uk";
// Set up the
Adafruit_NeoPixel pixels(NUM_PIXELS, D2, NEO_GRB | NEO_KHZ800);
WiFiClient client;
// the timer object
SimpleTimer timer;

// Colos and brightness
const uint32_t noColor = pixels.Color(0, 0, 0); // No color
const uint32_t cBlue = pixels.Color(0, 0, 200); // Blue
const uint32_t cGreen = pixels.Color(0, 100, 0); // Green
const uint32_t cYellow = pixels.Color(255, 255, 0); // Yellow
const uint32_t cAmber = pixels.Color(255, 126, 0); // Amber
const uint32_t cRed = pixels.Color(255, 0, 0); // Red
const int brightnessDefault = 50;
const int brightnessYellow = 60;
const int brightnessAmber = 80;
const int brightnessRed = 100;
// Alert level
const int alertYellow = 50;
const int alertAmber = 100;
const int alertRed = 200;

const int ringRatio = alertRed / NUM_PIXELS;
const int delayedLedAnumation = 50;

int connectionWaitID; // timer Id of the connection wait function
int showLedId = 0; // timer Id of the showLed function
int nextLed2Show = 0;
int ringLevel = 0;
uint32_t ringColor;

// If you need to test the ring, change this to true.
const boolean testRingMode = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  if (testRingMode) {
    timer.setInterval(3000, testRing);
  } else {
    timer.setTimeout(100, connecting2wifi);
  }
}

void connecting2wifi() {
  pixels.setBrightness(brightnessDefault);
  pixels.begin();
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  setRing2Blue();
  connectionWaitID = timer.setInterval(500, connectionWait);
}

void connectionWait() {
  Serial.print(".");
  if (WiFi.status() == WL_CONNECTED) {
    timer.disable(connectionWaitID);
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("connecting to: ");
    Serial.println(host);
    mainLoop();
  }
}

void loop() {
    timer.run();
}

void mainLoop() {
  // Use WiFiClient class to create TCP connections
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    setRing2Blue();
    return;
  }

  // Create a URI for the request
  String url = "/0.2/status/alerting-site-activity.xml";
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // Send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  timer.setTimeout(500, readServer);
}

// Read the replay form the server and get the last activity value.
void readServer(){
  while(client.available()){
    String line = client.readStringUntil('\r');
    String siteActivity = getXmlTag(line,"site_activity");
    if (siteActivity.length() > 0){
      String activity = getLastXmlTag(siteActivity,"activity");
      if (activity.length() > 0){
        String value = getXmlTag(activity,"value");
        if (value.length() > 0){
          Serial.print("Last activity = ");
          Serial.println(value);
          setRing(value.toInt());
        }
      }
    }
  }
  Serial.println();
  Serial.println("closing connection");
  digitalWrite(LED_BUILTIN, HIGH);
  timer.setTimeout(1000*60*3, mainLoop);
}

// Get a XML and return the first tag in it
String getXmlTag(String xml, String tagName) {
  String result;
  int startTag = xml.indexOf("<"+tagName);
  if(startTag > 0) {
    String tagFull = xml.substring(startTag);
    int tagEnds = tagFull.indexOf(">");
    String value = tagFull.substring(tagEnds+1);
    int endTag = value.indexOf("</"+tagName+">");
    result = (value.substring(0,endTag));
  }
  return result;
}
// Get a XML and return the last tag in it
String getLastXmlTag(String xml, String tagName) {
  String result;
  int startTag = xml.lastIndexOf("<"+tagName);
  if(startTag > 0) {
    String tagFull = xml.substring(startTag);
    int tagEnds = tagFull.indexOf(">");
    String value = tagFull.substring(tagEnds+1);
    int endTag = value.indexOf("</"+tagName+">");
    result = (value.substring(0,endTag));
    }
  return result;
}
// Set the led ring according to the activity level
// and show the leds with a delayed anumation.
void setRing(int level){
  Serial.print("Set ring to activity level:");
  Serial.println(level);
  ringColor = cGreen; // Green
  int brightness = brightnessDefault;
  pixels.setBrightness(brightnessDefault);
  if (level >= alertYellow && level < alertAmber) { // yellow
      ringColor = cYellow;
      brightness = brightnessYellow;
  } else if (level >= alertAmber && level < alertRed) { // amber
      ringColor = cAmber;
      brightness = brightnessAmber;
  } else if ( level >= alertRed) { // red
      ringColor = cRed;
      brightness = brightnessRed;
  }
  if (level > alertRed) {
    level = alertRed;
  }
  pixels.setBrightness(brightness);

  ringLevel = level;
  nextLed2Show = 0;
  if (showLedId > 0) {
    timer.enable(showLedId);
  } else {
    showLedId = timer.setInterval(delayedLedAnumation, showLed);
  }
}

// This function light each led in its turn
void showLed(){
  int pi = ringLevel / ringRatio;
  uint32_t ledColor = noColor;
  if (nextLed2Show < pi ) {
    ledColor = ringColor;
  }
  pixels.setPixelColor(nextLed2Show, ledColor);
  pixels.show();
  nextLed2Show++;
  if (nextLed2Show >= NUM_PIXELS) {
    timer.disable(showLedId);
  }
}
// Set all the ring to blue.
void setRing2Blue() {
  for(int i=0;i<NUM_PIXELS;i++){
     pixels.setPixelColor(i, cBlue);
  }
  pixels.show();
}
// Testing the ring leds
void testRing(){
  int level = (int) random(250);
  setRing(level);
}