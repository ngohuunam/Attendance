import createError from "http-errors";
import express from "express";
import { join, dirname } from "node:path";
import { fileURLToPath } from "node:url";
import cookieParser from "cookie-parser";
import logger from "morgan";

// import { CronJob } from "cron";

// const job = new CronJob("00 27 17 * * *", function () {
//   const d = new Date();
//   console.log("Midnight:", d);
// });

// job.start();

const __filename = fileURLToPath(import.meta.url);

const __dirname = dirname(__filename);

import indexRouter from "./routes/index.js";
// const usersRouter = require('./routes/users');

const app = express();

// view engine setup
app.set("views", join(__dirname, "views"));
app.set("view engine", "jade");

app.use(logger("dev"));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(join(__dirname, "public")));

app.use("/", indexRouter);
// app.use('/users', usersRouter);

// catch 404 and forward to error handler
app.use(function (req, res, next) {
  next(createError(404));
});

// error handler
app.use(function (err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get("env") === "development" ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render("error");
});

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

// import { runAtSpecificTimeOfDay } from "./tools.js";

// import initBot from "./bot.js";

// const bot = initBot();

import { initDB, pushDB, pushDBAndWrite, initDBDevice } from "./db.js";

initDB();

const devices = initDBDevice().data;

const connectLostTimestamp = devices.reduce((p, c) => ({ ...p, ...{ [c.propName]: 999 } }), {});

const connectLostCount = devices.reduce((p, c) => ({ ...p, ...{ [c.propName]: 0 } }), {});

const pingCount = { ...connectLostCount };

parser.on("data", function (data) {
  const dataArr = data.split(",");
  const _d = dataArr[0];
  const _i = dataArr[1];

  const device = devices.find((dv) => dv.d === _d && dv.i === _i);

  if (!device) return;

  const _cmd = dataArr[2];
  const _prop = device.propName;
  const _time = new Date().toLocaleTimeString("vi");

  let _db;

  if (connectLostTimestamp[_prop] > 0) {
    _db = pushDB("raw", _prop, `Connected: ${_time}`, device.displayName);
    connectLostTimestamp[_prop] = 0;
  }

  connectLostCount[_prop] = 0;

  if (_cmd === "p") {
    pingCount[_prop]++;
    const _res = `${_d},${_i},p,${pingCount[_prop]}`;
    return port.write(_res);
  }

  const _value = dataArr[3];
  let _savedata = `${_time},${data}`;
  let _needWriteDB = false;

  _db = pushDB("raw", _prop, _savedata, device.displayName);
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
    if (connectLostCount[key] > 3 && (connectLostTimestamp[key] === 0 || connectLostTimestamp[key] === 999)) {
      const timestamp = Date.now();
      connectLostTimestamp[key] = timestamp;
      const _device = devices.find((dv) => dv.propName === key);
      const _savedata = `Connect timeout: ${new Date(timestamp).toLocaleTimeString("vi")}`;
      _db = pushDB("raw", key, _savedata, _device.displayName);
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

const restartCheckInterval = () => {
  clearInterval(checkConnectInterval);
  checkConnectInterval = setInterval(checkConnect, checkConnectInterval_ms);
  clearInterval(raspRunningInterval);
  raspRunningInterval = setInterval(raspRunning, raspRunningInterval_ms);
};

import { runAtSpecificTimeOfDay } from "./tools.js";

let timerID = { out: 0, interval: 0 };

runAtSpecificTimeOfDay(
  0,
  2,
  function () {
    restartCheckInterval();
  },
  timerID
);

pushDBAndWrite("raw", "rasp", "Startup: " + new Date().toLocaleTimeString("vi"));

export default app;
