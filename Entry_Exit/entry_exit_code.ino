const int entrySensorPin = 2;
const int exitSensorPin = 3;
const int entryLedPin = 5;
const int exitLedPin = 6;
const int motorPin = 7;           // Motor control pin
const int startButtonPin = 8;     // Pin connected to the start button

int entryCount = 0;
int exitCount = 0;
int totalPersons = 0;

void setup() {
  Serial.begin(9600);

  pinMode(entrySensorPin, INPUT);
  pinMode(exitSensorPin, INPUT);
  pinMode(entryLedPin, OUTPUT);
  pinMode(exitLedPin, OUTPUT);
  pinMode(motorPin, OUTPUT);       // Motor control pin
  pinMode(startButtonPin, INPUT_PULLUP);  // Start button pin with internal pull-up resistor
}

void loop() {
  // Check if the start button is pressed (motor is running)
  if (digitalRead(startButtonPin) == HIGH) {
    // Check if a person is entering
    if (digitalRead(entrySensorPin) == HIGH) {
      delay(1000);  // Debounce delay (adjust as needed)
      if (digitalRead(entrySensorPin) == HIGH) {
        entryCount++;
        totalPersons++;
        updateDatabase("Enter", entryCount);
        indicateEntry();
        printTotalPersons();
      }
    }

    // Check if a person is exiting
    if (digitalRead(exitSensorPin) == HIGH && exitCount < entryCount) {
      delay(1000);  // Debounce delay (adjust as needed)
      if (digitalRead(exitSensorPin) == HIGH) {
        exitCount++;
        totalPersons--;
        updateDatabase("Exit", exitCount);
        indicateExit();
        printTotalPersons();
      }
    }
  }

  // Add additional logic or functionality as needed

  delay(500);  // Adjust the delay based on your requirements
}

void updateDatabase(String direction, int count) {
  // Implement the code to update the local database (e.g., text file)
  // You can use the SD card library or other storage options depending on your setup
  // Include direction and count in the database entry

  // For this example, we will send the data to the computer via serial
  Serial.print(direction);
  Serial.print(",");
  Serial.println(count);
}

void indicateEntry() {
  digitalWrite(entryLedPin, HIGH);  // Turn on green LED
  delay(1000);  // Indication duration (adjust as needed)
  digitalWrite(entryLedPin, LOW);   // Turn off green LED
}

void indicateExit() {
  digitalWrite(exitLedPin, HIGH);   // Turn on red LED
  delay(1000);  // Indication duration (adjust as needed)
  digitalWrite(exitLedPin, LOW);    // Turn off red LED
}

void printTotalPersons() {
  Serial.print("Total Persons: ");
  Serial.println(totalPersons);
}
