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

export const uuid = () => Math.random().toString(36).replace(/[^a-z]+/g, '').substring(0, 2)