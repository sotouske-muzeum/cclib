
const WebSocket = require('ws');

const ws = new WebSocket('ws://127.0.0.1:8001/','dumb-protocol');

ws.on('open', function open() {
  ws.send('Hello from node.js client');
});

ws.on('message', function incoming(data, flags) {
  // flags.binary will be set if a binary data is received.
  console.log('received: ' + data);

  if (data.indexOf('Hello from node.js client') != -1)
  {
    ws.close();
  }
});

