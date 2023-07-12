import "dotenv/config";
import TelegramBot from "node-telegram-bot-api";
import { portWrite } from "./serial.ts";

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
    const cmd = match ? match[1] : "";

    return portWrite(cmd);
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
