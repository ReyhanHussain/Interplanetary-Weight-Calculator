#include <ESP8266WiFi.h>
#include <Servo.h>

// Servo for weight and planet display
Servo combinedServo;
const int combinedServoPin = D5;  // Pin shared by both servos

// Rotary Encoder Pins
const int encoderPinA = D1;
const int encoderPinB = D2;

// Push Button Pin
const int buttonPin = D3;

// LED Pins
const int weightInputLED = D7;
const int planetSelectLED = D8;
const int displayWeightLED = D4;

// Buzzer Pin
const int buzzerPin = D0;


const char *ssid = "WIFI NAME";
const char *password = "PASSW";

int earthWeight = 0;
int currentPlanet = 0;
const char *planets[] = { "Mercury", "Venus", "Moon", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune" };
float gravityRatios[] = { 0.38, 0.91, 0.165, 0.38, 2.53, 1.07, 0.89, 1.14 };

int lastStateA;
int combinedServoAngle = 0;
bool weightEntered = false;
bool planetSelected = false;

WiFiServer server(80);

void setup() {

  combinedServo.attach(combinedServoPin);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(buttonPin, INPUT);

  pinMode(weightInputLED, OUTPUT);
  pinMode(planetSelectLED, OUTPUT);
  pinMode(displayWeightLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Server started");

  displayStartMessage();
}

void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    String response = generateHTML();
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    client.print(response);
  }

  if (!weightEntered) {
    handleWeightInput();
  } else if (weightEntered && !planetSelected) {
    handlePlanetSelection();
  } else if (planetSelected) {
    displayWeightOnPlanet();
  }
}

void displayStartMessage() {
  Serial.println("===================================================");
  Serial.println("|        * INTER PLANETARY WEIGHT CALCULATOR *    |");
  Serial.println("|                     # Welcome! #                |");
  Serial.println("===================================================");
  Serial.println("|----------------------------------------------------------------------|");
  Serial.println("|     This system will calculate your weight on different planets.     |");
  Serial.println("|----------------------------------------------------------------------|");
  Serial.println("|    1. Rotate the encoder to enter your weight on Earth.              |");
  Serial.println("|    2. Press the button to confirm your weight.                       |");
  Serial.println("|    3. Rotate the encoder to select a planet.                         |");
  Serial.println("|    4. Press the button to confirm the planet.                        |");
  Serial.println("|----------------------------------------------------------------------|");
  Serial.println("WORKING REAL LIFE APPLICATION OF MATHS IN DETERMING OBJECT WEIGHT[NOT MASS] ON OTHER PLANETS");
  Serial.println("                    PROJECT MADE BY STUDENTS OF BGSCET");
  digitalWrite(weightInputLED, HIGH);
  buzzShort();
  Serial.println("-------------------------------------------------------");
  Serial.println(" STEP 1) Select your Weight on earth[0 - 100kg] .");
  Serial.println("-------------------------------------------------------");
}

void handleWeightInput() {
  int currentStateA = digitalRead(encoderPinA);
  if (currentStateA != lastStateA) {
    earthWeight = updateEncoderValue(currentStateA, encoderPinB, earthWeight, 0, 100);
    updateServoDisplay(earthWeight, 0, 100);
    Serial.print(">> Earth Weight: ");
    Serial.print(earthWeight);
    Serial.println(" kg");

    lastStateA = currentStateA;
  }

  if (digitalRead(buttonPin) == LOW) {
    weightEntered = true;
    Serial.println("");
    Serial.println("-------------------------------------------------------");
    Serial.println(" STEP 2) Rotate the encoder to select a planet.");
    Serial.println("-------------------------------------------------------");

    digitalWrite(weightInputLED, LOW);
    digitalWrite(planetSelectLED, HIGH);
    buzzShort();
    delay(500);
  }
}

void handlePlanetSelection() {
  int currentStateA = digitalRead(encoderPinA);
  if (currentStateA != lastStateA) {
    currentPlanet = updateEncoderValue(currentStateA, encoderPinB, currentPlanet, 0, 7);
    updateServoDisplay(currentPlanet, 0, 7);

    Serial.println(">>Selected Planet: ");
    Serial.println(planets[currentPlanet]);

    lastStateA = currentStateA;
  }

  if (digitalRead(buttonPin) == LOW) {
    planetSelected = true;
    digitalWrite(planetSelectLED, LOW);
    digitalWrite(displayWeightLED, HIGH);
    buzzShort();
    delay(500);
  }
}

void displayWeightOnPlanet() {
  float planetWeight = earthWeight * gravityRatios[currentPlanet];
  updateServoDisplay(planetWeight, 0, 100);
  Serial.println("-------------------------------------------------------");
  Serial.print(">> YOUR WEIGHT ON ");
  Serial.print(planets[currentPlanet]);
  Serial.print(" WILL BE: ");
  Serial.print(planetWeight, 2);
  Serial.println(" kg <<");
  Serial.println("-------------------------------------------------------");
  blinkMessage();
  Serial.println(" Resting.......");
  delay(5000);
  resetProgram();
}

void resetProgram() {
  weightEntered = false;
  planetSelected = false;
  earthWeight = 0;
  currentPlanet = 0;

  digitalWrite(displayWeightLED, LOW);
  digitalWrite(weightInputLED, HIGH);
  buzzShort();
}

String generateHTML() {
  String html = "<!DOCTYPE html><html><head><title>Weight Calculator</title>";
  html += "<meta http-equiv='refresh' content='1'>";  // Auto-refresh every 1 second
  html += "<style>";

  html += "body { font-family: 'Courier New', monospace; color: #00FFDD; background-color: #000; margin: 0; padding: 20px; text-align: center; }";
  html += "h1, h2, h3 { color: #FF00FF; text-shadow: 0 0 10px #FF00FF; }";
  html += "p, li, td, th { color: #00FFDD; text-shadow: 0 0 5px #00FFDD; }";
  html += "a { color: #FF00FF; text-decoration: none; }";
  html += "a:hover { color: #00FFDD; text-decoration: underline; }";

  html += ".card { background: #111; border-radius: 8px; padding: 20px; margin: 20px auto; max-width: 700px; box-shadow: 0 0 20px #00FFDD; }";
  html += "button { background: #FF00FF; border: none; color: #000; padding: 10px 20px; border-radius: 5px; cursor: pointer; }";
  html += "button:hover { background: #00FFDD; color: #000; }";

  html += "table { width: 100%; border-collapse: collapse; margin-top: 20px; }";
  html += "th, td { border: 1px solid #00FFDD; padding: 10px; text-align: center; }";
  html += "th { background: #222; }";

  html += "ul { text-align: left; padding-left: 40px; }";

  html += "</style></head><body>";
  html += "<h1>Interplanetary Weight Calculator</h1><h4>DEVELOPED BY STUDENTS OF BGSCET</h4><div class='card'>";

  if (!weightEntered) {
    html += "<p>Rotate the encoder to set your weight on Earth.</p>";
    html += "<p>Weight: <b>" + String(earthWeight) + " kg</b></p>";
  } else if (!planetSelected) {
    html += "<p>Rotate the encoder to select a planet.</p>";
    html += "<p>Selected Planet: <b>" + String(planets[currentPlanet]) + "</b></p>";
  } else {
    float planetWeight = earthWeight * gravityRatios[currentPlanet];
    html += "<p>Weight on <b>" + String(planets[currentPlanet]) + "</b>: <b>" + String(planetWeight, 2) + " kg</b></p>";
    html += "<p>Reset the program to enter values for another user.</p>";
  }

  html += "</div><div class='info'>";
  html += "<pre></pre>";
  html += "<h2>About the Weight Calculation</h2>";
  html += "<p><b>Formula:</b> Weight on Planet = Weight on Earth × Gravity Ratio of Planet</p>";
  html += "<pre></pre>";
  html += "<h3>Gravity Ratios of Planets</h3>";
  html += "<table><tr><th>Planet</th><th>Gravity Ratio</th></tr>";
  for (int i = 0; i < 8; i++) {
    html += "<tr><td>" + String(planets[i]) + "</td><td>" + String(gravityRatios[i], 2) + "</td></tr>";
  }
  html += "</table>";

  html += "<h3>Planetary Details</h3>";
  html += "<ul>";
  html += "<li><b>Mercury:</b> Closest planet to the Sun with extreme temperatures.</li>";
  html += "<li><b>Venus:</b> Similar in size to Earth but has a toxic atmosphere.</li>";
  html += "<li><b>Moon:</b> Earth&#39;s only natural satellite, with 1/6th of Earth&#39;s gravity.</li>";
  html += "<li><b>Mars:</b> Known as the Red Planet, has the largest volcano in the solar system.</li>";
  html += "<li><b>Jupiter:</b> The largest planet, with intense gravity and storms.</li>";
  html += "<li><b>Saturn:</b> Famous for its prominent ring system.</li>";
  html += "<li><b>Uranus:</b> Rotates on its side and has a bluish color due to methane.</li>";
  html += "<li><b>Neptune:</b> The farthest planet, known for its deep blue color and strong winds.</li>";
  html += "</ul>";

  html += "<h3>Safety Notes and Dangers</h3>";
  html += "<ul>";
  html += "<li>Ensure accurate weight input for precise calculations.</li>";
  html += "<li>Some planets have extreme gravity—hypothetical weight may not be survivable!</li>";
  html += "<li>This tool is for educational purposes and assumes ideal conditions.</li>";
  html += "<li>The Curved Scale May not be Precise</li>";

  html += "</ul>";

  html += "<h3>Outcomes?</h3>";
  html += "<ul>";
  html += "<li>Our project applies concepts like scaling, range mapping, and logic to create an educational tool.</li>";
  html += "</ul>";

  html += "<ul>";
  html += "<li>We combined mathematics, engineering, and programming to control hardware and software.</li>";
  html += "</ul>";

  html += "<ul>";
  html += "<li>Understanding gravity ratios highlights math's role in physics and engineering.</li>";
  html += "</ul>";

  html += "<ul>";
  html += "<li>Our project teaches planetary gravity and weight in an engaging, interactive way.</li>";
  html += "</ul>";
  html += "<pre></pre>";
  html += "<h3>Creative Servo Display</h3>";
  html += "<ul>";
  html += "<li>Instead of an LCD, we used a servo motor to display values by mapping numbers to angles:</li>";
  html += "</ul>";
  html += "<ul>";
  html += "<li>Servo Angle = map(Value, Min, Max, 0, 180)</li>";
  html += "</ul>";
  html += "<ul>";
  html += "<li>Interactive: Servo motion makes outputs engaging.</li>";
  html += "</ul>";
  html += "<ul>";
  html += "<li>Resourceful: We innovatively overcame hardware limitations.</li>";
  html += "</ul>";
  html += "<ul>";
  html += "<li>A retro website Them, Inspired by old Sci-Fi Space Movies, neon lights...etc suits perfectly.</li>";
  html += "</ul>";
  html += "<pre></pre>";
  html += "<h3  >  Key Learning Outcome</h3>";
  html += "<ul>";
  html += "<li>We demonstrated how abstract mathematical concepts power real-world solutions in STEM projects.</li>";
  html += "</ul>";
  html += "</div></body></html>";
  return html;
}

int updateEncoderValue(int currentStateA, int pinB, int value, int minValue, int maxValue) {
  if (digitalRead(pinB) != currentStateA) {
    value++;
  } else {
    value--;
  }
  if (value > maxValue) {
    value = minValue;
  } else if (value < minValue) {
    value = maxValue;
  }
  return value;
}

void updateServoDisplay(float value, float minValue, float maxValue) {
  combinedServoAngle = map(value, minValue, maxValue, 0, 180);
  combinedServo.write(combinedServoAngle);
}

void buzzShort() {
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
}

void blinkMessage() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(displayWeightLED, LOW);
    delay(200);
    digitalWrite(displayWeightLED, HIGH);
    delay(200);
  }
}
