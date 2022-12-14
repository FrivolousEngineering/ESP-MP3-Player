#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <SoftwareSerial.h>
#include <array>

#include "Webpage.h" //-> Include the contents of the User Interface Web page, stored in the same folder as the .ino file
#include "MP3Commands.h"

const char* ssid = "grove";
const char* password = "";

const byte DNS_PORT = 53;
IPAddress APIP(172, 0, 0, 1); // Gateway

DNSServer dnsServer;
ESP8266WebServer server(80); 
SoftwareSerial mp3(5,4); // MP3 Player

static int8_t send_buffer[8] = {0}; //-> Buffer to send commands. 
static uint8_t answer_buffer[10] = {0}; //-> Buffer to receive response.  

int volume = 30; 

String player_status = "stop"; //-> //-> Variable string for all playback states displayed on the web page and monitor serial.
String command_last_received = "stop"; //-> Variable string for playback status "PLAY" and "STOP" displayed on the web page.

unsigned long previous_mp3_time = 0; //-> save the last time the data was received from the serial mp3 module.
const long mp3_update_interval = 500; //-> interval for updating data received from serial mp3 player.

unsigned long previous_feedback_time = 0; // Time (in MS) that the last feedback was sent to the webpage
const long feedback_update_interval = 500; //-> interval to display the last command in ms

bool led_light_on = false;

bool should_request_mp3_feedback = true; 

bool show_player_status = false;
int show_status_counter = 0;
int num_intervals_to_show_status = 3; // Show the command that was given for this amount of intervals (so this * feedback_update_interval).

int current_song_index = 1;
int num_total_songs = 0;

bool play_song_on_repeat = false;

std::array<String, 5> track_names = {"Song One", "Song Two", "Song Three", "Song Four", "Song Five"};

void setSongIndex(int song_index)
{
  current_song_index = song_index;
  if(current_song_index > num_total_songs)
  {
    current_song_index = 1;
  } else if(current_song_index < 1)
  {
    current_song_index = num_total_songs;
  }  
}

void setVolume(int _volume)
{
  volume = _volume;
  if (volume > 30)
  { 
    volume = 30;
  }
  if(volume < 0)
  {
    volume = 0;
  }
}

void handleRoot() 
{
  server.send(200, "text/html", WEB_PAGE); //-> Send web page
}

String getAllTracksAsString()
{
  String result = "";
  for (String name: track_names)
  {
    result += name + ",";
  }
  // Remove the last ','
  return result.substring(0, result.length() - 1);
}

void handleAllTrackNames()
{
  server.send(200, "text/plane", getAllTracksAsString());
}

void handleCommand() 
{
  String command = server.arg("cmd"); //-> Variable string to hold commands from web page.
  Serial.print("Received command: ");
  Serial.println(command);

  String extra_serial_output = "";

  show_player_status = true;
  if(command == "play")  
  {
    if(player_status == "pause")
    {
      sendCommand(CMD_PLAY);
    } else 
    {
      if(play_song_on_repeat)
      {
        sendCommand(CMD_SNG_CYCL_PLAY, 0, current_song_index);
        extra_serial_output += "Cycled, Song index = " + String(current_song_index);
      } else
      {
        sendCommand(CMD_PLAY_W_INDEX, 0, current_song_index);
        extra_serial_output += "Once, Song index = " + String(current_song_index);
      }
    }
    command_last_received = command;
    show_player_status = false;
    player_status = "play"; 
  }
  else if(command == "pause") 
  {
    sendCommand(CMD_PAUSE);
    command_last_received = command;
    player_status = "pause"; 
  }
  else if(command == "previous") 
  {
    setSongIndex(current_song_index - 1);
    player_status = "previous"; 
  }
  else if(command == "next") 
  {
    setSongIndex(current_song_index + 1);
    player_status = "next";
  }
  else if(command == "volume_down") 
  {
    setVolume(volume - 2);
    sendCommand(CMD_SET_VOLUME, 0, volume);
    player_status = "volume down";
    extra_serial_output += "= " + String(volume);
  }
  else if(command == "volume_up") 
  {
    setVolume(volume + 2);
    
    sendCommand(CMD_SET_VOLUME, 0, volume);
    player_status = "volume up";
    extra_serial_output += "= " + String(volume);
  }
  else if(command == "stop") 
  {
    sendCommand(CMD_STOP_PLAY);
    player_status = "stop";
    command_last_received = command;
  } else
  {
    command_last_received = "UNKNOWN";
    show_player_status = false;
  }
  Serial.print("Final command was: ");
  Serial.print(player_status);
  Serial.print(" ");
  Serial.println(extra_serial_output);
  
  server.send(200, "text/plane", "");
}

void handleSetTrack()
{
  server.send(200, "text/plane", "");
  String track_number = server.arg("t"); 
  Serial.print("Attempting to set index to:");
  Serial.println(track_number.toInt());
  setSongIndex(track_number.toInt());
}


void handleStatusRequest() 
{
  String  player_status_feedback = "";
  String volume_string = String(volume);

  unsigned long current_time = millis();
  if (current_time - previous_feedback_time >= feedback_update_interval) 
  {
    // Blink the led
    led_light_on = !led_light_on;
    
    if(led_light_on)
    {
      digitalWrite(LED_BUILTIN, LOW); // Turn the led on
    } else
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }

    previous_feedback_time = current_time;

    if (show_player_status == true) 
    {
      show_status_counter++;
      if (show_status_counter > num_intervals_to_show_status) 
      {
        show_player_status = false;
        show_status_counter = 0;
      }
    }
  }

  if (show_player_status == true) 
  {
    player_status_feedback = player_status;
  }
  else 
  {
    player_status_feedback = command_last_received;
  }

  server.send(200, "text/plane", volume_string + "," + player_status_feedback + "," + getTrackName(current_song_index)); 
}

String getTrackName(int index)
{
  int index_to_use = index - 1;
  if(index_to_use < 0) index_to_use = 0;
  if(index_to_use > track_names.size() -1) index_to_use = track_names.size() - 1;
  return track_names[index_to_use];
}

void setup()
{
  Serial.begin(9600);
  mp3.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT); // Register built in led

  Serial.println("Begin");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);  // Start the soft Acces Point
  dnsServer.start(DNS_PORT, "*", APIP); // DNS spoofing (Only for HTTP)
  
  IPAddress our_ip = WiFi.softAPIP(); 
  Serial.print("Connect your wifi laptop/mobile phone to this NodeMCU Access Point : ");
  Serial.println(ssid);
  Serial.print("Our IP is: ");
  Serial.println(our_ip); 
  
  // Setup callbacks for the various endpoints
  server.on("/", handleRoot); 
  server.on("/command", handleCommand);
  server.on("/track", handleSetTrack);  
  server.on("/status", handleStatusRequest); 
  server.on("/allTrackNames", handleAllTrackNames);
  server.onNotFound([]() { handleRoot(); }); // This is what triggers the captive portal. 
  server.begin();
  
  Serial.println("HTTP server started\n");
  
  Serial.println("Select storage device to TF card.");
  sendCommand(CMD_SEL_DEV, 0, DEV_TF);
  delay(250); 

  Serial.println("Adjust volume = 30 (highest).");
  sendCommand(CMD_SET_VOLUME, 0, volume);
  delay(250);

  Serial.println("Requesting num of tracks available");
  sendCommand(CMD_QUERY_TOT_TRACKS);

  should_request_mp3_feedback = false;
}

void loop()
{
  dnsServer.processNextRequest();
  server.handleClient();  

  unsigned long current_time = millis();
  if (current_time - previous_mp3_time >= mp3_update_interval) 
  {
    previous_mp3_time = current_time;

    // Check response.
    if (mp3.available())
    {
      Serial.println(decodeMP3Answer());
    }
  }
}

// The meaning of the mp3 player module response
String decodeMP3Answer() 
{
  String decodedMP3Answer = "";

  decodedMP3Answer += sanswer();

  switch (answer_buffer[3]) 
  {
    case 0x3A:
      decodedMP3Answer += " -> Memory card inserted.";
      break;

    case 0x3D:
      decodedMP3Answer += " -> Completed play num " + String(answer_buffer[6], DEC);
      break;

    case 0x40:
      decodedMP3Answer += " -> Error";
      show_player_status = true;
      command_last_received = "error";
      break;

    case 0x41:
      decodedMP3Answer += " -> Data recived correctly.";
      break;

    case 0x42:
      decodedMP3Answer += " -> Status playing: " + String(answer_buffer[6], DEC);
      break;

    case 0x48:
      decodedMP3Answer += " -> File count: " + String(answer_buffer[6], DEC);
      num_total_songs = (int) answer_buffer[6];
      break;

    case 0x4C:
      decodedMP3Answer += " -> Playing: " + String(answer_buffer[6], DEC);
      break;

    case 0x4E:
      decodedMP3Answer += " -> Folder file count: " + String(answer_buffer[6], DEC);
      break;

    case 0x4F:
      decodedMP3Answer += " -> Folder count: " + String(answer_buffer[6], DEC);
      break;
  }

  if (answer_buffer[3] == 0x3D) 
  { 
    player_status = "stop"; 
    command_last_received = player_status;
    Serial.println("MP3 player told us that STOP happend");
  }

  return decodedMP3Answer;
}

// Function for sending commands.
void sendCommand(byte command)
{
  sendCommand(command, 0, 0);
}

void sendCommand(byte command, byte dat1, byte dat2)
{
  delay(20);
  send_buffer[0] = 0x7E;    //-> Every command should start with $(0x7E)
  send_buffer[1] = 0xFF;    //-> Version information
  send_buffer[2] = 0x06;    //-> The number of bytes of the command without starting byte and ending byte
  send_buffer[3] = command; //-> Such as PLAY and PAUSE and so on
  
  // 0x01 = feedback && 0x00 -> No feedback
  if (should_request_mp3_feedback == true) 
  {
    send_buffer[4] = 0x01;
  }
  else 
  {
    send_buffer[4] = 0x00;    
  }

  send_buffer[5] = dat1;
  send_buffer[6] = dat2;
  send_buffer[7] = 0xEF;    //-> Ending byte of the command
  
  Serial.print("Sending: ");
  
  for (uint8_t i = 0; i < 8; i++)
  {
    mp3.write(send_buffer[i]) ;
    Serial.print(sbyte2hex(send_buffer[i]));
  }
  Serial.println();
}

// Functions for conversion
// For byte to hex conversion.
String sbyte2hex(uint8_t b)
{
  String shex;

  shex = "0X";

  if (b < 16) shex += "0";
  shex += String(b, HEX);
  shex += " ";
  return shex;
}

// For hex to integer conversion.
int shex2int(char *s, int n)
{
  int r = 0;
  for (int i = 0; i < n; i++)
  {
    if(s[i] >= '0' && s[i] <= '9')
    {
      r *= 16; 
      r +=s[i]-'0';
    }
    else if(s[i] >= 'A' && s[i] <= 'F')
    {
      r *= 16;
      r += (s[i] - 'A') + 10;
    }
  }
  return r;
}

// Function: sanswer. Returns a String answer from mp3 UART module.          
// Parameter:- uint8_t b. void.                                                  
// Return: String. If the answer is well formated answer.                        
String sanswer(void)
{
  uint8_t i = 0;
  String mp3answer = ""; 

  // Get only 10 Bytes
  while (mp3.available() && (i < 10))
  {
    uint8_t b = mp3.read();
    answer_buffer[i] = b;
    i++;
    mp3answer += sbyte2hex(b);
  }

  // if the answer format is correct.
  if (answer_buffer[0] == 0x7E && answer_buffer[9] == 0xEF)
  {
    return mp3answer;
  }

  return "???: " + mp3answer;
}
