import { join, dirname } from "node:path"
import { fileURLToPath } from "node:url"

import { LowSync } from "lowdb"
import { JSONFileSync } from "lowdb/node"
import lodash from 'lodash'

import fs from "fs"

import { runAtSpecificTimeOfDay, propExists, RunAtSpecificTimeOfDayTimerID_T } from "./tools.ts"

import { botSend } from "./bot.ts"
import { DB } from "types/DB"

// Extend Low class with a new `chain` field
export class LowWithLodash<T> extends LowSync<T> {
  chain: lodash.ExpChain<this['data']> = lodash.chain(this).get('data')
  data: T = this.data
}

let infoDB: undefined | LowWithLodash<DB>

const eventDBNames = ["raw", "finger", "qr"] as const
type eventDBNames_T = typeof eventDBNames[number]
type RawData_T = { rasp: string[], "Nhà xe": string[] }

type OmitEventDBData_T = Omit<Record<eventDBNames_T, string[]>, "raw">
type EventDBData_T = OmitEventDBData_T & { raw: RawData_T }

type EventDBObj_T = Record<eventDBNames_T, undefined | LowWithLodash<EventDBData_T>>
let eventDBObj = {} as EventDBObj_T

let timerID: RunAtSpecificTimeOfDayTimerID_T = { out: 0, interval: 0 }

export const initInfoDB = () => {
  if (infoDB !== undefined) return infoDB
  
  const _file = join(__dirname, `/dbjson/db.json`)
  const _adapter = new JSONFileSync<DB>(_file)

  infoDB = new LowWithLodash<DB>(_adapter, { devices: [], doors: [], users: [], qrs: [] })

  return infoDB
}

const createLowDBPath: () => { folder: string, json: Record<typeof eventDBNames[number], string> } = () => {
  const d = new Date()
  const year = d.getFullYear()
  const month = `0${d.getMonth() + 1}`.slice(-2)
  const folder = `/dbjson/${year}/THANG${month}/`
  const day = `0${d.getDate()}`.slice(-2)
  const filename = `${day}${month}${year}`
  return { folder, json: { raw: `${filename}_raw.json`, finger: `${filename}_finger.json`, qr: `${filename}_qr.json` } }
}

export const createEventDB = () => {

  if (checkEventDBCreated()) return eventDBObj

  let pathLowDB = createLowDBPath()
  console.log(pathLowDB)

  const folderPath = join(__dirname, pathLowDB.folder)

  if (!fs.existsSync(folderPath)) {
    fs.mkdirSync(folderPath, { recursive: true })
  }

  const _defaultData = {
    raw: {} as RawData_T,
    finger: [],
    qr: [],
  } as EventDBData_T

  eventDBNames.map((key) => {
    const _file = join(folderPath, pathLowDB.json[key])
    const _adapter = new JSONFileSync<EventDBData_T>(_file)
    //@ts-ignore
    eventDBObj[key] = new LowWithLodash<EventDBData_T>(_adapter, _defaultData[key])
    eventDBObj[key]?.read()
  })

  return eventDBObj
}

type PushEventDB_I = (dbName: eventDBNames_T, path: string, value: any, botMess?: string) => LowWithLodash<EventDBData_T> | undefined

export const pushEventDB: PushEventDB_I = (dbName, path, value, botMess) => {
  createEventDB();
  const db = eventDBObj[dbName]
  db?.chain.update(path, (n) => (n ? [...n, ...[value]] : [value])).value()
  botSend((botMess ? `${botMess}: ` : "") + value)
  return db
}

export const pushEventDBAndWrite: PushEventDB_I = (dbName, path, value, botMess) => {
  const _db = pushEventDB(dbName, path, value, botMess)
  _db?.write()
  return _db
}

export const initEventDB = () => {
  runAtSpecificTimeOfDay(0, 0, createEventDB, timerID)
}

export const checkEventDBCreated = () => (eventDBNames.every(name => propExists(eventDBObj, name + ".data")))
