import { initEventDB, pushEventDB, pushEventDBAndWrite, initInfoDB } from "./db.js";

initEventDB();

const infoDBObj = initInfoDB();

const devices = infoDBObj.devices.data;
const doors = infoDBObj.doors.data;

export const handleFingerScan = (_queryFingerId) => {
  let resp = "noid";
  const _queryUserId = device.fingerIds.find((_idObj) => _idObj.fingerId === _queryFingerId)?.userId;
  if (_queryUserId) {
    resp = "nodoor";
    const _permissions = doors.find((d) => d.deviceId === device.id)?.permissions;
    if (_permissions) {
      resp = "denied";
      const _user = _permissions.find((_pObj) => _pObj.userId === _queryUserId);
      if (_user) {
        resp = "granted";
        console.log(`user: ${_user.userId}, position: ${device.name} permission granted`);
      } else {
        console.log(`user: ${_user.userId}, position: ${device.name} permission denied`);
      }
    }
  }
  return resp;
};

const handleFingerEnrollSuccess = (_device, _fingerId, _userId) => {
  const [_d, _i, _fn, _cmd, _value] = dataArr;
  const device = devices.find((dv) => dv.d === _d && dv.i === _i);

  let _res = `${_d},${_i},${_fn},${_cmd},err,noid`;

  return _res;
};

export default { fsc: handleFingerScan, fes: handleFingerEnrollSuccess };
