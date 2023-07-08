import { SerialPortStream } from "@serialport/stream";
import { ReadlineParser } from "@serialport/parser-readline";

import { MockBinding } from "@serialport/binding-mock";

import { pushEventDBAndWrite } from "./db.js";

MockBinding.createPort("/dev/ROBOT", { echo: true, record: true });

export const port = new SerialPortStream({ binding: MockBinding, path: "/dev/ROBOT", baudRate: 14400 });

export const parser = port.pipe(new ReadlineParser());

// Open errors will be emitted as an error event
port.on("error", function (err) {
  const _savedata = `Port error: ${err.message} - ${new Date().toLocaleTimeString("vi")}`;
  console.error(_savedata);
  pushEventDBAndWrite("raw", "rasp", _savedata);
});

////////////////////////////////////////////////////////////////////////////////////////////////////////////

// let portBusy = false;
const portDatas = [];

const writeAndDrain = (callback) => {
  const data = portDatas.shift();
  port.write(data);
  port.drain(callback);
};

const writeAndDrainMulti = () => {
  if (portDatas.length() > 0) {
    writeAndDrain(writeAndDrainMulti);
  }
};

const pushAndWriteAndDrainMulti = (data) => {
  portDatas.push(data);
  writeAndDrainMulti();
};

export default { portWrite: pushAndWriteAndDrainMulti };
