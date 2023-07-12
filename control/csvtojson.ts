import fs from "fs";

import { join, dirname } from "node:path"
import { fileURLToPath } from "node:url"

const __filename = fileURLToPath(import.meta.url)

const __dirname = dirname(__filename)

const readCSV = (filepath: string, separator = ",") => {
  /** Reads a csv file, taking into consideration linebreaks inside of fields, and double quotes or no quotes.
   * Converts it into a json object
   */
  const fp = join(__dirname, `${filepath}.csv`)
  const file = fs.readFileSync(fp, { encoding: "utf-8" });
  console.log("🚀 ~ file: csvtojson.ts:16 ~ readCSV ~ file:", file)

  // Figure out how many cells there are by counting the first line. 
  // ATTENTION: If your header contains commas or a linebreak, this will fail.
  const firstLineBreak = file.indexOf("\n");
  const rowsNum = file.slice(0, firstLineBreak).split(",").length;
  console.log("🚀 ~ file: csvtojson.ts:22 ~ readCSV ~ rowsNum:", rowsNum)

  // Construct a regex based on how many headers there are
  const singleCellRegex = `(?:(?:"([\\s\\S]*?)")|((?:(?:[^"${separator}\\n])|(?:""))+))`;
  let regexText = "";

  for (let i = 0; i < rowsNum; i++) {
    regexText += "," + singleCellRegex;
  }

  const regex = new RegExp(regexText.slice(1), "g");
  const results = file.matchAll(regex);

  const rowsArr = [];
  for (const row of results) {
    const newRow = [];

    for (let i = 0; i < rowsNum; i++) {
      const rowValue = row[2 * i + 1] ?? row[2 * i + 2];
      console.log("🚀 ~ file: csvtojson.ts:40 ~ readCSV ~ rowValue:", rowValue)
      newRow.push(rowValue.replaceAll('""', '"')); // Remove double double quotes
    }

    rowsArr.push(newRow);
  }

  const headers = rowsArr[0];
  console.log("🚀 ~ file: csvtojson.ts:47 ~ readCSV ~  rowsArr:",  rowsArr)
  console.log("🚀 ~ file: csvtojson.ts:47 ~ readCSV ~ headers:", headers)
  const rows = rowsArr.slice(1);
  console.log("🚀 ~ file: csvtojson.ts:48 ~ readCSV ~ rows:", rows)

  return rows.map((row) =>
    row.reduce((jsonRow, field, idx) => {
      jsonRow[headers[idx]] = field;
      console.log("🚀 ~ file: csvtojson.ts:51 ~ row.reduce ~ jsonRow:", jsonRow)
      return jsonRow;
    }, {} as { [k: typeof headers[number]]: string })
  );
}

export default readCSV

import { LowSync } from "lowdb"
import { JSONFileSync } from "lowdb/node"

const csvToJsonDB = (pathFileNoExt: string) => {
  const jsonfile = join(__dirname, `${pathFileNoExt}.json`)

  const _adapter = new JSONFileSync<{ [k: string]: string }[]>(jsonfile)

  // const csvFilePath = join(__dirname, `${pathFileNoExt}.csv`)

  const json = readCSV(pathFileNoExt, ",");

  const jsonDb = new LowSync<{ [k: string]: string }[]>(_adapter, [])

  jsonDb.read()
  jsonDb.data = [...json]
  jsonDb.write()

}

csvToJsonDB("./users");