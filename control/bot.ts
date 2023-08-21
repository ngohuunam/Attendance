import * as dotenv from 'dotenv'
dotenv.config()

import TelegramBot from "node-telegram-bot-api";
import { uartSendRaw } from 'monitor';

let bot: TelegramBot;

const chatID = process.env.CHAT_ID;

const initBot = () => {
  if (bot) return bot;

  // replace the value below with the Telegram token you receive from @BotFather
  const token = process.env.BOT_TOKEN;

  // Create a bot that uses 'polling' to fetch new updates
  bot = new TelegramBot(token, { polling: true });

  bot.onText(/\/cmd (.+)/, (msg, match) => {
    // 'msg' is the received Message from Telegram
    // 'match' is the result of executing the regexp above on the text content
    // of the message

    // const chatId = msg.chat.id;
    if (match) {
      // enroll r1 21
      let raw = match[1];
      const splits = raw.split(" ")
      const cmd = splits[0]
      const device = splits[1]
      const value = splits[2]

      if (cmd === "enroll") {
        // enroll r1 21  
        raw = `${device},f,${cmd},000,${value}`
      } else if (cmd === "restart") {
        // restart r1
        raw = `${device},r,${cmd},000`
      } else if (cmd === "open") {
        switch (device) {
          case "main":
            raw = `h0,rl,push,000,1`
            break
        }
      } else if (cmd === "off") {
        switch (device) {
          case "main":
            raw = `h0,rl,normal,000,3`
            break
        }
      } else if (cmd === "on") {
        switch (device) {
          case "main":
            raw = `h0,rl,trigger,000,3`
            break
        }
      }
      uartSendRaw(raw)
    }
    // send back the matched "whatever" to the chat
    // bot.sendMessage(chatID, resp);
  });

  // Listen for any kind of message. There are different kinds of
  // messages.
  bot.on("message", (msg) => {
    const chatId = msg.chat.id;
    console.log("🚀 ~ file: bot.ts:35 ~ bot.on ~ chatId:", msg.chat.id)

    // send a message to the chat acknowledging receipt of their message
    if (msg?.text === "chatid") bot.sendMessage(chatId, "chatId: " + chatId);
  });

  return bot;
};

export const botSend = (mess: string) => {
  if (!bot) initBot();
  bot.sendMessage(chatID, mess);
};

export default initBot;
