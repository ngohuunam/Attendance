// import { SerialPort } from "serialport";
import { SerialPortStream } from "@serialport/stream";
import { ReadlineParser } from "@serialport/parser-readline";

import { MockBinding } from "@serialport/binding-mock";

MockBinding.createPort("/dev/ROBOT", { echo: true, record: true });

const port = new SerialPortStream({ binding: MockBinding, path: "/dev/ROBOT", baudRate: 14400 });

const parser = port.pipe(new ReadlineParser());

// const port = new SerialPort({ path: "/dev/ROBOT", baudRate: 9600 }, function (err) {
//   if (err) {
//     return console.log("Error: ", err.message);
//   }
// });

import { stringSentenceCase } from "./tools.js";

// import initBot from "./bot.js";

// const bot = initBot();

import { initDB, pushDB, pushDBAndWrite } from "./db.js";

initDB();

const connectLostCount = { host: 0, reader_1: 0, reader_2: 0, reader_3: 0, reader_4: 0 };
const connectLostTimestamp = { host: 0, reader_1: 0, reader_2: 0, reader_3: 0, reader_4: 0 };

const pingCount = { host: 0, reader_1: 0, reader_2: 0, reader_3: 0, reader_4: 0 };

parser.on("data", function (data) {
  const dataArr = data.split(",");
  const _device = dataArr[0];
  const _id = dataArr[1];
  const _cmd = dataArr[2];
  const _value = dataArr[3];
  const _time = new Date().toLocaleTimeString("vi");
  const _savedata = `${_time},${data}`;
  let _needWriteDB = false;
  let _db;

  const _prop = _device === "h" ? "host" : `reader_${_id}`;
  connectLostCount[_prop] = 0;
  connectLostTimestamp[_prop] = 0;

  if (_cmd === "p") {
    pingCount[_prop]++;
    const _res = `${_device},${_id},p,${pingCount[_prop]}`;
    return port.write(_res);
  }

  const _botMess = _device === "h" ? "Host" : `Reader ${_id}`;

  _db = pushDB("raw", _prop, _savedata, _botMess);
  _needWriteDB = !!_db && true;

  if (_cmd === "fe") {
    const _fingerID = _value ? parseInt(_value) : 199;
    const _res = `r,${_cmd},fe,${_fingerID}}`;
    port.write(_res);
  }

  if (_needWriteDB) {
    _db.write();
  }
});

port.write("turn on", function (err) {
  if (err) {
    return console.log("Error on write: ", err.message);
  }
  console.log("message written");
});

// Open errors will be emitted as an error event
port.on("error", function (err) {
  const _savedata = `Port error: ${err.message} - ${new Date().toLocaleTimeString("vi")}`;
  console.error(_savedata);
  pushDBAndWrite("raw", "rasp", _savedata);
});

const checkConnect = () => {
  let _needWriteDB = false;
  let _db;
  Object.keys(connectLostCount).map((key) => {
    console.log(`${key}: ${connectLostCount[key]}`);
    connectLostCount[key]++;
    if (connectLostCount[key] > 3 && connectLostTimestamp[key] === 0) {
      const timestamp = Date.now();
      connectLostTimestamp[key] = timestamp;
      const _savedata = `Connect timeout: ${new Date(timestamp).toLocaleTimeString("vi")}`;
      const _botMess = stringSentenceCase(key).replace("_", " ");
      _db = pushDB("raw", key, _savedata, _botMess);
      _needWriteDB = !!_db && true;
    }
  });
  if (_needWriteDB) {
    console.log("~ connectLostTimestamp:", connectLostTimestamp);
    _db.write();
  }
};

const raspRunning = () => {
  const _savedata = `Running: ${new Date().toLocaleTimeString("vi")}`;
  pushDBAndWrite("raw", key, _savedata);
};

const checkConnectInterval_ms = 1 * 10 * 1000;

const raspRunningInterval_ms = 1 * 60 * 60 * 1000;

let checkConnectInterval = setInterval(checkConnect, checkConnectInterval_ms);

let raspRunningInterval = setInterval(raspRunning, raspRunningInterval_ms);

