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

// const { raw: db.raw, finger: fingerdb, qr: qrdb } = db;

const port = new SerialPortStream({ binding: MockBinding, path: "/dev/ROBOT", baudRate: 14400 });

const parser = port.pipe(new ReadlineParser());

// const port = new SerialPort({ path: "/dev/ROBOT", baudRate: 9600 }, function (err) {
//   if (err) {
//     return console.log("Error: ", err.message);
//   }
// });

import { createDB } from "./db.js";

import { runAtSpecificTimeOfDay } from "./tools.js";

let db = { raw: "", finger: "", qr: "" };

const initDB = () => {
  db = createDB();
  db.raw.read();
  db.finger.read();
  db.qr.read();
};

initDB();

runAtSpecificTimeOfDay(0, 0, initDB);

const connectLostCount = { host: 0, reader_1: 0, reader_2: 0, reader_3: 0, reader_4: 0 };
const connectLostTimestamp = { host: 0, reader_1: 0, reader_2: 0, reader_3: 0, reader_4: 0 };

setInterval(function () {
  for (const [key, value] of Object.entries(connectLostCount)) {
    console.log(`${key}: ${value}`);
    connectLostCount[key]++;
    if (connectLostCount[key] > 3 && connectLostTimestamp[key] === 0) {
      connectLostTimestamp[key] = Date.now();
      db.raw.data[key].push("ping lost: " + connectLostTimestamp[key]);
      db.raw.write();
    }
  }
  console.log("~ connectLostTimestamp:", connectLostTimestamp);
}, 5 * 60 * 1000);

const pingCount = { host: 0, reader_1: 0, reader_2: 0, reader_3: 0, reader_4: 0 };

parser.on("data", function (data) {
  const dataArr = data.split(",");
  const _device = dataArr[0];
  const _cmd = dataArr[1];
  const _value = dataArr[2];
  const _time = new Date().toLocaleTimeString("vi");
  const _savedata = `${_time},${data}`;
  let _needWriteDB = false;
  if (_device === "h") {
    if (_cmd === "p") {
      connectLostCount.host = 0;
      connectLostTimestamp.host = 0;
      pingCount.host++;
      const _res = `h,p,${pingCount.host}`;
      port.write(_res);
    } else {
      db.raw.data.host.push(_savedata);
      _needWriteDB = true;
    }
  } else if (_device === "r") {
    const _desPath = "reader_" + _cmd;
    connectLostCount[_desPath] = 0;
    connectLostTimestamp[_desPath] = 0;
    if (_value === "p") {
      pingCount[_desPath]++;
      const _res = `r,${_cmd},p,${pingCount[_desPath]}`;
      port.write(_res);
    } else {
      if (_value === "enroll") {
        const _fingerID = 199;
        const _res = `r,${_cmd},enroll,${_fingerID}}`;
        port.write(_res);
      }
      db.raw.data[_desPath].push(_savedata);
      _needWriteDB = true;
    }
  } else {
    console.log(data);
  }

  if (_needWriteDB) {
    db.raw.write();
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
  console.log("Error: ", err.message);
  db.raw.data.rasp.push("Port error: ", err.message + " - " + new Date().toLocaleTimeString("vi"));
  db.raw.write();
  console.log("Running...");
});

db.raw.data.rasp.push("Startup: " + new Date().toLocaleTimeString("vi"));
db.raw.write();

setInterval(function () {
  db.raw.data.rasp.push("Running: " + new Date().toLocaleTimeString("vi"));
  db.raw.write();
}, 1 * 60 * 60 * 1000);

export default app;
