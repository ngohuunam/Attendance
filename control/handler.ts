import { initInfoDB } from 'db'
import { uartSendCmd } from 'monitor'
import { CmdTable_T, DeviceIDs_T, Funcs_T, UartData_T, cmdTable, deviceIDs, uuid } from "tools"

while (initInfoDB === undefined) { }
const infoDB = initInfoDB()

export const handlePing = ({ deviceId, cmdId }: UartData_T) => {
  if (deviceId.includes("r")) {
    ping({ deviceId, func: "r", cmdId })
  }
}

export const handleQrCode = (data: UartData_T) => {
  const cmd = infoDB.chain.get("qrs").find({ id: data.value, active: true }).get("cmd").value()
  console.dir(cmd, { depth: null })
}

export const storeEnrolledFingerprint = async (data: UartData_T) => {
  try {
    const device = infoDB.chain.get("devices").find({ id: data.deviceId })
    const fingerIDs = device.get("fingerIDs")
    const fingerID = data.value
    const userIDObj = fingerIDs.find({ fingerID }).value()
    const userID = userIDObj?.userID || uuid()
    if (userIDObj) {
      userIDObj.fingerID = fingerID
    } else {
      infoDB.chain.get("devices").find({ id: data.deviceId }).get("fingerIDs").push({ userID, fingerID }).value()
    }
    fingerIDs.value()
    const doorID = device.get("doorID").value()
    const readerID = device.get("id").value()
    const permissions = infoDB.chain.get("doors").find({ id: doorID, deviceID: readerID }).get("permissions")
    const permission = permissions.find({ userID }).value()
    console.log("🚀 ~ file: handler.ts:36 ~ storeEnrolledFingerprint ~ permission:", permission)
    if (!permission) {
      infoDB.chain.get("doors").find({ id: doorID, deviceID: readerID }).get("permissions").push({ userID, active: false, time: "all" }).value()
      await infoDB.write()
    }
    uartSendCmdNotify({ data, raw: "ok" })
  } catch (e) {
    console.error(e)
    uartSendCmdNotify({ data, raw: "error" })
  }
}

export const authFingerprint = (data: UartData_T) => {
  let isAuth = false
  try {
    const device = infoDB.chain.get("devices").find({ id: data.deviceId })
    const userId = device.get("fingerIDs").find({ fingerID: data.value }).get("userID").value()
    const doorID = device.get("doorID").value()
    const readerID = device.get("id").value()
    const door = infoDB.chain.get("doors").find({ id: doorID, deviceID: readerID })
    isAuth = door.get("permissions").some({ userID: userId, active: true, time: "all" }).value()
    if (isAuth) {
      const relayID = door.get("relayID").value()
      console.log("~ file: handler.ts:52 ~ authFingerprint ~ relayID:", relayID)
      openDoor({ relayID, source: data.source })
    }
  } catch (e) {
    console.error(e)
  }
  uartSendCmdNotify({ data, raw: isAuth ? "ok" : "error" })
  return isAuth
}

export type UartSendHostFn_T = ({ func, cmd, cmdId, value, source }: {
  func: Funcs_T;
  cmd: keyof CmdTable_T["relay"];
  cmdId?: string | undefined;
  value: string;
  source: string;
}) => void

export const uartSendHostFn: UartSendHostFn_T = ({ func, cmd, cmdId = uuid(), value, source }) => {
  uartSendCmd({ deviceId: 'h0', func, cmd, cmdId, value, err: "", source })
}

export const openDoor = ({ relayID, source }: { relayID: string, source: string }) => {
  uartSendHostFn({ func: "rl", cmd: "push", value: relayID, source })
}

export const uartSendCmdNotify = ({ data, raw }: { data: UartData_T, raw: keyof CmdTable_T["led"] }) => uartSendFnCmd<"led">({ data, func: "led", raw })

export type UartSendFnCmd_T = <T extends keyof CmdTable_T>(params: { data: UartData_T, func: T, raw: keyof CmdTable_T[T] }) => string[]

export const uartSendFnCmd: UartSendFnCmd_T = ({ data, func, raw }) => {
  const cmd = { ...data, ...cmdTable[func][raw] }
  console.log("~ file: handler.ts:79 ~ uartSendFnCmd ~ cmd:", cmd)
  return uartSendCmd(cmd)
}

export const ping = ({ deviceId, func, cmdId = uuid() }: { deviceId: DeviceIDs_T, func: Funcs_T, cmdId?: string }) => {
  uartSendCmd({ deviceId, func, cmd: "ping", cmdId, value: "", err: "", source: "" })
}

let pingIntervalMinute = 5;

export const setPingIntervalMinute = (minute: number) => pingIntervalMinute = minute;

export type PingInterval_T = {
  [id in DeviceIDs_T]: NodeJS.Timer | undefined
}

export const pingInterval: PingInterval_T = deviceIDs.reduce((pre, curr) => ({ ...pre, ...{ [curr]: undefined } }), {} as PingInterval_T)

export const setIntervalPingDeviceOne = (deviceId: DeviceIDs_T) => {
  pingInterval[deviceId] = setInterval(function () { ping({ deviceId, func: "r" }) }, pingIntervalMinute * 60 * 1000)
}

export const clearIntervalPingDeviceOne = (deviceId: DeviceIDs_T) => {
  clearInterval(pingInterval[deviceId]);
}

export const setIntervalPingDeviceMulti = (deviceIds: DeviceIDs_T[]) => {
  deviceIds.map(deviceId => setIntervalPingDeviceOne(deviceId))
}

export const clearIntervalPingDeviceMulti = (deviceIds: DeviceIDs_T[]) => {
  deviceIds.map(deviceId => clearIntervalPingDeviceOne(deviceId))
}

export const restartIntervalPingDeviceOne = (deviceId: DeviceIDs_T) => {
  clearIntervalPingDeviceOne(deviceId)
  setIntervalPingDeviceOne(deviceId)
}

export const restartIntervalPingDeviceMulti = (deviceIds: DeviceIDs_T[]) => {
  deviceIds.map(deviceId => restartIntervalPingDeviceOne(deviceId))
}

export const setPingIntervalMinuteAndRestartIntervalPingDeviceOne = (minute: number, deviceId: DeviceIDs_T) => {
  setPingIntervalMinute(minute)
  restartIntervalPingDeviceOne(deviceId)
}

export const setPingIntervalMinuteAndRestartIntervalPingDeviceMulti = (minute: number, deviceIds: DeviceIDs_T[]) => {
  setPingIntervalMinute(minute)
  restartIntervalPingDeviceMulti(deviceIds)
}

export const setPingIntervalSecondAndRestartIntervalPingDeviceOne = (second: number, deviceId: DeviceIDs_T) => {
  const minute = second / 60
  setPingIntervalMinuteAndRestartIntervalPingDeviceOne(minute, deviceId)
}

export const setPingIntervalSecondAndRestartIntervalPingDeviceMulti = (second: number, deviceIds: DeviceIDs_T[]) => {
  const minute = second / 60
  setPingIntervalMinuteAndRestartIntervalPingDeviceMulti(minute, deviceIds)
}