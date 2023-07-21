declare global {
  namespace NodeJS {
    interface ProcessEnv {
      NODE_ENV: 'development' | 'production';
      PORT: string;
      BOT_TOKEN: string;
      CHAT_ID: string;
      SERIAL_PATH: string;
      SERIAL_BAUDRATE: string;
      SERIAL_DELIMITER: string;
    }
  }
}

// If this file has no import/export statements (i.e. is a script)
// convert it into a module by adding an empty export statement.
export {}