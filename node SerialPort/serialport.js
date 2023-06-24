const { SerialPort } = require('serialport')
const { ReadlineParser } = require('@serialport/parser-readline')

const port = new SerialPort({ path: '/dev/serial0', baudRate: 9600 }, function (err) {
  if (err) {
    return console.log('Error: ', err.message)
  }
})

// Creating the parser and piping can be shortened to
const parser = port.pipe(new ReadlineParser())

port.pipe(parser)
parser.on('data', console.log)
port.write('ROBOT PLEASE RESPOND\n')

port.write('turn on', function(err) {
  if (err) {
    return console.log('Error on write: ', err.message)
  }
  console.log('message written')
})

// Open errors will be emitted as an error event
port.on('error', function(err) {
  console.log('Error: ', err.message)
})