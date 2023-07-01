import { join, dirname } from "node:path";
import { fileURLToPath } from "node:url";
import { LowSync } from "lowdb";
import { JSONFileSync } from "lowdb/node";

import fs from "fs";

import { runAtSpecificTimeOfDay } from "./tools.js";


const __filename = fileURLToPath(import.meta.url);

const __dirname = dirname(__filename);

export const createDB = () => {
  const createLowDBPath = () => {
    const d = new Date();
    const year = d.getFullYear();
    const month = `0${d.getMonth() + 1}`.slice(-2);
    const day = `0${d.getDay()}`.slice(-2);
    return { folder: `/dbjson/${year}/THANG${month}/`, raw: `${day}${month}${year}_raw.json`, finger: `${day}${month}${year}_finger.json`, qr: `${day}${month}${year}_qr.json` };
  };

  let pathLowDB = createLowDBPath();
  console.log(pathLowDB);

  const folderPath = join(__dirname, pathLowDB.folder);

  if (!fs.existsSync(folderPath)) {
    fs.mkdirSync(folderPath, { recursive: true });
  }

  const _db = { raw: "", finger: "", qr: "" };

  const _file = { raw: "", finger: "", qr: "" };

  const _adapter = { raw: "", finger: "", qr: "" };

  const _defaultData = {
    raw: { host: [], reader_1: [], reader_2: [], reader_3: [], reader_4: [], rasp: [] },
    finger: [],
    qr: [],
  };

  Object.keys(_db).map((key) => {
    _file[key] = join(folderPath, pathLowDB[key]);
    _adapter[key] = new JSONFileSync(_file[key]);
    _db[key] = new LowSync(_adapter[key], _defaultData[key]);
  });

  return _db;
};

export const initDB = (h, m) => {
  const _db = createDB();
  runAtSpecificTimeOfDay(h, m, createDB);
  return _db;
};
