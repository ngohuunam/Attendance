import { SerialPortStream } from "@serialport/stream"
import { ReadlineParser } from "@serialport/parser-readline"

import { MockBinding } from "@serialport/binding-mock"

import { pushEventDBAndWrite } from "./db.ts"

const portPath = '/dev/ROBOT'
MockBinding.createPort(portPath, { echo: true, record: true })

export const port = new SerialPortStream({ binding: MockBinding, path: portPath, baudRate: 14400 })

export const parser = port.pipe(new ReadlineParser())

// Open errors will be emitted as an error event
port.on("error", function (err) {
  const _savedata = `Port error: ${err.message} - ${new Date().toLocaleTimeString("vi")}`
  console.error(_savedata)
  pushEventDBAndWrite("raw", "rasp", _savedata)
})

// wait for port to open...
port.on('open', () => {
  port.write('ecec')
  port.drain()
  // ...then test by simulating incoming data
  port.port?.emitData("Hello, world!\n")
  const _savedata = `Port open: ${new Date().toLocaleTimeString("vi")}`
  pushEventDBAndWrite("raw", "rasp", _savedata)
})


////////////////////////////////////////////////////////////////////////////////////////////////////////////

// let portBusy = false
const portDatas: string[] = []
let portFree = true

const writeAndDrain: (calback: () => void) => void = (callback) => {
  port.flush()
  portFree = false
  const data = portDatas.shift()
  port.write(data)
  port.drain(callback)
}

const writeAndDrainMulti = () => {
  console.log("🚀 ~ file: serialport.ts:42 ~ writeAndDrainMulti ~ portDatas.length:", portDatas.length)
  if (portDatas.length > 0) {
    writeAndDrain(writeAndDrainMulti)
  } else {
    portFree = true
  }
}

export const portWrite: (data: string) => void = (data) => {
  portDatas.push(data)
  if (portFree) writeAndDrainMulti()
}

