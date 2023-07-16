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
import { initEventDB, pushEventDBAndWrite, initInfoDB } from "./db.ts"
import { processUartRx } from 'monitor.ts'

initEventDB()

const infoDB = initInfoDB()

const devices = infoDB.chain.get("devices")

parser.on("data", function (raw : Buffer) {
  console.log("~ file: app.ts:27 ~ raw:", raw.toString())
  processUartRx(raw.toString())
})

portWrite("turn on")

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