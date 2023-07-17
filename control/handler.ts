import { initInfoDB } from 'db'
import { uartSendCmd } from 'monitor'
import { CmdTable_T, DeviceIDs_T, Funcs_T, UartData_T, cmdTable, uuid } from "tools"

const infoDB = initInfoDB()

export const handlerQrCode = (data: UartData_T) => {
  const cmd = infoDB.chain.get("qrs").find({ id: data.value, active: true }).get("cmd").value()
  console.dir(cmd, { depth: null })
}

export const storeEnrolledFingerprint = (data: UartData_T) => {
  try {
    const device = infoDB.chain.get("devices").find({ id: data.deviceId })
    const fingerIDs = device.get("fingerIDs")
    const userIDObj = fingerIDs.find({ fingerID: data.value }).value()
    const fingerID = data.value
    if (userIDObj) {
      userIDObj.fingerID = fingerID
    } else {
      fingerIDs.push({ userID: "", fingerID })
    }
    fingerIDs.value()
    const doorID = device.get("doorID").value()
    const readerID = device.get("id").value()
    const permissions = infoDB.chain.get("doors").find({ id: doorID, deviceID: readerID }).get("permissions")
    const permission = permissions.find({ userID: userIDObj.userID }).value()
    if (permission) {
      permission.active = false
    } else {
      permissions.push({ userID: userIDObj.userID, active: false, time: "all" })
    }
    permissions.value()
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

export const openDoor = ({ relayID, source }: { relayID: string, source: string }) => {
  uartSendCmd({ deviceId: "h0", func: "rl", cmd: "open", cmdId: uuid(), value: relayID, err: "", source })
}

export const uartSendCmdNotify = ({ data, raw }: { data: UartData_T, raw: keyof CmdTable_T["led"] }) => uartSendFnCmd<"led">({ data, func: "led", raw })

export type UartSendFnCmd_T = <T extends keyof CmdTable_T>(params: { data: UartData_T, func: T, raw: keyof CmdTable_T[T] }) => string[]

export const uartSendFnCmd: UartSendFnCmd_T = ({ data, func, raw }) => {
  const cmd = { ...data, ...cmdTable[func][raw] }
  console.log("~ file: handler.ts:72 ~ uartSendFnCmd ~ cmd:", cmd)
  return uartSendCmd(cmd)
}

export const ping = (deviceId: DeviceIDs_T, func: Funcs_T) => {
  uartSendCmd({ deviceId, func, cmd: "ping", cmdId: uuid(), value: "", err: "", source: "" })
}