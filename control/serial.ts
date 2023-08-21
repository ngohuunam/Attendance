import { SerialPortStream } from "@serialport/stream"
import { DelimiterParser } from '@serialport/parser-delimiter'

import { MockBinding } from "@serialport/binding-mock"

import { pushEventDBAndWrite } from "db"
import { ping } from "handler"

const portPath = process.env.SERIAL_PATH
const baudRate = Number(process.env.SERIAL_BAUDRATE)
const delimiter = process.env.SERIAL_DELIMITER

MockBinding.createPort(portPath, { echo: true, record: true })

export const port = new SerialPortStream({ binding: MockBinding, path: portPath, baudRate })

export const parser = port.pipe(new DelimiterParser({ delimiter }))

// Open errors will be emitted as an error event
port.on("error", function (err) {
  const _savedata = `Port error: ${err.message} - ${new Date().toLocaleTimeString("vi")}`
  console.error(_savedata)
  pushEventDBAndWrite("raw", "rasp", _savedata)
})

// wait for port to open...
port.on('open', () => {
  // ...then test by simulating incoming data
  ping({ deviceId: "all", func: "r" })
  port.port?.emitData("Hello, world!\n")
  const _savedata = `Port open: ${new Date().toLocaleTimeString("vi")}`
  pushEventDBAndWrite("raw", "rasp", _savedata)
})


////////////////////////////////////////////////////////////////////////////////////////////////////////////

const portDatas: PortDatas_T = []
let timeID: NodeJS.Timeout | null = null
let portFree = true

const writeAndDrain = () => {
  if (port.isOpen) {
    const data = portDatas.shift()
    if (data && portFree) {
      portFree = false
      try {
        port.flush(error => {
          if (error) return console.error('Flush error: ', error)

          port.write(data, error => {
            if (error) return console.error('Write error: ', error)
          })

          port.drain(error => {
            portFree = true
            if (error) return console.error('Drain error: ', error)
          })
        })
      } catch (e) {
        console.error(e)
      }
    }
  }
}

timeID = setInterval(writeAndDrain, 300)

export type PortDatas_T = string[]

export const portWrite = (data: string) => {
  portDatas.push(data)
  console.log("~ file: serial.ts:73 ~ portDatas:", portDatas)
  pushEventDBAndWrite("raw", "rasp", `port write: ${data} - ${new Date().toLocaleTimeString("vi")}`)
  return portDatas
}
