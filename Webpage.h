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
      <button type="button" class="buttonPLAY" onclick="sendCommand('play')">PLAY</button>
      <button type="button" class="buttonPAUSE" onclick="sendCommand('pause')">PAUSE</button>
      <button type="button" class="buttonPREV" onclick="sendCommand('previous')">PREV</button>
      <button type="button" class="buttonNEXT" onclick="sendCommand('next')">NEXT</button>
      <button type="button" class="buttonVolume" onclick="sendCommand('volume_down')">VOLUME -</button>
      <button type="button" class="buttonVolume" onclick="sendCommand('volume_up')">VOLUME +</button>
      <button type="button" class="buttonSTOP" onclick="sendCommand('stop')">STOP</button><br>
      <br>
      <form action="/track" id = "track_selector_form">
        <label for="cars">Select a track:</label>
        <select name="track_selector" id="track_selector"> </select>
        <br><br>
        <input type="submit" value="Submit">
      </form>
      
    </div>
    
    <div>
      <p style="color:#2c3e50;font-weight: bold;font-size: 20px;">Status: <span id="command_display">NA</span></p>
      <p style="color:#2c3e50;font-weight: bold;font-size: 20px;">Volume: <span id="volume_display">NA</span></p>
      <p style="color:#2c3e50;font-weight: bold;font-size: 20px;">Current track: <span id="track_name">NA</span></p>
    </div>

    <script>
      const form  = document.getElementById('track_selector_form');

      form.addEventListener('submit', (event) => {
          // handle the form data
          event.preventDefault();
          var xhttp = new XMLHttpRequest();
          xhttp.onreadystatechange = function() 
          {
            if (this.readyState == 4 && this.status == 200) {}  
          };
          var track_selector = document.getElementById("track_selector")
          xhttp.open("GET", "track?t=" + track_selector.value, true);
          xhttp.send();
      });


      function sendCommand(CMD) 
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() 
        {
          if (this.readyState == 4 && this.status == 200) {}  
        };
        xhttp.open("GET", "command?cmd=" + CMD, true);
        xhttp.send(); 
      }

      setInterval(function() 
      {
        // Call a function repetatively with 0.1 (100ms) Second interval
        requestStatus();
      }, 100);

      function requestTrackNames()
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() 
        {
          if (this.readyState == 4 && this.status == 200) 
          {
            var status = "";
            status = this.responseText;
            var splitted = status.split(",");

            var track_selector = document.getElementById("track_selector")
            for (let index = 0; index < splitted.length; ++index)
            {
              var option = document.createElement("option")
              option.value = index + 1;
              option.innerHTML = splitted[index];
              track_selector.appendChild(option);
            }
          }
        }
        xhttp.open("GET", "allTrackNames", true);
        xhttp.send();
      }

      function requestStatus() 
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() 
        {
          if (this.readyState == 4 && this.status == 200) 
          {
            var status = "";
            status = this.responseText;

            var splitted = status.split(",");
            
            document.getElementById("volume_display").innerHTML = splitted[0]
            document.getElementById("command_display").innerHTML = splitted[1]
            document.getElementById("track_name").innerHTML = splitted[2]
            
          }
        };
        xhttp.open("GET", "status", true);
        xhttp.send();
      }

      requestTrackNames();
    </script>
  </body>
</html>
)=====";