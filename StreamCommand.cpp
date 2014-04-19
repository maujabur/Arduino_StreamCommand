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
 
#include "StreamCommand.h"

/**
 * Constructor makes sure some things are set.
 */
StreamCommand::StreamCommand(Stream *stream,
                             byte maxCommands
                            )
  : _stream(stream),
    _commandList(NULL),
    _commandCount(0),
    _defaultHandler(NULL),
    _nullHandler(NULL),
    _term('\r'),           // default terminator for commands, CR character
    _last(NULL)
{
  _maxCommands = maxCommands;
  strcpy(_delim, " "); // strtok_r needs a null-terminated string
  clearBuffer();
  //allocate memory for the command list
  _commandList = (StreamCommandCallback *) calloc(maxCommands, sizeof(StreamCommandCallback));
}
/**
 * Constructor makes sure some things are set.
 */
StreamCommand::StreamCommand(
                             byte maxCommands
                            )
  : _stream(&Serial),
    _commandList(NULL),
    _commandCount(0),
    _defaultHandler(NULL),
    _nullHandler(NULL),
    _term('\r'),           // default terminator for commands, CR character
    _last(NULL)
{
  _maxCommands = maxCommands;
  strcpy(_delim, " "); // strtok_r needs a null-terminated string
  clearBuffer();
   //allocate memory for the command list
  _commandList = (StreamCommandCallback *) calloc(maxCommands, sizeof(StreamCommandCallback));
 }

/**
 * Selects which Stream will be used (e.g.a hardware or software serial port)
 */
void StreamCommand::setStream(Stream *_stream){
  _stream = _stream;
}

  /**
 * Adds a "command" and a handler function to the list of available commands.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with it.
 */
void StreamCommand::addCommand(const char *command, void(*function)()) {
  #ifdef STREAMCOMMAND_DEBUG
    _stream->print("Adding command (");
    _stream->print(_commandCount);
    _stream->print("): ");
    _stream->println(command);
  #endif
  if (_commandCount >= _maxCommands){
    #ifdef STREAMCOMMAND_DEBUG
      _stream->println("Error: maxCommands was exceeded");
    #endif
    return;
  }
  //make a new callback
  struct StreamCommandCallback new_callback;
  new_callback.command  = command;
  new_callback.function = function;
  _commandList[_commandCount] = new_callback;
  _commandCount++;
}

/**
 * This sets up a handler to be called in the event that the received command string
 * isn't in the list of commands.
 */
void StreamCommand::setDefaultHandler(void (*function)(const char *)) {
  _defaultHandler = function;
}

/**
 * This sets up a handler to be called in the event that the user hits enter on a
 * blank line
 */
void StreamCommand::setNullHandler(void (*function)()) {
  _nullHandler = function;
}

/**
 * This checks the stream for characters, and assembles them into a buffer.
 * When the terminator character (default '\n') is seen, it starts parsing the
 * buffer for a prefix command, and calls handlers set up by addCommand() member
 */
void StreamCommand::readStream() {
  while (_stream->available() > 0) {
    char inChar = _stream->read();   // Read single available character, there may be more waiting
    #ifdef STREAMCOMMAND_DEBUG
      _stream->print(inChar);   // Echo back to stream
    #endif

    if (inChar == _term) {     // Check for the terminator (default '\r') meaning end of command
      #ifdef STREAMCOMMAND_DEBUG
        _stream->print("Received: ");
        _stream->println(_buffer);
      #endif

      char *command = strtok_r(_buffer, _delim, &_last);   // Search for command at start of buffer
      if (command != NULL) {
        boolean matched = false;
        for (int i = 0; i < _commandCount; i++) {
          #ifdef STREAMCOMMAND_DEBUG
            _stream->print("Comparing [");
            _stream->print(command);
            _stream->print("] to [");
            _stream->print(_commandList[i].command);
            _stream->println("]");
          #endif

          // Compare the found command against the list of known commands for a match
          if (strcmp(command, _commandList[i].command) == 0) {
            #ifdef STREAMCOMMAND_DEBUG
              _stream->print("Matched Command: ");
              _stream->println(command);
            #endif

            // Execute the stored handler function for the command
            (*_commandList[i].function)();
            matched = true;
            break;
          }
        }
        if (!matched && (_defaultHandler != NULL)) {
          (*_defaultHandler)(command);
        }
      } else if (_nullHandler != NULL) {
        (*_nullHandler)();
      }
      clearBuffer();
    }
    else if (isprint(inChar)) {     // Only printable characters into the buffer
      if (_bufPos < STREAMCOMMAND_BUFFER) {
        _buffer[_bufPos++] = inChar;  // Put character into buffer
        _buffer[_bufPos] = '\0';      // Null terminate
      } else {
        #ifdef STREAMCOMMAND_DEBUG
          _stream->println("Line buffer is full - increase STREAMCOMMAND_BUFFER");
        #endif
      }
    }
  }
}

/*
 * Clear the input buffer.
 */
void StreamCommand::clearBuffer() {
  _buffer[0] = '\0';
  _bufPos = 0;
}

/**
 * Retrieve the next token ("word" or "argument") from the command buffer.
 * Returns NULL if no more tokens exist.
 */
char *StreamCommand::next() {
  return strtok_r(NULL, _delim, &_last);
}

/**
  * forward all writes to the encapsulated "port" Stream object
  */
 size_t StreamCommand::write(uint8_t val) {
   return _stream->write(val);
 }
