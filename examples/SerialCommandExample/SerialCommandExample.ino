// Demo Code for StreamCommand Library
// April 2014
//
// based on code by
// Steven Cogswell
// May 2011

#include <StreamCommand.h>

#define arduinoLED 13   // Arduino LED on board

//StreamCommand sCmd;     // The demo SerialCommand object, default stream: Serial, maxCommands: 10
//StreamCommand sCmd(5);     // The demo SerialCommand object, default stream: Serial, maxCommands set to 5
StreamCommand sCmd(&Serial, 5);     // The demo SerialCommand object, stream set to Serial, maxCommands set to 5


void setup() {
  pinMode(arduinoLED, OUTPUT);      // Configure the onboard LED for output
  digitalWrite(arduinoLED, LOW);    // default to LED off

  Serial.begin(9600);

  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("ON",    LED_on);          // Turns LED on
  sCmd.addCommand("OFF",   LED_off);         // Turns LED off
  sCmd.addCommand("HELLO", sayHello);        // Echos the string argument back
  sCmd.addCommand("P",     processCommand);  // Converts two arguments to integers and echos them back
  sCmd.addCommand("Q",     processCommand);  // This will register
  sCmd.addCommand("R",     processCommand);  // This won't register due to the maxCommands set
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  sCmd.setNullHandler(blankLine);            // Handler for a blank line  (says "No Input.")
  sCmd.println("Ready");
}

void loop() {
  sCmd.readStream();     // We don't do much, just process serial commands
}

void LED_on() {
  sCmd.println("LED on");
  digitalWrite(arduinoLED, HIGH);
}

void LED_off() {
  sCmd.println("LED off");
  digitalWrite(arduinoLED, LOW);
}

void sayHello() {
  char *arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    sCmd.print("Hello ");
    sCmd.println(arg);
  }
  else {
    sCmd.println("Hello, whoever you are");
  }
}

void processCommand() {
  int aNumber;
  char *arg;

  sCmd.println("We're in processCommand");
  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    sCmd.print("First argument was: ");
    sCmd.println(aNumber);
  }
  else {
    sCmd.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atol(arg);
    sCmd.print("Second argument was: ");
    sCmd.println(aNumber);
  }
  else {
    sCmd.println("No second argument");
  }
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  sCmd.println("What?");
}

// This gets set as the null handler, and gets called a blank line is received.
void blankLine() {
  Serial.println("No input.");
}
