import { pushEventDBAndWrite } from "db"
import { authFingerprint, ping, storeEnrolledFingerprint } from "handler"
import { portWrite } from "serial.ts"
import { UartData_T, deviceIDs, uartDataProps, uuid } from "tools.ts"

export type CmdObj_T = {
  cmdId: string
  raw: string
  timestamp: number
  interval: number
  retry: number
  counter: number
  source: string
}

export const cmdStores: CmdObj_T[] = []

export const rawCmd =
  ({ deviceId, func, cmd, cmdId, value, err }: UartData_T, id?: string) =>
    `${deviceId},${func},${cmd},${cmdId ? cmdId : (id ? id : uuid())},${value}${err ? `,${err}` : ""}`

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
  console.log("~ file: monitor.ts:31 ~ uartSendCmd ~ cmdStores:", cmdStores)
  return portWrite(raw)
}

export const processUartRx = (raw: string) => {
  pushEventDBAndWrite("raw", "rasp", `uart rx: ${raw} - ${new Date().toLocaleTimeString("vi")}`)
  const data = parseUartStr(raw)
  const id = deviceIDs.find(_id => data.deviceId.includes(_id))
  if (!id) return false
  data.deviceId = id
  const idx = cmdStores.findIndex(cmd => cmd.cmdId === data.cmdId)
  console.log("~ file: monitor.ts:43 ~ processUartRx ~ cmd index:", idx)
  pushEventDBAndWrite("raw", "rasp", `cmd index: ${idx} - ${new Date().toLocaleTimeString("vi")}`)
  if (idx > -1) {
    cmdStores.splice(idx, 1)
    return true
  }
  clearInterval(pingInterval)
  switch (data.cmd) {
    case "captured":
      return authFingerprint(data)
    case "enrolled":
      return storeEnrolledFingerprint(data)
    default:
  }
  return false
}

const checkCmd = (cmdStore: CmdObj_T) => {
  const now = Date.now()
  const period = 1 * 1 * 1 * 1000
  const timepast = cmdStore.timestamp - now > period
  cmdStore.counter += timepast ? 1 : 0
  if (cmdStore.counter > 2) {
    cmdStore.counter = 0
    cmdStore.timestamp = Date.now()
  }
}

export const checkCmdStores = () => cmdStores.map(cmdStore => checkCmd(cmdStore))

export const checkCmdStoresInterval = setInterval(function () { checkCmdStores() }, 20)

export const parseRawCmd = (raw: string) => raw.split(',').reduce((p, c, i) => ({ ...p, [uartDataProps[i]]: c }), {} as UartData_T)

const validateSendCmd = (raw: string) => {
  const cmdObj = parseRawCmd(raw)
  const { cmdId } = cmdObj
  const cmdStoreIdx = cmdStores.findIndex(cmd => cmd.cmdId === cmdId)
  const cmdStore = cmdStores[cmdStoreIdx]
  if (cmdStore) {
    cmdStores.splice(cmdStoreIdx, 1)
    if (cmdStore.raw !== raw) {

    }
  }
}

export const parseUartStr = (raw: string) => {
  const uartData = raw.trim().split(',').reduce((p, c, i) => ({ ...p, ...{ [uartDataProps[i]]: c } }), {} as UartData_T)
  uartData.source = raw
  return uartData
}

const pingInterval = setInterval(function () { ping("r1", "r") }, 5000)