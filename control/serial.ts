import { SerialPortStream } from "@serialport/stream"
import { DelimiterParser } from '@serialport/parser-delimiter'

import { MockBinding } from "@serialport/binding-mock"

import { pushEventDBAndWrite } from "./db.ts"

const portPath = '/dev/ROBOT'
MockBinding.createPort(portPath, { echo: true, record: true })

export const port = new SerialPortStream({ binding: MockBinding, path: portPath, baudRate: 14400 })

export const parser = port.pipe(new DelimiterParser({ delimiter: '.' }))

// Open errors will be emitted as an error event
port.on("error", function (err) {
  const _savedata = `Port error: ${err.message} - ${new Date().toLocaleTimeString("vi")}`
  console.error(_savedata)
  pushEventDBAndWrite("raw", "rasp", _savedata)
})

// wait for port to open...
port.on('open', () => {
  // ...then test by simulating incoming data
  port.port?.emitData("Hello, world!\n")
  const _savedata = `Port open: ${new Date().toLocaleTimeString("vi")}`
  pushEventDBAndWrite("raw", "rasp", _savedata)
})


////////////////////////////////////////////////////////////////////////////////////////////////////////////

const portDatas: PortDatas_T = []
let timeID: NodeJS.Timeout | null = null

const writeAndDrain = () => {
  port.flush()
  const data = portDatas.shift()
  if (data) {
    try {
      port.write(data, function () {
        port.drain(function() {

        })
      })

    } catch (e) {
      console.error(e)
    }
  }
}

timeID = setInterval(writeAndDrain, 300)

export type PortDatas_T = string[]

export const portWrite = (data: string) => {
  const _data = data + '.'
  portDatas.push(_data)
  pushEventDBAndWrite("raw", "rasp", `port write: ${_data} - ${new Date().toLocaleTimeString("vi")}`)
  return portDatas
}

