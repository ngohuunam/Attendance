export type RunAtSpecificTimeOfDayTimerID_T = { interval: 0 | NodeJS.Timer, out: 0 | NodeJS.Timeout }

export const runAtSpecificTimeOfDay = (hour: number, minutes: number, func: () => void, _timerID: RunAtSpecificTimeOfDayTimerID_T) => {
  if (!!_timerID.out || !!_timerID.interval) return;
  const twentyFourHours = 86400000;
  const now = new Date();
  let eta_ms = new Date(now.getFullYear(), now.getMonth(), now.getDate(), hour, minutes, 0, 0).getTime() - now.getTime();
  if (eta_ms < 0) {
    eta_ms += twentyFourHours;
  }

  _timerID.out = setTimeout(function () {
    //run once
    func();
    // run every 24 hours from now on
    _timerID.interval = setInterval(function () {
      clearTimeout(_timerID.out);
      _timerID.out = 0;
      func();
    }, twentyFourHours);
  }, eta_ms);

  // return _timerID;
};

interface ProbExists_I { (obj: { [k: string]: any }, path: string): boolean }

export const propExists: ProbExists_I = (obj, path) => Boolean(path.split(".").reduce((p, c) => p?.[c], obj));

export const stringSentenceCase = (str: string) => str.replace(/\.\s+([a-z])[^\.]|^(\s*[a-z])[^\.]/g, (s) => s.replace(/([a-z])/, (s) => s.toUpperCase()));

export const uuid = () => Math.random().toString(36).replace(/[^a-z]+/g, '').substring(0, 3)

export const uartDataProps = ["deviceId", "func", "cmd", "cmdId", "value", "err", "source"] as const
export const deviceIDs = ["h0", "r1", "r2", "r3", "r4"] as const
export const funcs = ["r", "f", "q", "o", "l", "rl"] as const

export type UartDataProps_T = typeof uartDataProps[number]
export type UartDataBase_T = Record<UartDataProps_T, string>

export type DeviceIDs_T = typeof deviceIDs[number]
export type Funcs_T = typeof funcs[number]
export type UartData_T = UartDataBase_T & {
  deviceId: DeviceIDs_T,
  func: Funcs_T
}

export type CmdTable_T = typeof cmdTable

export type CmdTable_Fn_T = keyof CmdTable_T;
export type UartFnCmd_T<T extends keyof CmdTable_T> = CmdTable_T[T]

export const cmdTable = {
  oled: {
    on: { func: 'o', cmd: 'on' },
    off: { func: 'o', cmd: 'off' },
    draw: { func: 'o', cmd: 'off', value: '_' },
    clear: { func: 'o', cmd: 'off' },
    flip: { func: 'o', cmd: 'flip' },
  },
  reader: {
    ping: { func: 'r', cmd: 'ping' },
    reset: { func: 'r', cmd: 'reset' },
    debug_on: { func: 'r', cmd: 'debug', value: 'on' },
    debug_off: { func: 'r', cmd: 'debug', value: 'off' },
  },
  qr: {
    ping: { func: 'q', cmd: 'ping' },
    on: { func: 'q', cmd: 'on' },
    off: { func: 'q', cmd: 'off' },
    user: { func: 'q', cmd: 'user' },
    normal: { func: 'q', cmd: 'normal' },
  },
  finger: {
    ping: { func: 'f', cmd: 'ping' },
    enroll: { func: 'f', cmd: 'enroll' },
    delete: { func: 'f', cmd: 'delete' },
    empty: { func: 'f', cmd: 'empty' },
    getid: { func: 'f', cmd: 'getid' },
    on: { func: 'f', cmd: 'on' },
    off: { func: 'f', cmd: 'off' },
    slow: { func: 'f', cmd: 'slow' },
    normal: { func: 'f', cmd: 'normal' },
  },
  led: {
    on: { func: 'l', cmd: 'on' },
    off: { func: 'l', cmd: 'off' },
    breath: { func: 'l', cmd: 'breath' },
    breath_red: { func: 'l', cmd: 'breath', value: 'red' },
    breath_blue: { func: 'l', cmd: 'breath', value: 'blue', },
    breath_green: { func: 'l', cmd: 'breath', value: 'green', },
    breath_yellow: { func: 'l', cmd: 'breath', value: 'yellow', },
    ok: { func: 'l', cmd: 'notify', value: 'ok', },
    error: { func: 'l', cmd: 'notify', value: 'error', },
    wait: { func: 'l', cmd: 'wait', value: 1000 },
    ms: { func: 'l', cmd: 'ms', value: 100, },
    flash: { func: 'l', cmd: 'flash', },
    flash_red: { func: 'l', cmd: 'flash', value: 'red', },
    flash_blue: { func: 'l', cmd: 'flash', value: 'blue', },
    flash_green: { func: 'l', cmd: 'flash', value: 'green', },
    flash_yellow: { func: 'l', cmd: 'flash', value: 'yellow', },
    red: { func: 'l', cmd: 'red', },
    blue: { func: 'l', cmd: 'blue', },
    green: { func: 'l', cmd: 'green', },
    yellow: { func: 'l', cmd: 'yellow', },
  } as const,
}