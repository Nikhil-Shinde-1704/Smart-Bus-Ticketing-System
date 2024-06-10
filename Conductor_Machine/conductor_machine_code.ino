
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buttonUp = A0;
const int buttonDown = A1;
const int buttonEnter = A2;
const int buttonExit = A3;

// List of all locations
const char *locations[] = {
  "Akurdi Railway St",
  "Sambhaji Chowk",
  "Bijalinagar Corn",
  "Chaphekar Chowk",
  "Chinchwad Gaon",
  "M.M.School BRTS",
  "Rahatani Phata",
  "Pimple Saudagar",
  "Pune Vidyapeeth",
  "Shivajinagar",
  "C.O.E.P.Hostel ",
  "Manapa Bhavan"
};

int selectedPickupIndex = -1;
int selectedDropIndex = -1;
float fare = -1.0;

int selectedOption = 1; // Initial menu option

bool paymentSectionActive = false;  // Flag to indicate if payment section is active
bool paymentCompleted = false;  // Flag to indicate if payment is completed

int count = 0;
char c;
String id;

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_PAGEADDR, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonEnter, INPUT_PULLUP);
  pinMode(buttonExit, INPUT_PULLUP);

  delay(2000);
  display.clearDisplay();
  display.display();

  Serial.println("Please select pickup and drop locations.");
}

void loop() {
  displayMenu(selectedOption);

  // Check button presses
  if (digitalRead(buttonUp) == LOW) {
    delay(200); // debounce
    selectedOption = (selectedOption > 1) ? (selectedOption - 1) : 4;
  }

  if (digitalRead(buttonDown) == LOW) {
    delay(200); // debounce
    selectedOption = (selectedOption < 4) ? (selectedOption + 1) : 1;
  }

  if (digitalRead(buttonEnter) == LOW) {
    delay(200); // debounce
    handleOption(selectedOption);
  }

  // Check if in Payment option and Exit button is pressed
  if (selectedOption == 3 && digitalRead(buttonExit) == LOW) {
    delay(200); // debounce
    if (paymentSectionActive) {
      returnToMainMenu();
    } else {
      handlePaymentOption();
    }
  }
}

void displayMenu(int option) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Display menu options
  display.print(F("1. Select Pickup.\n"));
  display.print(F("2. Select Drop.\n"));
  display.print(F("3. Payment.\n"));
  display.print(F("4. Exit.\n"));

  // Highlight the selected option
  display.setCursor(0, (option - 1) * 8);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.print(option);
  display.setTextColor(SSD1306_WHITE);

  display.display();
}

int selectLocation(const char *title, int &selectedIndex) {
  selectedIndex = 0;

  while (true) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(title);

    for (int i = selectedIndex; i < min(selectedIndex + SCREEN_HEIGHT / 8, sizeof(locations) / sizeof(locations[0])); ++i) {
      // Display numbers along with locations
      display.print(i + 1);
      display.print(". ");

      // Highlight the selected location
      if (i == selectedIndex) {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.print(">");
      } else {
        display.setTextColor(SSD1306_WHITE);
        display.print(" ");
      }

      display.println(locations[i]);
    }

    display.display();

    if (digitalRead(buttonUp) == LOW) {
      delay(200); // debounce
      selectedIndex = (selectedIndex > 0) ? (selectedIndex - 1) : 0;
    }

    if (digitalRead(buttonDown) == LOW) {
      delay(200); // debounce
      selectedIndex = (selectedIndex < sizeof(locations) / sizeof(locations[0]) - 1) ? (selectedIndex + 1) : selectedIndex;
    }

    if (digitalRead(buttonEnter) == LOW) {
      delay(200); // debounce
      // User has selected a location, exit the function
      display.clearDisplay();
      display.display();
      Serial.print(F("Selected Location Index: "));
      Serial.println(selectedIndex);
      return selectedIndex;
    }

    if (digitalRead(buttonExit) == LOW) {
      delay(200); // debounce
      return -1; // Indicate exit
    }
  }
}

float calculateFare(int pickupIndex, int dropIndex) {
  const float farePerStop = 10.0;
  const float baseFare = 5.0;

  float fare = abs(dropIndex - pickupIndex) * farePerStop + baseFare;

  return fare;
}

void handleOption(int option) {
  if (option == 1) {
    // Select Pickup
    selectedPickupIndex = selectLocation("Select Pickup", selectedPickupIndex);
  } else if (option == 2) {
    // Select Drop
    selectedDropIndex = selectLocation("Select Drop", selectedDropIndex);
  } else if (option == 3) {
    handlePaymentOption();
  } else if (option == 4) {
    // Exit option selected
    Serial.println(F("Exit option selected"));
    // Implement exit logic here
    returnToMainMenu();
  }
}

void handlePaymentOption() {
  // Payment option selected
  Serial.println(F("Payment option selected"));

  // Check if both pickup and drop locations are selected
  if (selectedPickupIndex != -1 && selectedDropIndex != -1) {
    fare = calculateFare(selectedPickupIndex, selectedDropIndex);

    // Display pickup, drop, and fare information
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(F("Pickup: "));
    display.println(locations[selectedPickupIndex]);
    display.print(F("Drop: "));
    display.println(locations[selectedDropIndex]);

    if (fare != -1.0) {
      display.print(F("Fare: $"));
      display.println(fare, 2);
    } else {
      display.println(F("Invalid locations"));
    }

    display.print(F("Scan RFID to complete payment."));

    display.display();

    // Activate payment section
    paymentSectionActive = true;

    // Buffer to store RFID data
    char rfidBuffer[13];
    int bufferIndex = 0;

    // Expected RFID tag
    const char expectedRFID[] = "AB123456789A";

    // Wait until payment is completed or exit button is pressed
    while (!paymentCompleted) {
      // Check if the exit button is pressed
      if (digitalRead(buttonExit) == LOW) {
        returnToMainMenu();
        break;
      }

      // Simulate RFID tag scanning
      while (Serial.available() > 0) {
        char c = Serial.read();

        // Check if the character is alphanumeric and buffer is not full
        if (isAlphaNumeric(c) && bufferIndex < 12) {
          rfidBuffer[bufferIndex++] = c;
        }

        // Introduce a delay to ensure complete RFID data is received
        delay(10);
      }

      // Check if the buffer contains a complete RFID tag
      if (bufferIndex == 12) {
        rfidBuffer[bufferIndex] = '\0'; // Null-terminate the string

        // Check if the scanned RFID is valid
        if (strcmp(rfidBuffer, expectedRFID) == 0) {
          Serial.println(F("Valid RFID. Payment completed."));
          paymentCompleted = true;
          // You might want to add additional logic or trigger an external event for payment completion
        } else {
          Serial.println(F("Invalid RFID. Payment not completed."));
          Serial.print(F("Expected RFID: "));
          Serial.println(expectedRFID);
        }

        // Clear the buffer for the next scan
        bufferIndex = 0;
      }

      // Introduce a delay before checking again
      delay(100);
    }

    // Reset flags
    paymentSectionActive = false;
    paymentCompleted = false;
  } else {
    Serial.println(F("Please select both pickup and drop locations."));
  }
}





void returnToMainMenu() {
  selectedOption = 1; // Return to the main menu
  selectedPickupIndex = -1;
  selectedDropIndex = -1;
  fare = -1.0;

  Serial.println("Please select pickup and drop locations.");
}
