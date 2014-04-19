/**
 * StreamCommand - A Wiring/Arduino library to tokenize and parse commands
 * received over a stream port.
 * 
 * Copyright (C) 2014 Mauricio Jabur
 * 
 * SerialCommand:
 * Copyright (C) 2014 Craig Versek
 * Copyright (C) 2012 Stefan Rado
 * Copyright (C) 2011 Steven Cogswell <steven.cogswell@gmail.com>
 *                    http://husks.wordpress.com
 * 
 * Version 20140419
 * 
 * Updated for blank line support by DeKay, Feb 2014
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
 
 // TODO: implement a second delimiter for the arguments, in order to understand commands like 'RGB 220,0,234'
 
#ifndef StreamCommand_h
#define StreamCommand_h

#if defined(WIRING) && WIRING >= 100
  #include <Wiring.h>
#elif defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif
#include <string.h>
#include <Stream.h>

// Size of the input buffer in bytes (maximum length of one command plus arguments)
#define STREAMCOMMAND_BUFFER 32
// Maximum number of commands to be added
#define STREAMCOMMAND_MAXCOMMANDS_DEFAULT 10

// Uncomment the next line to run the library in debug mode (verbose messages)
//#define STREAMCOMMAND_DEBUG 

class StreamCommand : public Print{
  public:
    StreamCommand(Stream *stream = &Serial,
                  byte maxCommands = STREAMCOMMAND_MAXCOMMANDS_DEFAULT
                 );       // Constructor
    StreamCommand(
                  byte maxCommands
                 );       // Constructor
    void setStream(Stream *stream);  // Change the Stream object
    void addCommand(const char *command, void(*function)());           // Add a command to the processing dictionary.
    void addCommand(__FlashStringHelper *command, void(*function)());  // Add a command to the processing dictionary.
    void setDefaultHandler(void (*function)(const char *));                         // A handler to call when no valid command received.
    void setNullHandler(void (*function)());      // A handler to call when empty command received.

    void readStream();    // Main entry point.
    void clearBuffer();   // Clears the input buffer.
    char *next();         // Returns pointer to next token found in command buffer (for getting arguments to commands).

    size_t write(uint8_t val); //provide method for printing
	
  private:
  Stream *_stream;
    // Command/handler dictionary
    struct StreamCommandCallback {
      const char *command;
      void (*function)();
    };                                     // Data structure to hold Command/Handler function key-value pairs
    StreamCommandCallback *_commandList;  // Actual definition for command/handler array
    byte _commandCount;
	byte _maxCommands;

    // Pointer to the default handler function
    void (*_defaultHandler)(const char *);
    // Pointer to the null handler function
    void (*_nullHandler)();

    char _delim[2]; // null-terminated list of character to be used as delimiters for tokenizing (default " ")
    char _term;     // Character that signals end of command (default '\n')

    char _buffer[STREAMCOMMAND_BUFFER + 1]; // Buffer of stored characters while waiting for terminator character
    byte _bufPos;                           // Current position in the buffer
    char *_last;                            // State variable used by strtok_r during processing
};

#endif //StreamCommand_h
