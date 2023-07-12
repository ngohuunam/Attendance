import { InfoDBData_T } from "db"

export type DeviceDatas_T = DeviceData_T[]

export interface DeviceData_T extends InfoDBData_T {
  id: string
  type: string
  name: string
  symbol: string
  no: string
  ping: number
  functions: string[]
  fingerIds: DeviceDatas_FingerId_I[]
}

export interface DeviceDatas_FingerId_I {
  fingerId: string
  userId: string
}
