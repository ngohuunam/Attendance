const aedes = require("aedes")();
const { createServer } = require("aedes-server-factory");
const port = 1883;

const server = createServer(aedes);

server.listen(port, function () {
  console.log("server started and listening on port ", port);
});
