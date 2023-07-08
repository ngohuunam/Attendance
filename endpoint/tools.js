export const runAtSpecificTimeOfDay = (hour, minutes, func, _timerID) => {
  if (!!_timerID.out || !!_timerID.interval) return;
  const twentyFourHours = 86400000;
  const now = new Date();
  let eta_ms = new Date(now.getFullYear(), now.getMonth(), now.getDate(), hour, minutes, 0, 0).getTime() - now;
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

export const propExists = (obj, path) => path.split(".").reduce((p, c) => p?.[c], obj);

export const stringSentenceCase = (str) => str.replace(/\.\s+([a-z])[^\.]|^(\s*[a-z])[^\.]/g, (s) => s.replace(/([a-z])/, (s) => s.toUpperCase()));
