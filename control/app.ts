'use strict'

import * as dotenv from 'dotenv'
dotenv.config()

import formbody from '@fastify/formbody'
import sensible from '@fastify/sensible'

import rootRoute from './routes/root.ts'

import Fastify from 'fastify'
// import { Server, IncomingMessage, ServerResponse } from 'http'

const port: number = Number(process.env.PORT) || 5000

import { portWrite, parser } from "./serial.ts"
import { initEventDB, pushEventDB, pushEventDBAndWrite, initInfoDB, InfoDBData_T, LowWithLodash } from "./db.ts"

initEventDB()

const infoDBObj = initInfoDB()

const devicesDB = infoDBObj.devices as undefined | LowWithLodash<DeviceDatas_T>

const devices = devicesDB?.data

const connectLostTimestamp = devices!.reduce((p, c) => ({ ...p, ...{ [c.name]: 999 } }), {} as {[k: string]: number})

const connectLostCount = devices!.reduce((p, c) => ({ ...p, ...{ [c.name]: 0 } }), {} as {[k: string]: number})

const pingCount = { ...connectLostCount }

let temp = {} as any

parser.on("data", function (data: string) {
  console.log("🚀 ~ file: app.ts:36 ~ data:", data)
})

portWrite("turn on")

const checkConnect = () => {
  let _needWriteDB = false
  let _eventDb: any
  Object.keys(connectLostCount).map((key) => {
    // console.log(`${key}: ${connectLostCount[key]}`)
    connectLostCount[key]++
    if (connectLostCount[key] > 3 && (connectLostTimestamp[key] === 0 || connectLostTimestamp[key] === 999)) {
      const timestamp = Date.now()
      connectLostTimestamp[key] = timestamp
      const _device = devices?.find((dv) => dv.name === key)
      const _savedata = `Connect timeout: ${new Date(timestamp).toLocaleTimeString("vi")}`
      _eventDb = pushEventDB("raw", key, _savedata, _device?.name)
      _needWriteDB = !!_eventDb && true
    }
  })
  if (_needWriteDB) {
    console.log("~ connectLostTimestamp:", connectLostTimestamp)
    _eventDb?.write()
  }
}

const raspRunning = () => {
  const _savedata = `Running: ${new Date().toLocaleTimeString("vi")}`
  pushEventDBAndWrite("raw", "rasp", _savedata)
}

const checkConnectInterval_ms = 1 * 10 * 1000

const raspRunningInterval_ms = 1 * 60 * 60 * 1000

let checkConnectInterval = setInterval(checkConnect, checkConnectInterval_ms)

let raspRunningInterval = setInterval(raspRunning, raspRunningInterval_ms)

const restartCheckInterval = () => {
  clearInterval(checkConnectInterval)
  checkConnectInterval = setInterval(checkConnect, checkConnectInterval_ms)
  clearInterval(raspRunningInterval)
  raspRunningInterval = setInterval(raspRunning, raspRunningInterval_ms)
}

import { RunAtSpecificTimeOfDayTimerID_T, runAtSpecificTimeOfDay } from "./tools.js"
import { DeviceDatas_T } from 'types/Devices'

let timerID: RunAtSpecificTimeOfDayTimerID_T = { out: 0, interval: 0 }

runAtSpecificTimeOfDay(
  0,
  2,
  function () {
    restartCheckInterval()
  },
  timerID
)

pushEventDBAndWrite("raw", "rasp", "Startup: " + new Date().toLocaleTimeString("vi"))

const startServer = async () => {
  try {
    const server = Fastify({})
    server.register(formbody)
    server.register(sensible)
    server.register(rootRoute)
    server.setErrorHandler((error) => {
      server.log.error(error)
    })

    await server.listen({ port })
  } catch (e) {
    console.error(e)
  }
}

process.on('unhandledRejection', (e) => {
  console.error(e)
  process.exit(1)
})

await startServer()

export default {}