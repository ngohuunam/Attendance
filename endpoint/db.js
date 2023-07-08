import { join, dirname } from "node:path";
import { fileURLToPath } from "node:url";

import { LowSync } from "lowdb";
import { JSONFileSync } from "lowdb/node";

import fs from "fs";

import { runAtSpecificTimeOfDay, propExists } from "./tools.js";

import { botSend } from "./bot.js";

let eventDBObj;
let infoDBObj;
let timerID = { out: 0, interval: 0 };

const __filename = fileURLToPath(import.meta.url);

const __dirname = dirname(__filename);

export const initInfoDB = () => {
  console.log(checkInfoDBCreated());
  if (checkInfoDBCreated()) return infoDBObj;

  const _jsonFileNames = ["devices", "doors", "users", "relays", "status"];

  infoDBObj = {};

  _jsonFileNames.map((name) => {
    const _file = join(__dirname, `/dbjson/${name}.json`);
    const _adapter = new JSONFileSync(_file);
    infoDBObj[name] = new LowSync(_adapter, []);
    infoDBObj[name].read();
  });

  return infoDBObj;
};

export const checkInfoDBCreated = () => Boolean(propExists(infoDBObj, "devices.data"));

export const createEventDB = () => {
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

  if (!eventDBObj) eventDBObj = {};

  Object.keys(_defaultData).map((key) => {
    const _file = join(folderPath, pathLowDB[key]);
    const _adapter = new JSONFileSync(_file);
    eventDBObj[key] = new LowSync(_adapter, _defaultData[key]);
    eventDBObj[key].read();
  });

  return eventDBObj;
};

import _update from "lodash.update";

export const pushEventDB = (dbName, path, value, botMess) => {
  if (!checkEventDBCreated()) createEventDB();
  const _db = eventDBObj[dbName];
  if (!_db) return;
  const _deep = `data.${path}`;
  _update(_db, _deep, (n) => (n ? [...n, ...[value]] : [value]));
  botSend((botMess ? `${botMess}: ` : "") + value);
  return _db;
};

export const pushEventDBAndWrite = (dbName, path, value, botMess) => {
  const _db = pushEventDB(dbName, path, value, botMess);
  if (!_db) return;
  return _db.write();
};

export const initEventDB = () => {
  if (!checkEventDBCreated()) createEventDB();
  runAtSpecificTimeOfDay(0, 0, createEventDB, timerID);
};

export const checkEventDBCreated = () => Boolean(propExists(eventDBObj, "raw.data"));
