const express = require('express')
const app = express()
const server = require('http').createServer(app);
const WebSocket = require('ws');

const wss = new WebSocket.Server({ server:server });

wss.on('connection', function connection(ws) {
  console.log('A new client Connected!');
  ws.send('Welcome New Client!');
 
  ws.on('message', function incoming(message) {
    //console.log('received: %s', message); ///
    //ws.send(`Got your message its ${String(message)}`); //is this the same message from who sent it??? guess so.
    ws.send(String(message)); //is this the same message from who sent it??? guess so.
    //when I click on lightcontrol, this DOES NOT gets to the server, but it DOES GET TO THE BOARD

    //broadcast to everyone... in this case, gitthe board and the frontend. both are clients of this websocket.
    wss.clients.forEach(function each(client) {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        console.log('inside broadcast %s', message) // this is from all other senders??? ONLY PRINTS WHEN MORE THAN 1 IS CONNECTED..
        //when I click on lightcontrol, this message DOES GET TO SERVER, but it DOES NOT GET TO THE BOARD

        client.send(String(message));
        // try{
        //   //const sensorData = JSON.parse(message)
        //   //console.log(sensorData.temperature, sensorData.airHumidity)
        //   client.send(String(message));
        // }catch(e){
        //   console.log(e)
        // }
        //const sensorData = JSON.parse(message)
        //console.log(sensorData.temperature, sensorData.airHumidity)
        //client.send(String(message));
      }
    });
    
  });

});

app.get('/', (req, res) => res.send('Hello World!'))

server.listen(3000, () => console.log(`Lisening on port :3000`))