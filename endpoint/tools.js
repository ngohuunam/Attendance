export const runAtSpecificTimeOfDay = (hour, minutes, func) => {
  const twentyFourHours = 86400000;
  const now = new Date();
  let eta_ms = new Date(now.getFullYear(), now.getMonth(), now.getDate(), hour, minutes, 0, 0).getTime() - now;
  if (eta_ms < 0) {
    eta_ms += twentyFourHours;
  }
  let _timerID = setTimeout(function () {
    //run once
    func();
    // run every 24 hours from now on
    let _timerID = setInterval(func, twentyFourHours);
  }, eta_ms);

  return _timerID;
};
