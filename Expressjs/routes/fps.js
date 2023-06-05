var express = require('express');
var router = express.Router();

var mqttHandler = require('../tools/mqtt');

var mqttClient = new mqttHandler();
mqttClient.connect();

/* GET home page. */
router.get('/search', function(req, res, next) {
  mqttClient.sendMessage("/fps/search", "search");
  res.status(200).send("Message sent to mqtt");
});

module.exports = router;
