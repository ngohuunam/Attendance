'use strict'

import * as dotenv from 'dotenv'
dotenv.config()

import formbody from '@fastify/formbody'
import sensible from '@fastify/sensible'

import rootRoute from './routes/root.ts'

import Fastify from 'fastify'
// import { Server, IncomingMessage, ServerResponse } from 'http'

const port: number = Number(process.env.PORT) || 5000

import { portWrite, parser } from "serial"
import { initEventDB, pushEventDBAndWrite, initInfoDB } from "db"
import { processUartRx } from 'monitor'
import initBot from 'bot'

initEventDB()
initInfoDB()
initBot()

// const devices = infoDB.chain.get("devices")

parser.on("data", function (raw : Buffer) {
  console.log("~ file: app.ts:28 ~ raw:", raw.toString())
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