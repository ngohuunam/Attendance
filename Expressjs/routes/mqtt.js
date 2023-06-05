var express = require('express');
var router = express.Router();

var mqttHandler = require('../tools/mqtt');

var mqttClient = new mqttHandler();
mqttClient.connect();

/* POST mqtt message. */
router.post('/', function(req, res, next) {
  const mess = req.body.message
  console.log("🚀 ~ file: mqtt.js:12 ~ router.post ~ mess:", mess)
  
  mqttClient.sendMessage("/feeds/onoff", mess);
  res.status(200).send("Message sent to mqtt");
});

module.exports = router;
