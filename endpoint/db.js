import { join, dirname } from "node:path";
import { fileURLToPath } from "node:url";

import { LowSync } from "lowdb";
import { JSONFileSync } from "lowdb/node";

import fs from "fs";

import { runAtSpecificTimeOfDay, propExists } from "./tools.js";

import { botSend } from "./bot.js";

let dbObj;
let dbDevice;
let timerID = { out: 0, interval: 0 };
  
const __filename = fileURLToPath(import.meta.url);

const __dirname = dirname(__filename);

export const initDBDevice = () => {
  if (dbDevice) return dbDevice;

  const _file = join(__dirname, "/dbjson/device.json");
  const _adapter = new JSONFileSync(_file);
  dbDevice = new LowSync(_adapter, []);
  dbDevice.read();
  return dbDevice;
};

export const createDB = () => {
  const createLowDBPath = () => {
    const d = new Date();
    const year = d.getFullYear();
    const month = `0${d.getMonth() + 1}`.slice(-2);
    const folder = `/dbjson/${year}/THANG${month}/`;
    const day = `0${d.getDate()}`.slice(-2);
    const filename = `${day}${month}${year}`;
    return { folder, raw: `${filename}_raw.json`, finger: `${filename}_finger.json`, qr: `${filename}_qr.json` };
  };

  let pathLowDB = createLowDBPath();
  console.log(pathLowDB);

  const folderPath = join(__dirname, pathLowDB.folder);

  if (!fs.existsSync(folderPath)) {
    fs.mkdirSync(folderPath, { recursive: true });
  }

  const _defaultData = {
    raw: {},
    finger: [],
    qr: [],
  };

  if (!dbObj) dbObj = {};

  Object.keys(_defaultData).map((key) => {
    const _file = join(folderPath, pathLowDB[key]);
    const _adapter = new JSONFileSync(_file);
    dbObj[key] = new LowSync(_adapter, _defaultData[key]);
    dbObj[key].read();
  });

  return dbObj;
};

import _update from "lodash.update";

export const pushDB = (dbName, path, value, botMess) => {
  if (!dbObj) createDB();
  const _db = dbObj[dbName];
  if (!_db) return;
  const _deep = `data.${path}`;
  _update(_db, _deep, (n) => (n ? [...n, ...[value]] : [value]));
  botSend((botMess ? `${botMess}: ` : "") + value);
  return _db;
};

export const pushDBAndWrite = (dbName, path, value, botMess) => {
  const _db = pushDB(dbName, path, value, botMess);
  if (!_db) return;
  return _db.write();
};

export const initDB = () => {
  runAtSpecificTimeOfDay(18, 54, createDB, timerID);
};
