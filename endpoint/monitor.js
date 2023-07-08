export const systemStatus = {
  host: [{ id: "h0", timestamp: 0, time: "", status: "" }],
  reader: {},
};

export const checkHost = () => {};

export const checkReader = () => {};

export const checkInternet = () => {};

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
      _db = pushEventDB("raw", key, _savedata, _device.displayName);
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
