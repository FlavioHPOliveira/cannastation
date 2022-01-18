const express = require('express')
const app = express()
const server = require('http').createServer(app);
const WebSocket = require('ws');

const wss = new WebSocket.Server({ server:server });

//param format: /?token=123?clientType=board

const getTokenFromURLREQ = (param) =>{
  try{
    param = param.replace('/','')
    const words = param.split('?')
    //after split: [ '', 'token=123', 'clientType=board' ]
    //console.log(words)
    const token = words[1].split('=')[1]
    const clientType = words[2].split('=')[1]
    return { 
      token: token, 
      clientType: clientType
    }
  }catch(e){
    console.log('Err parsing URLREQ', e)
  }
  
}

wss.on('connection', function connection(ws, req) {

  console.log('A new client Connected!');
  console.log(req.url);
  const tokenAndClientType = getTokenFromURLREQ(req.url);
  //console.log(token)
  ws.send(`Welcome New Client, url sent ${req.url}`);

  ws.token = tokenAndClientType.token
  ws.clientType = tokenAndClientType.clientType
 
  ws.on('message', function incoming(message) {
    //console.log('received: %s', message); ///
    //ws.send(`Got your message its ${String(message)}`); //Sent confirmation message to whomever sent it.
    ws.send(String(message)); 

    wss.clients.forEach(function each(client) {
      //if (client !== ws && client.readyState === WebSocket.OPEN) {
      if (client.token == ws.token && client.readyState === WebSocket.OPEN) {
        //console.log('inside broadcast %s', message) // this is from all other senders??? ONLY PRINTS WHEN MORE THAN 1 IS CONNECTED..
        client.send(String(message));

      }
    });
    
  });

});

app.get('/', (req, res) => res.send('Hello World!'))

server.listen(3000, () => console.log(`Lisening on port :3000`))