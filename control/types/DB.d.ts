import { UartData_T } from "tools";

export type DB = {
  devices: Device[];
  doors: Door[];
  users: User[];
  qrs: QR[];
}

export type Device = {
  id: string;
  type: string;
  doorID: string;
  functions: string[];
  fingerIDs: FingerID[];
}

export type FingerID = {
  fingerID: string;
  userID: string;
}

export type Door = {
  id: string;
  name: string;
  alias: string;
  deviceID: string;
  relayID: string;
  permissions: Permission[];
}

export type Permission = {
  userID: string;
  time: string;
  active: boolean;
}

export type User = {
  id: string;
  name: string;
  room: string;
  bikes: Bike[];
  personalInfo: PersonalInfo;
}

export type Bike = {
  name: string;
  no: string;
  doorID: string;
}

export type PersonalInfo = {
  fullName: string;
  cccd: string;
  birthday: string;
  gen: string;
  address: string;
}

export type QR = {
  id: string;
  cmd: UartData_T;
  active: boolean | string;
}