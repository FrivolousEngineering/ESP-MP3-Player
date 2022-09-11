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
      button
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
        border: none;
        border-radius: 15px;
        border: none;
        border-radius: 15px;
        box-shadow: 0 5px #999;
      }
      
      button:active
      {
      	box-shadow: 0 1px #666;
        transform: translateY(4px);
      }
      
      .buttonPLAY { background-color: #4CAF50 }
      .buttonPLAY:hover { background-color: #3e8e41 }

      .buttonPAUSE { background-color: #f8b24f }
      .buttonPAUSE:hover {background-color: #dda049}

      .buttonPREV { background-color: #1b6ca8 }
      .buttonPREV:hover { background-color: #185b8f }

      .buttonNEXT { background-color: #1b6ca8 }
      .buttonNEXT:hover { background-color: #185b8f }

      .buttonVolume { background-color: #0a97b0; }
      .buttonVOLDOWN:hover {background-color: #0b889e}
      

      .buttonSTOP { background-color: #e74c3c }
      .buttonSTOP:hover { background-color: #c0392b }

    </style>
  </head>
  
  <body>
    <div>
      <button type="button" class="buttonPLAY" onclick="sendData('play')">PLAY</button>
      <button type="button" class="buttonPAUSE" onclick="sendData('pause')">PAUSE</button>
      <button type="button" class="buttonPREV" onclick="sendData('previous')">PREV</button>
      <button type="button" class="buttonNEXT" onclick="sendData('next')">NEXT</button>
      <button type="button" class="buttonVolume" onclick="sendData('volume_down')">VOLUME -</button>
      <button type="button" class="buttonVolume" onclick="sendData('volume_up')">VOLUME +</button>
      <button type="button" class="buttonSTOP" onclick="sendData('stop')">STOP</button><br>
    </div>
    
    <div>
      <p style="color:#2c3e50;font-weight: bold;font-size: 20px;">Status: <span id="PLAYERCMD">NA</span></p>
      <p style="color:#2c3e50;font-weight: bold;font-size: 20px;">Volume: <span id="volStat">NA</span></p>
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