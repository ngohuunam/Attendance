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

import { portWrite, parser } from "./serialport.js";

import { initEventDB, pushEventDB, pushEventDBAndWrite, initInfoDB } from "./db.js";

initEventDB();

const infoDBObj = initInfoDB();

const devices = infoDBObj.devices.data;

const connectLostTimestamp = devices.reduce((p, c) => ({ ...p, ...{ [c.name]: 999 } }), {});

const connectLostCount = devices.reduce((p, c) => ({ ...p, ...{ [c.name]: 0 } }), {});

const pingCount = { ...connectLostCount };

let temp = {};

parser.on("data", function (data) {
  const dataArr = data.replace(/(?:\r\n|\r|\n)/g, "").split(",");
  const _symbol = dataArr[0];
  const _no = dataArr[1];
  const device = devices.find((dv) => dv.symbol === _symbol && dv.no === _no);

  if (!device) return;

  const _fn = dataArr[2];

  const _preResp = `${_symbol},${_no},${_fn}`;

  const _cmd = dataArr[3];
  const _prop = device.name;
  const _time = new Date().toLocaleTimeString("vi");

  let _eventDb;

  if (connectLostTimestamp[_prop] > 0) {
    _eventDb = pushEventDB("raw", _prop, `Connected: ${_time}`, device.name);
    connectLostTimestamp[_prop] = 0;
  }

  connectLostCount[_prop] = 0;

  if (_cmd === "p") {
    pingCount[_prop]++;
    const _res = `${_preResp},pan,${pingCount[_prop]}`;
    return portWrite(_res);
  }

  const _value = dataArr[4];
  let _savedata = `${_time}: ${data}`;
  let _needWriteDB = false;
  let _res;

  _eventDb = pushEventDB("raw", _prop, _savedata, device.name);
  _needWriteDB = !!_eventDb && true;

  if (_cmd === "fsc") {
    _res = `${_preResp},fsc,err,noid`;
    if (_value) {
      _res = `${_preResp},fsc,err,noqueryid`;
      const _queryUserId = device.fingerIds.find((_idObj) => _idObj.fingerId === _value)?.userId;
      if (_queryUserId) {
        _res = `${_preResp},fsc,err,denied`;
        const _permissions = infoDBObj.doors.data.find((d) => d.deviceId === device.id)?.permissions;
        if (_permissions) {
          const _user = _permissions.find((_pObj) => _pObj.userId === _queryUserId);
          if (_user) {
            _res = `${_preResp},fsc,granted`;
            console.log(`user: ${_user.userId}, position: ${device.name} permission granted`);
          } else {
            console.log(`user: ${_user.userId}, position: ${device.name} permission denied`);
          }
        }
      }
    }
  } else if (_cmd === "fe") {
    if (_fn === "q") {
      if (_value) {
        const [_id, _name] = _value.split(",");
        if (parseInt(_id) < 1 || parseInt(_id) > 200) {
          _res = `${_preResp},fe,err,idinvalid`;
        } else {
          temp.name = _name;
          const _idIsExisted = device.fingerIds.some((_idObj) => _idObj.fingerId === _id);
          _res = `${_preResp},fe,${_idIsExisted ? "err,exist" : _id}`;
        }
      } else {
        for (let i = 1; i <= 200; i++) {
          const _idIsExisted = device.fingerIds.some((_idObj) => _idObj.fingerId === i.toString());
          if (!_idIsExisted) {
            _res = `${_preResp},fe,${i}`;
            break;
          }
        }
      }
    } else if (_fn === "f") {
      device.fingerIds.push({
        fingerId: _value,
        userId: temp.name,
      });
      infoDBObj.devices.write();
      _res = `${_preResp},fe,done`;
    }
  }

  if (_res) portWrite(_res);

  if (_needWriteDB) {
    _eventDb.write();
  }
});

portWrite("turn on");

const checkConnect = () => {
  let _needWriteDB = false;
  let _eventDb;
  Object.keys(connectLostCount).map((key) => {
    // console.log(`${key}: ${connectLostCount[key]}`);
    connectLostCount[key]++;
    if (connectLostCount[key] > 3 && (connectLostTimestamp[key] === 0 || connectLostTimestamp[key] === 999)) {
      const timestamp = Date.now();
      connectLostTimestamp[key] = timestamp;
      const _device = devices.find((dv) => dv.name === key);
      const _savedata = `Connect timeout: ${new Date(timestamp).toLocaleTimeString("vi")}`;
      _eventDb = pushEventDB("raw", key, _savedata, _device.name);
      _needWriteDB = !!_eventDb && true;
    }
  });
  if (_needWriteDB) {
    console.log("~ connectLostTimestamp:", connectLostTimestamp);
    _eventDb.write();
  }
};

const raspRunning = () => {
  const _savedata = `Running: ${new Date().toLocaleTimeString("vi")}`;
  pushEventDBAndWrite("raw", "rasp", _savedata);
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

pushEventDBAndWrite("raw", "rasp", "Startup: " + new Date().toLocaleTimeString("vi"));

export default app;
