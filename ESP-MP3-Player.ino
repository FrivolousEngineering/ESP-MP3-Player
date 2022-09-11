#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

#include "Webpage.h" //-> Include the contents of the User Interface Web page, stored in the same folder as the .ino file
#include "MP3Commands.h"

const char* ssid = "grove";
const char* password = "";

ESP8266WebServer server(80); 
SoftwareSerial mp3(5,4); // MP3 Player

static int8_t send_buffer[8] = {0}; //-> Buffer to send commands. 
static uint8_t answer_buffer[10] = {0}; //-> Buffer to receive response.  

int vol = 30; 

int cnt_3d = 0; //-> Variable as an indicator to continue to the next playback after the previous playback is finished.

String PLAYERStatus = "STOP"; //-> //-> Variable string for all playback states displayed on the web page and monitor serial.
String PlayerStatSend = "STOP"; //-> Variable string for playback status "PLAY" and "STOP" displayed on the web page.

unsigned long previousMillisCR = 0; //-> save the last time the data was received from the serial mp3 module.
const long intervalCR = 500; //-> interval for updating data received from serial mp3 player.

unsigned long previous_feedback_time = 0; // Time (in MS) that the last feedback was sent to the webpage
const long feedback_interval = 500; //-> interval to display the last command in ms


bool FFB = true; // Variable as an indicator to get feedback from the Serial MP3 Module.

bool SFB = false; //-> The bool variable for creating conditions displays the PlayerStatSend and PLAYERStatus variables on a web page.
int intv = 0; //-> The variable for the interval displays the PLAYERStatus.

void handleRoot() 
{
  server.send(200, "text/html", MAIN_page); //-> Send web page
}

void handlePLAYERCMD() 
{
  String command = server.arg("PLAYERCMD"); //-> Variable string to hold commands from web page.
  Serial.println();
  Serial.println(command);
  if(command == "play")  
  {
    sendCommand(CMD_PLAY);
    PlayerStatSend = "PLAY";
    PLAYERStatus = "PLAY"; 
    Serial.println(PLAYERStatus);
    Serial.println();
  }
  else if(command == "pause") 
  {
    sendCommand(CMD_PAUSE);
    SFB = true;
    PlayerStatSend = "PAUSE";
    PLAYERStatus = "PAUSE"; 
    Serial.println(PLAYERStatus);
    Serial.println();
  }
  else if(command == "previous") 
  {
    sendCommand(CMD_PREV_SONG);
    SFB = true;
    PlayerStatSend = "PLAY";
    PLAYERStatus = "PREV"; 
    Serial.println(PLAYERStatus);
    Serial.println();
  }
  else if(command == "next") 
  {
    sendCommand(CMD_NEXT_SONG);
    SFB = true;
    PlayerStatSend = "PLAY";
    PLAYERStatus = "NEXT";
    Serial.println(PLAYERStatus);
    Serial.println();
  }
  else if(command == "volume_down") 
  {
    vol = vol - 2;
    if (vol < 0) 
    {
      vol = 0;
    }
    sendCommand(CMD_SET_VOLUME, 0, vol);
    SFB = true;
    PLAYERStatus = "VOLUME DOWN";
    Serial.print(PLAYERStatus);
    Serial.print(" = ");
    Serial.println(vol);
    Serial.println();
  }
  else if(command == "volume_up") 
  {
    vol=vol + 2;
    if (vol > 30)
    { 
      vol = 30;
    }
    sendCommand(CMD_SET_VOLUME, 0, vol);
    SFB = true;
    PLAYERStatus = "VOLUME UP";
    Serial.print(PLAYERStatus);
    Serial.print(" = ");
    Serial.println(vol);
    Serial.println();
  }
  else if(command == "stop") 
  {
    sendCommand(CMD_STOP_PLAY);
    SFB = true;
    PlayerStatSend = "STOP";
    PLAYERStatus = "STOP";
    Serial.println(PLAYERStatus);
    Serial.println();
  }

  server.send(200, "text/plane", "");
}


void handleFB() 
{
  String  player_status_feedback = "";
  String volume_string;
  if (vol < 10) 
  {
    volume_string = "0" + String(vol);
  }
  else 
  {
    volume_string = String(vol);
  }

  unsigned long current_time = millis();
  if (current_time - previous_feedback_time >= feedback_interval) 
  {
    // save the last time you blinked the LED
    previous_feedback_time = current_time;

    if (SFB == true) 
    {
      intv++;
      if (intv > 3) 
      {
        SFB = false;
        intv = 0;
      }
    }
  }

  if (SFB == true) 
  {
    player_status_feedback = PLAYERStatus;
  }
  else 
  {
    player_status_feedback = PlayerStatSend;
  }

  server.send(200, "text/plane", volume_string + player_status_feedback); 
}

void setup()
{
  Serial.begin(9600);
  mp3.begin(9600);
  delay(250);
  
  Serial.println("Begin");
  delay(250);
  
  WiFi.softAP(ssid, password);  // Start the soft Acces Point

  IPAddress our_ip = WiFi.softAPIP(); 
  Serial.print("Connect your wifi laptop/mobile phone to this NodeMCU Access Point : ");
  Serial.println(ssid);
  Serial.print("Our IP is: ");
  Serial.println(our_ip); 
  
 
  // Setup callbacks for the various endpoints
  server.on("/", handleRoot); //--> Routine to handle at root location. This is to display web page.
  server.on("/setPLAYER", handlePLAYERCMD);  //--> Routine to handle the call procedure handlePLAYERCMD.
  server.on("/FBStat", handleFB);  //--> Routine to handle the call procedure handleFB.
  
  server.begin(); //--> Start server
  Serial.println("HTTP server started\n");
  delay(250);
  
  Serial.println("Select storage device to TF card.");
  sendCommand(CMD_SEL_DEV, 0, DEV_TF);
  delay(250); 

  Serial.println("Adjust volume = 30 (highest).");
  sendCommand(CMD_SET_VOLUME, 0, vol);
  delay(250);

  FFB = false;
}


void loop()
{
  server.handleClient();  

  unsigned long currentMillisCR = millis();
  if (currentMillisCR - previousMillisCR >= intervalCR) 
  {
    // save the last time the data was received from the serial mp3 module.
    previousMillisCR = currentMillisCR;

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
      break;

    case 0x41:
      decodedMP3Answer += " -> Data recived correctly. ";
      break;

    case 0x42:
      decodedMP3Answer += " -> Status playing: " + String(answer_buffer[6], DEC);
      break;

    case 0x48:
      decodedMP3Answer += " -> File count: " + String(answer_buffer[6], DEC);
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
    cnt_3d++;
    if (cnt_3d == 2) 
    {
      Serial.println();
      PLAYERStatus = "STOP"; 
      PlayerStatSend = PLAYERStatus;
      Serial.println(PLAYERStatus);
      Serial.println();
      cnt_3d = 0;
    }
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
  if (FFB == true) 
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
