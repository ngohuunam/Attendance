import { pushEventDBAndWrite } from "db"
import { authFingerprint, handlePing, handleQrCode, setPingIntervalMinuteAndRestartIntervalPingDeviceOne, storeEnrolledFingerprint } from "handler"
import { CmdObj_T } from "index"
import { portWrite } from "serial"
import { UartData_T, deviceIDs, uartDataProps, uuid } from "tools"

export const cmdStores: CmdObj_T[] = []

const delimiter = process.env.SERIAL_DELIMITER

export const rawCmd =
  ({ deviceId, func, cmd, cmdId, value, err }: UartData_T, id?: string) =>
    `${deviceId},${func},${cmd},${cmdId ? cmdId : (id ? id : uuid())}${value ? `,${value}` : ""}${err ? `,${err}` : ""}${delimiter}`

export const uartSendCmd = (cmdObj: UartData_T) => {
  const raw = rawCmd(cmdObj)
  const { cmdId, source } = cmdObj
  cmdStores.push({
    cmdId, source, raw,
    timestamp: Date.now(),
    interval: 1 * 1 * 1000,
    retry: 3,
    counter: 0,
  })
  console.log("~ file: monitor.ts:25 ~ uartSendCmd ~ cmdStores:", cmdStores)
  return portWrite(raw)
}

export const uartSendRaw = (raw: string) => {
  const cmdObj = parseRawCmd(raw)
  return uartSendCmd(cmdObj)
}

export const processUartRx = (raw: string) => {
  const data = parseUartStr(raw)
  const deviceId = deviceIDs.find(_id => data.deviceId.includes(_id))
  if (!deviceId) return
  data.deviceId = deviceId
  const idx = cmdStores.findIndex(cmd => cmd.cmdId === data.cmdId)
  console.log("~ file: monitor.ts:40 ~ processUartRx ~ cmd index:", idx)
  if (idx > -1) {
    cmdStores.splice(idx, 1)
    return
  }
  pushEventDBAndWrite("raw", "rasp", `cmd index: ${idx} - ${new Date().toLocaleTimeString("vi")}`)
  switch (data.cmd) {
    case "ping":
      handlePing(data)
      break
    case "captured":
      authFingerprint(data)
      break
    case "enrolled":
      storeEnrolledFingerprint(data)
      break
    case "read":
      handleQrCode(data)
      break
    default:
    // clearInterval(pingInterval)
  }
}

export const parseRawCmd = (raw: string) => raw.split(',').reduce((p, c, i) => ({ ...p, [uartDataProps[i]]: c }), {} as UartData_T)

export const writeDBRawUartRx = (raw: string) => {
  pushEventDBAndWrite("raw", "rasp", `uart rx: ${raw} - ${new Date().toLocaleTimeString("vi")}`)
}

export const parseUartStr = (raw: string) => {
  raw.trim()
  writeDBRawUartRx(raw)
  const uartData = raw.split(',').reduce((p, c, i) => ({ ...p, ...{ [uartDataProps[i]]: c } }), {} as UartData_T)
  uartData.source = raw
  return uartData
}

setPingIntervalMinuteAndRestartIntervalPingDeviceOne(5, "r1")