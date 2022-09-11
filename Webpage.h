const char WEB_PAGE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      html 
      {
        font-family: Arial;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      
      .buttonPLAY 
      {
        display: inline-block;
        padding: 15px 25px;
        font-size: 24px;
        font-weight: bold;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        outline: none;
        color: #fff;
        background-color: #4CAF50;
        border: none;
        border-radius: 15px;
        box-shadow: 0 5px #999;
      }
      .buttonPLAY:hover {background-color: #3e8e41}
      .buttonPLAY:active 
      {
        background-color: #3e8e41;
        box-shadow: 0 1px #666;
        transform: translateY(4px);
      }

      .buttonPAUSE 
      {
        display: inline-block;
        padding: 15px 25px;
        font-size: 24px;
        font-weight: bold;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        outline: none;
        color: #fff;
        background-color: #f8b24f;
        border: none;
        border-radius: 15px;
        box-shadow: 0 5px #999;
      }
      .buttonPAUSE:hover {background-color: #dda049}
      .buttonPAUSE:active 
      {
        background-color: #dda049;
        box-shadow: 0 1px #666;
        transform: translateY(4px);
      }

      .buttonPREV 
      {
        display: inline-block;
        padding: 15px 25px;
        font-size: 24px;
        font-weight: bold;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        outline: none;
        color: #fff;
        background-color: #1b6ca8;
        border: none;
        border-radius: 15px;
        box-shadow: 0 5px #999;
      }
      .buttonPREV:hover {background-color: #185b8f}
      .buttonPREV:active 
      {
        background-color: #185b8f;
        box-shadow: 0 1px #666;
        transform: translateY(4px);
      }

      .buttonNEXT 
      {
        display: inline-block;
        padding: 15px 25px;
        font-size: 24px;
        font-weight: bold;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        outline: none;
        color: #fff;
        background-color: #1b6ca8;
        border: none;
        border-radius: 15px;
        box-shadow: 0 5px #999;
      }

      .buttonNEXT:hover {background-color: #185b8f}

      .buttonNEXT:active 
      {
        background-color: #185b8f;
        box-shadow: 0 1px #666;
        transform: translateY(4px);
      }

      .buttonVOLDOWN 
      {
        display: inline-block;
        padding: 15px 25px;
        font-size: 24px;
        font-weight: bold;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        outline: none;
        color: #fff;
        background-color: #0a97b0;
        border: none;
        border-radius: 15px;
        box-shadow: 0 5px #999;
      }

      .buttonVOLDOWN:hover {background-color: #0b889e}
      
      .buttonVOLDOWN:active 
      {
        background-color: #0b889e;
        box-shadow: 0 1px #666;
        transform: translateY(4px);
      }

      .buttonVOLUP 
      {
        display: inline-block;
        padding: 15px 25px;
        font-size: 24px;
        font-weight: bold;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        outline: none;
        color: #fff;
        background-color: #0a97b0;
        border: none;
        border-radius: 15px;
        box-shadow: 0 5px #999;
      }
      
      .buttonVOLUP:hover {background-color: #0b889e}
      
      .buttonVOLUP:active {
        background-color: #0b889e;
        box-shadow: 0 1px #666;
        transform: translateY(4px);
      }

      .buttonSTOP 
      {
        display: inline-block;
        padding: 15px 25px;
        font-size: 24px;
        font-weight: bold;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        outline: none;
        color: #fff;
        background-color: #e74c3c;
        border: none;
        border-radius: 15px;
        box-shadow: 0 5px #999;
      }
      .buttonSTOP:hover {background-color: #c0392b}
      .buttonSTOP:active 
      {
        background-color: #c0392b;
        box-shadow: 0 1px #666;
        transform: translateY(4px);
      }
    </style>
  </head>
  
  <body>
    <div>
      <h2>NodeMCU ESP8266 Local Web Server Controlling Serial MP3 Player</h2><br>
      <button type="button" class="buttonPLAY" onclick="sendData('play')">PLAY</button>
      <button type="button" class="buttonPAUSE" onclick="sendData('pause')">PAUSE</button>
      <button type="button" class="buttonPREV" onclick="sendData('previous')">PREV</button>
      <button type="button" class="buttonNEXT" onclick="sendData('next')">NEXT</button>
      <button type="button" class="buttonVOLDOWN" onclick="sendData('volume_down')">VOLUME -</button>
      <button type="button" class="buttonVOLUP" onclick="sendData('volume_up')">VOLUME +</button>
      <button type="button" class="buttonSTOP" onclick="sendData('stop')">STOP</button><br>
    </div>
    
    <div>
      <p style="color:#2c3e50;font-weight: bold;font-size: 24px;">MP3 Player Status is : <span id="PLAYERCMD">NA</span></p>
      <p style="color:#2c3e50;font-weight: bold;font-size: 24px;">Volume Level : <span id="volStat">NA</span></p>
    </div>

    <script>
      function sendData(CMD) 
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() 
        {
          if (this.readyState == 4 && this.status == 200) {}  
        };
        xhttp.open("GET", "setPLAYER?PLAYERCMD=" + CMD, true);
        xhttp.send(); 
      }

      setInterval(function() 
      {
        // Call a function repetatively with 0.1 (100ms) Second interval
        requestStatus();
      }, 100);

      function requestStatus() 
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() 
        {
          if (this.readyState == 4 && this.status == 200) 
          {
            var status = "";
            status = this.responseText;
            
            // Example: if the data received for display is "30PLAY", then status = "30PLAY".
            // Then 30PLAY separated into 30 and PLAY.
            
            document.getElementById("volStat").innerHTML = status.slice(0, 2); // Get the first two characters from the response
            
            document.getElementById("PLAYERCMD").innerHTML = status.slice(2, status.length); // Retrieve the state (everything that comes after!)
          }
        };
        xhttp.open("GET", "status", true);
        xhttp.send();
      }
    </script>
  </body>
</html>
)=====";