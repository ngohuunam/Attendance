import { portWrite } from "serial.ts"
import { uuid } from "tools.ts"

export const cmdSendStores: {
  cmdId: string
  raw: string
  timestamp: number
  counter: number
}[] = []

const CmdObjProps = ["deviceId", "func", "cmd", "cmdId", "value", "err"] as const

type CmdObj_T = Record<typeof CmdObjProps[number], string>

export const rawCmd =
  ({ deviceId, func, cmd, cmdId, value, err }: CmdObj_T, id?: string) =>
    `${deviceId},${func},${cmd},${cmdId ? cmdId : (id ? id : uuid())},${value}${err ? `,${err}` : ""}`

export const sendCmd = (cmdObj: CmdObj_T) => {
  const cmdId = uuid()
  const raw = rawCmd(cmdObj, cmdId)
  cmdSendStores.push({
    cmdId, raw,
    timestamp: Date.now(),
    counter: 0
  })
  portWrite(raw)
}

const checkCmd = (cmdStore: CmdStore_T) => {
  const now = Date.now()
  const period = 1 * 1 * 1 * 1000
  const timepast = cmdStore.timestamp - now > period
  cmdStore.counter += timepast ? 1 : 0
  if (cmdStore.counter > 2) {
    console.log("🚀 ~ file: monitor.ts:41 ~ checkCmd ~ counter > 2: ", cmdStore)
    cmdStore.counter = 0
    cmdStore.timestamp = Date.now()
  }
}

export const checkCmdStores = () => cmdSendStores.map(cmdStore => checkCmd(cmdStore))

export const checkCmdStoresInterval = setInterval(function () { checkCmdStores() }, 20)

export type CmdStore_T = typeof cmdSendStores[number]
export type CmdStores_T = CmdStore_T[]

export const parseRawCmd = (raw: string) => raw.split(',').reduce((p, c, i) => ({ ...p, [CmdObjProps[i]]: c }), {} as CmdObj_T)

const validateSendCmd = (raw: string) => {
  const cmdObj = parseRawCmd(raw)
  const { cmdId } = cmdObj
  const cmdStoreIdx = cmdSendStores.findIndex(cmd => cmd.cmdId === cmdId)
  const cmdStore = cmdSendStores[cmdStoreIdx]
  if (cmdStore) {
    cmdSendStores.splice(cmdStoreIdx, 1)
    if (cmdStore.raw !== raw) {

    }
  }
}

