// Created by http://oleddisplay.squix.ch/ Consider a donation
// In case of problems make sure that you are using the font file with the correct version!
const uint8_t Roboto[] PROGMEM = {
  0x08, // Width: 8
  0x0C, // Height: 12
  0x20, // First Char: 32
  0xE0, // Numbers of Chars: 224

  // Jump Table:
  0xFF, 0xFF, 0x00, 0x02,  // 32:65535
  0x00, 0x00, 0x04, 0x02,  // 33:0
  0x00, 0x04, 0x03, 0x03,  // 34:4
  0x00, 0x07, 0x09, 0x05,  // 35:7
  0x00, 0x10, 0x08, 0x05,  // 36:16
  0x00, 0x18, 0x0C, 0x07,  // 37:24
  0x00, 0x24, 0x0C, 0x06,  // 38:36
  0x00, 0x30, 0x01, 0x02,  // 39:48
  0x00, 0x31, 0x06, 0x03,  // 40:49
  0x00, 0x37, 0x04, 0x03,  // 41:55
  0x00, 0x3B, 0x07, 0x04,  // 42:59
  0x00, 0x42, 0x09, 0x05,  // 43:66
  0x00, 0x4B, 0x02, 0x02,  // 44:75
  0x00, 0x4D, 0x03, 0x03,  // 45:77
  0x00, 0x50, 0x04, 0x02,  // 46:80
  0x00, 0x54, 0x05, 0x04,  // 47:84
  0x00, 0x59, 0x08, 0x05,  // 48:89
  0x00, 0x61, 0x06, 0x05,  // 49:97
  0x00, 0x67, 0x0A, 0x05,  // 50:103
  0x00, 0x71, 0x08, 0x05,  // 51:113
  0x00, 0x79, 0x09, 0x05,  // 52:121
  0x00, 0x82, 0x09, 0x05,  // 53:130
  0x00, 0x8B, 0x09, 0x05,  // 54:139
  0x00, 0x94, 0x09, 0x05,  // 55:148
  0x00, 0x9D, 0x09, 0x05,  // 56:157
  0x00, 0xA6, 0x07, 0x05,  // 57:166
  0x00, 0xAD, 0x04, 0x02,  // 58:173
  0x00, 0xB1, 0x02, 0x02,  // 59:177
  0x00, 0xB3, 0x07, 0x05,  // 60:179
  0x00, 0xBA, 0x07, 0x05,  // 61:186
  0x00, 0xC1, 0x07, 0x05,  // 62:193
  0x00, 0xC8, 0x07, 0x04,  // 63:200
  0x00, 0xCF, 0x0F, 0x08,  // 64:207
  0x00, 0xDE, 0x0C, 0x06,  // 65:222
  0x00, 0xEA, 0x0A, 0x06,  // 66:234
  0x00, 0xF4, 0x09, 0x06,  // 67:244
  0x00, 0xFD, 0x09, 0x06,  // 68:253
  0x01, 0x06, 0x0A, 0x05,  // 69:262
  0x01, 0x10, 0x09, 0x05,  // 70:272
  0x01, 0x19, 0x0A, 0x06,  // 71:281
  0x01, 0x23, 0x0C, 0x06,  // 72:291
  0x01, 0x2F, 0x04, 0x02,  // 73:303
  0x01, 0x33, 0x08, 0x05,  // 74:307
  0x01, 0x3B, 0x0C, 0x06,  // 75:315
  0x01, 0x47, 0x0A, 0x05,  // 76:327
  0x01, 0x51, 0x0E, 0x08,  // 77:337
  0x01, 0x5F, 0x0A, 0x06,  // 78:351
  0x01, 0x69, 0x09, 0x06,  // 79:361
  0x01, 0x72, 0x09, 0x06,  // 80:370
  0x01, 0x7B, 0x0A, 0x06,  // 81:379
  0x01, 0x85, 0x0A, 0x06,  // 82:389
  0x01, 0x8F, 0x09, 0x05,  // 83:399
  0x01, 0x98, 0x09, 0x05,  // 84:408
  0x01, 0xA1, 0x09, 0x06,  // 85:417
  0x01, 0xAA, 0x09, 0x06,  // 86:426
  0x01, 0xB3, 0x0F, 0x08,  // 87:435
  0x01, 0xC2, 0x0A, 0x06,  // 88:450
  0x01, 0xCC, 0x09, 0x05,  // 89:460
  0x01, 0xD5, 0x0A, 0x05,  // 90:469
  0x01, 0xDF, 0x04, 0x02,  // 91:479
  0x01, 0xE3, 0x06, 0x04,  // 92:483
  0x01, 0xE9, 0x02, 0x02,  // 93:489
  0x01, 0xEB, 0x05, 0x04,  // 94:491
  0x01, 0xF0, 0x08, 0x04,  // 95:496
  0x01, 0xF8, 0x03, 0x03,  // 96:504
  0x01, 0xFB, 0x08, 0x05,  // 97:507
  0x02, 0x03, 0x09, 0x05,  // 98:515
  0x02, 0x0C, 0x08, 0x05,  // 99:524
  0x02, 0x14, 0x08, 0x05,  // 100:532
  0x02, 0x1C, 0x08, 0x05,  // 101:540
  0x02, 0x24, 0x05, 0x03,  // 102:548
  0x02, 0x29, 0x08, 0x05,  // 103:553
  0x02, 0x31, 0x08, 0x05,  // 104:561
  0x02, 0x39, 0x04, 0x02,  // 105:569
  0x02, 0x3D, 0x02, 0x02,  // 106:573
  0x02, 0x3F, 0x08, 0x04,  // 107:575
  0x02, 0x47, 0x04, 0x02,  // 108:583
  0x02, 0x4B, 0x0E, 0x08,  // 109:587
  0x02, 0x59, 0x08, 0x05,  // 110:601
  0x02, 0x61, 0x09, 0x05,  // 111:609
  0x02, 0x6A, 0x09, 0x05,  // 112:618
  0x02, 0x73, 0x08, 0x05,  // 113:627
  0x02, 0x7B, 0x05, 0x03,  // 114:635
  0x02, 0x80, 0x08, 0x05,  // 115:640
  0x02, 0x88, 0x06, 0x03,  // 116:648
  0x02, 0x8E, 0x08, 0x05,  // 117:654
  0x02, 0x96, 0x07, 0x04,  // 118:662
  0x02, 0x9D, 0x0B, 0x07,  // 119:669
  0x02, 0xA8, 0x08, 0x04,  // 120:680
  0x02, 0xB0, 0x07, 0x04,  // 121:688
  0x02, 0xB7, 0x08, 0x04,  // 122:695
  0x02, 0xBF, 0x06, 0x03,  // 123:703
  0x02, 0xC5, 0x04, 0x02,  // 124:709
  0x02, 0xC9, 0x05, 0x03,  // 125:713
  0x02, 0xCE, 0x09, 0x06,  // 126:718
  0x02, 0xD7, 0x08, 0x04,  // 127:727
  0x02, 0xDF, 0x08, 0x04,  // 128:735
  0x02, 0xE7, 0x08, 0x04,  // 129:743
  0x02, 0xEF, 0x08, 0x04,  // 130:751
  0x02, 0xF7, 0x08, 0x04,  // 131:759
  0x02, 0xFF, 0x08, 0x04,  // 132:767
  0x03, 0x07, 0x08, 0x04,  // 133:775
  0x03, 0x0F, 0x08, 0x04,  // 134:783
  0x03, 0x17, 0x08, 0x04,  // 135:791
  0x03, 0x1F, 0x08, 0x04,  // 136:799
  0x03, 0x27, 0x08, 0x04,  // 137:807
  0x03, 0x2F, 0x08, 0x04,  // 138:815
  0x03, 0x37, 0x08, 0x04,  // 139:823
  0x03, 0x3F, 0x08, 0x04,  // 140:831
  0x03, 0x47, 0x08, 0x04,  // 141:839
  0x03, 0x4F, 0x08, 0x04,  // 142:847
  0x03, 0x57, 0x08, 0x04,  // 143:855
  0x03, 0x5F, 0x08, 0x04,  // 144:863
  0x03, 0x67, 0x08, 0x04,  // 145:871
  0x03, 0x6F, 0x08, 0x04,  // 146:879
  0x03, 0x77, 0x08, 0x04,  // 147:887
  0x03, 0x7F, 0x08, 0x04,  // 148:895
  0x03, 0x87, 0x08, 0x04,  // 149:903
  0x03, 0x8F, 0x08, 0x04,  // 150:911
  0x03, 0x97, 0x08, 0x04,  // 151:919
  0x03, 0x9F, 0x08, 0x04,  // 152:927
  0x03, 0xA7, 0x08, 0x04,  // 153:935
  0x03, 0xAF, 0x08, 0x04,  // 154:943
  0x03, 0xB7, 0x08, 0x04,  // 155:951
  0x03, 0xBF, 0x08, 0x04,  // 156:959
  0x03, 0xC7, 0x08, 0x04,  // 157:967
  0x03, 0xCF, 0x08, 0x04,  // 158:975
  0x03, 0xD7, 0x08, 0x04,  // 159:983
  0xFF, 0xFF, 0x00, 0x02,  // 160:65535
  0x03, 0xDF, 0x02, 0x02,  // 161:991
  0x03, 0xE1, 0x08, 0x05,  // 162:993
  0x03, 0xE9, 0x0A, 0x05,  // 163:1001
  0x03, 0xF3, 0x0C, 0x07,  // 164:1011
  0x03, 0xFF, 0x09, 0x05,  // 165:1023
  0x04, 0x08, 0x02, 0x02,  // 166:1032
  0x04, 0x0A, 0x09, 0x05,  // 167:1034
  0x04, 0x13, 0x05, 0x04,  // 168:1043
  0x04, 0x18, 0x0D, 0x07,  // 169:1048
  0x04, 0x25, 0x05, 0x04,  // 170:1061
  0x04, 0x2A, 0x05, 0x04,  // 171:1066
  0x04, 0x2F, 0x07, 0x05,  // 172:1071
  0x04, 0x36, 0x03, 0x03,  // 173:1078
  0x04, 0x39, 0x0D, 0x07,  // 174:1081
  0x04, 0x46, 0x05, 0x04,  // 175:1094
  0x04, 0x4B, 0x05, 0x03,  // 176:1099
  0x04, 0x50, 0x08, 0x05,  // 177:1104
  0x04, 0x58, 0x05, 0x03,  // 178:1112
  0x04, 0x5D, 0x05, 0x03,  // 179:1117
  0x04, 0x62, 0x03, 0x03,  // 180:1122
  0x04, 0x65, 0x08, 0x05,  // 181:1125
  0x04, 0x6D, 0x06, 0x04,  // 182:1133
  0x04, 0x73, 0x01, 0x02,  // 183:1139
  0x04, 0x74, 0x04, 0x02,  // 184:1140
  0x04, 0x78, 0x03, 0x03,  // 185:1144
  0x04, 0x7B, 0x05, 0x04,  // 186:1147
  0x04, 0x80, 0x05, 0x04,  // 187:1152
  0x04, 0x85, 0x0B, 0x07,  // 188:1157
  0x04, 0x90, 0x0C, 0x07,  // 189:1168
  0x04, 0x9C, 0x0D, 0x07,  // 190:1180
  0x04, 0xA9, 0x08, 0x04,  // 191:1193
  0x04, 0xB1, 0x0C, 0x06,  // 192:1201
  0x04, 0xBD, 0x0C, 0x06,  // 193:1213
  0x04, 0xC9, 0x0C, 0x06,  // 194:1225
  0x04, 0xD5, 0x0C, 0x06,  // 195:1237
  0x04, 0xE1, 0x0C, 0x06,  // 196:1249
  0x04, 0xED, 0x0C, 0x06,  // 197:1261
  0x04, 0xF9, 0x10, 0x08,  // 198:1273
  0x05, 0x09, 0x09, 0x06,  // 199:1289
  0x05, 0x12, 0x0A, 0x05,  // 200:1298
  0x05, 0x1C, 0x0A, 0x05,  // 201:1308
  0x05, 0x26, 0x0A, 0x05,  // 202:1318
  0x05, 0x30, 0x0A, 0x05,  // 203:1328
  0x05, 0x3A, 0x03, 0x02,  // 204:1338
  0x05, 0x3D, 0x04, 0x02,  // 205:1341
  0x05, 0x41, 0x03, 0x02,  // 206:1345
  0x05, 0x44, 0x03, 0x02,  // 207:1348
  0x05, 0x47, 0x09, 0x06,  // 208:1351
  0x05, 0x50, 0x0A, 0x06,  // 209:1360
  0x05, 0x5A, 0x09, 0x06,  // 210:1370
  0x05, 0x63, 0x09, 0x06,  // 211:1379
  0x05, 0x6C, 0x09, 0x06,  // 212:1388
  0x05, 0x75, 0x09, 0x06,  // 213:1397
  0x05, 0x7E, 0x09, 0x06,  // 214:1406
  0x05, 0x87, 0x08, 0x05,  // 215:1415
  0x05, 0x8F, 0x0B, 0x06,  // 216:1423
  0x05, 0x9A, 0x09, 0x06,  // 217:1434
  0x05, 0xA3, 0x09, 0x06,  // 218:1443
  0x05, 0xAC, 0x09, 0x06,  // 219:1452
  0x05, 0xB5, 0x09, 0x06,  // 220:1461
  0x05, 0xBE, 0x09, 0x05,  // 221:1470
  0x05, 0xC7, 0x09, 0x05,  // 222:1479
  0x05, 0xD0, 0x09, 0x05,  // 223:1488
  0x05, 0xD9, 0x08, 0x05,  // 224:1497
  0x05, 0xE1, 0x08, 0x05,  // 225:1505
  0x05, 0xE9, 0x08, 0x05,  // 226:1513
  0x05, 0xF1, 0x08, 0x05,  // 227:1521
  0x05, 0xF9, 0x08, 0x05,  // 228:1529
  0x06, 0x01, 0x08, 0x05,  // 229:1537
  0x06, 0x09, 0x0E, 0x08,  // 230:1545
  0x06, 0x17, 0x08, 0x05,  // 231:1559
  0x06, 0x1F, 0x08, 0x05,  // 232:1567
  0x06, 0x27, 0x08, 0x05,  // 233:1575
  0x06, 0x2F, 0x08, 0x05,  // 234:1583
  0x06, 0x37, 0x08, 0x05,  // 235:1591
  0x06, 0x3F, 0x02, 0x02,  // 236:1599
  0x06, 0x41, 0x04, 0x02,  // 237:1601
  0x06, 0x45, 0x03, 0x02,  // 238:1605
  0x06, 0x48, 0x03, 0x02,  // 239:1608
  0x06, 0x4B, 0x09, 0x05,  // 240:1611
  0x06, 0x54, 0x08, 0x05,  // 241:1620
  0x06, 0x5C, 0x09, 0x05,  // 242:1628
  0x06, 0x65, 0x09, 0x05,  // 243:1637
  0x06, 0x6E, 0x09, 0x05,  // 244:1646
  0x06, 0x77, 0x09, 0x05,  // 245:1655
  0x06, 0x80, 0x09, 0x05,  // 246:1664
  0x06, 0x89, 0x09, 0x05,  // 247:1673
  0x06, 0x92, 0x09, 0x05,  // 248:1682
  0x06, 0x9B, 0x08, 0x05,  // 249:1691
  0x06, 0xA3, 0x08, 0x05,  // 250:1699
  0x06, 0xAB, 0x08, 0x05,  // 251:1707
  0x06, 0xB3, 0x08, 0x05,  // 252:1715
  0x06, 0xBB, 0x07, 0x04,  // 253:1723
  0x06, 0xC2, 0x09, 0x05,  // 254:1730
  0x06, 0xCB, 0x07, 0x04,  // 255:1739

  // Font Data:
  0x00, 0x00, 0x7C, 0x01, // 33
  0x00, 0x00, 0x0C, // 34
  0x40, 0x00, 0xF0, 0x01, 0x5C, 0x01, 0xF8, 0x00, 0x54, // 35
  0x88, 0x00, 0x14, 0x01, 0x24, 0x03, 0xFC, 0x01, // 36
  0x08, 0x00, 0x94, 0x00, 0x5C, 0x00, 0xB0, 0x00, 0x48, 0x01, 0xC0, 0x01, // 37
  0x00, 0x00, 0xD8, 0x01, 0x24, 0x01, 0x5C, 0x01, 0xC0, 0x01, 0x00, 0x01, // 38
  0x0C, // 39
  0x00, 0x00, 0xF8, 0x03, 0x04, 0x04, // 40
  0x04, 0x04, 0xF8, 0x03, // 41
  0x18, 0x00, 0x0C, 0x00, 0x18, 0x00, 0x08, // 42
  0x40, 0x00, 0x40, 0x00, 0xF0, 0x01, 0x40, 0x00, 0x40, // 43
  0x00, 0x03, // 44
  0x40, 0x00, 0x40, // 45
  0x00, 0x00, 0x00, 0x01, // 46
  0x80, 0x03, 0x70, 0x00, 0x0C, // 47
  0x00, 0x00, 0xFC, 0x01, 0x04, 0x01, 0xFC, 0x01, // 48
  0x00, 0x00, 0x08, 0x00, 0xFC, 0x01, // 49
  0x08, 0x00, 0x84, 0x01, 0x44, 0x01, 0x3C, 0x01, 0x00, 0x01, // 50
  0x88, 0x00, 0x04, 0x01, 0x24, 0x01, 0xDC, 0x01, // 51
  0xC0, 0x00, 0xB0, 0x00, 0x88, 0x00, 0xFC, 0x01, 0x80, // 52
  0x00, 0x00, 0xBC, 0x01, 0x14, 0x01, 0xB4, 0x01, 0x40, // 53
  0x00, 0x00, 0xF8, 0x01, 0x14, 0x01, 0xB4, 0x01, 0x40, // 54
  0x04, 0x00, 0x84, 0x01, 0x64, 0x00, 0x1C, 0x00, 0x04, // 55
  0xD8, 0x00, 0x64, 0x01, 0x24, 0x01, 0x5C, 0x01, 0x80, // 56
  0x38, 0x00, 0x44, 0x01, 0x44, 0x01, 0xFC, // 57
  0x00, 0x00, 0x10, 0x01, // 58
  0x10, 0x03, // 59
  0x40, 0x00, 0x60, 0x00, 0x60, 0x00, 0x90, // 60
  0x00, 0x00, 0xA0, 0x00, 0xA0, 0x00, 0xA0, // 61
  0x00, 0x00, 0x90, 0x00, 0x60, 0x00, 0x60, // 62
  0x08, 0x00, 0x44, 0x01, 0x24, 0x00, 0x18, // 63
  0xE0, 0x01, 0x10, 0x02, 0xC8, 0x04, 0x28, 0x05, 0xE8, 0x05, 0x08, 0x01, 0x90, 0x01, 0x60, // 64
  0x00, 0x01, 0xF0, 0x00, 0x8C, 0x00, 0xB8, 0x00, 0xC0, 0x00, 0x00, 0x01, // 65
  0x00, 0x00, 0xFC, 0x01, 0x24, 0x01, 0x24, 0x01, 0xDC, 0x01, // 66
  0x00, 0x00, 0xF8, 0x00, 0x04, 0x01, 0x04, 0x01, 0xD8, // 67
  0x00, 0x00, 0xFC, 0x01, 0x04, 0x01, 0x04, 0x01, 0xF8, // 68
  0x00, 0x00, 0xFC, 0x01, 0x24, 0x01, 0x24, 0x01, 0x04, 0x01, // 69
  0x00, 0x00, 0xFC, 0x01, 0x24, 0x00, 0x24, 0x00, 0x04, // 70
  0x00, 0x00, 0xF8, 0x00, 0x04, 0x01, 0x44, 0x01, 0xCC, 0x01, // 71
  0x00, 0x00, 0xFC, 0x01, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0xFC, 0x01, // 72
  0x00, 0x00, 0xFC, 0x01, // 73
  0x80, 0x00, 0x00, 0x01, 0x00, 0x01, 0xFC, 0x01, // 74
  0x00, 0x00, 0xFC, 0x01, 0x30, 0x00, 0x48, 0x00, 0x84, 0x01, 0x00, 0x01, // 75
  0x00, 0x00, 0xFC, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, // 76
  0x00, 0x00, 0xFC, 0x01, 0x60, 0x00, 0x80, 0x01, 0xE0, 0x00, 0x10, 0x00, 0xFC, 0x01, // 77
  0xFC, 0x01, 0x08, 0x00, 0x30, 0x00, 0xC0, 0x00, 0xFC, 0x01, // 78
  0x70, 0x00, 0x88, 0x00, 0x04, 0x01, 0x04, 0x01, 0xF8, // 79
  0x00, 0x00, 0xFC, 0x01, 0x44, 0x00, 0x44, 0x00, 0x7C, // 80
  0x70, 0x00, 0x88, 0x00, 0x04, 0x01, 0x04, 0x01, 0xF8, 0x02, // 81
  0x00, 0x00, 0xFC, 0x01, 0x44, 0x00, 0xC4, 0x00, 0x38, 0x01, // 82
  0x98, 0x00, 0x24, 0x01, 0x24, 0x01, 0x24, 0x01, 0xC8, // 83
  0x04, 0x00, 0x04, 0x00, 0xFC, 0x01, 0x04, 0x00, 0x04, // 84
  0x00, 0x00, 0xFC, 0x01, 0x00, 0x01, 0x00, 0x01, 0xFC, // 85
  0x0C, 0x00, 0x70, 0x00, 0x80, 0x01, 0xE0, 0x00, 0x1C, // 86
  0x1C, 0x00, 0x60, 0x00, 0xE0, 0x01, 0x1C, 0x00, 0x78, 0x00, 0x80, 0x01, 0xF8, 0x00, 0x04, // 87
  0x04, 0x01, 0xD8, 0x00, 0x20, 0x00, 0xD8, 0x00, 0x04, 0x01, // 88
  0x04, 0x00, 0x18, 0x00, 0xE0, 0x01, 0x18, 0x00, 0x04, // 89
  0x80, 0x01, 0x44, 0x01, 0x24, 0x01, 0x1C, 0x01, 0x04, 0x01, // 90
  0x00, 0x00, 0xFC, 0x03, // 91
  0x0C, 0x00, 0x70, 0x00, 0x80, 0x03, // 92
  0xFC, 0x03, // 93
  0x10, 0x00, 0x0C, 0x00, 0x18, // 94
  0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, // 95
  0x02, 0x00, 0x04, // 96
  0x00, 0x00, 0xD0, 0x01, 0x50, 0x01, 0xF0, 0x01, // 97
  0x00, 0x00, 0xFC, 0x01, 0x10, 0x01, 0xB0, 0x01, 0x40, // 98
  0x00, 0x00, 0xF0, 0x01, 0x10, 0x01, 0xB0, 0x01, // 99
  0xE0, 0x00, 0x10, 0x01, 0x10, 0x01, 0xFC, 0x01, // 100
  0x00, 0x00, 0xF0, 0x01, 0x50, 0x01, 0x70, 0x01, // 101
  0x10, 0x00, 0xFC, 0x01, 0x14, // 102
  0xE0, 0x00, 0x10, 0x05, 0x10, 0x05, 0xF0, 0x07, // 103
  0x00, 0x00, 0xFC, 0x01, 0x10, 0x00, 0xF0, 0x01, // 104
  0x00, 0x00, 0xF4, 0x01, // 105
  0xF4, 0x07, // 106
  0x00, 0x00, 0xFC, 0x01, 0xA0, 0x00, 0x10, 0x01, // 107
  0x00, 0x00, 0xFC, 0x01, // 108
  0x00, 0x00, 0xF0, 0x01, 0x10, 0x00, 0xD0, 0x01, 0x20, 0x00, 0x10, 0x00, 0xF0, 0x01, // 109
  0x00, 0x00, 0xF0, 0x01, 0x10, 0x00, 0xF0, 0x01, // 110
  0xE0, 0x00, 0x10, 0x01, 0x10, 0x01, 0xB0, 0x01, 0x40, // 111
  0x00, 0x00, 0xF0, 0x07, 0x10, 0x01, 0xB0, 0x01, 0x40, // 112
  0xE0, 0x00, 0x10, 0x01, 0x10, 0x01, 0xF0, 0x07, // 113
  0x00, 0x00, 0xF0, 0x01, 0x10, // 114
  0x00, 0x00, 0x30, 0x01, 0x50, 0x01, 0xD0, 0x01, // 115
  0x10, 0x00, 0xF8, 0x01, 0x10, 0x01, // 116
  0x00, 0x00, 0xF0, 0x01, 0x00, 0x01, 0xF0, 0x01, // 117
  0x70, 0x00, 0x80, 0x01, 0xE0, 0x00, 0x10, // 118
  0x70, 0x00, 0x80, 0x01, 0xE0, 0x00, 0x70, 0x00, 0x80, 0x01, 0xF0, // 119
  0x10, 0x01, 0xE0, 0x00, 0xA0, 0x00, 0x10, 0x01, // 120
  0x70, 0x04, 0x80, 0x07, 0xE0, 0x00, 0x10, // 121
  0x90, 0x01, 0x50, 0x01, 0x30, 0x01, 0x10, 0x01, // 122
  0x40, 0x00, 0xB8, 0x03, 0x04, 0x04, // 123
  0x00, 0x00, 0xFC, 0x03, // 124
  0x04, 0x04, 0xB8, 0x03, 0x40, // 125
  0x00, 0x00, 0xC0, 0x00, 0x40, 0x00, 0x80, 0x00, 0xC0, // 126
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 127
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 128
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 129
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 130
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 131
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 132
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 133
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 134
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 135
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 136
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 137
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 138
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 139
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 140
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 141
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 142
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 143
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 144
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 145
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 146
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 147
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 148
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 149
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 150
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 151
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 152
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 153
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 154
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 155
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 156
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 157
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 158
  0xFC, 0x01, 0x74, 0x01, 0x94, 0x01, 0xFC, 0x01, // 159
  0xD0, 0x07, // 161
  0x00, 0x00, 0xF0, 0x01, 0x18, 0x03, 0xB0, 0x01, // 162
  0x20, 0x01, 0xF8, 0x01, 0x24, 0x01, 0x04, 0x01, 0x08, 0x01, // 163
  0x00, 0x00, 0xF8, 0x01, 0x08, 0x01, 0x08, 0x01, 0x08, 0x01, 0xF8, 0x01, // 164
  0x04, 0x00, 0xD8, 0x00, 0xE0, 0x01, 0xDC, 0x00, 0x04, // 165
  0xBC, 0x03, // 166
  0x68, 0x02, 0x94, 0x04, 0x14, 0x05, 0x24, 0x07, 0xC8, // 167
  0x00, 0x00, 0x04, 0x00, 0x04, // 168
  0x70, 0x00, 0xA8, 0x00, 0x54, 0x01, 0x54, 0x01, 0x54, 0x01, 0xD8, 0x00, 0x20, // 169
  0x00, 0x00, 0x34, 0x00, 0x3C, // 170
  0x40, 0x00, 0xE0, 0x00, 0xA0, // 171
  0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0xC0, // 172
  0x40, 0x00, 0x40, // 173
  0x70, 0x00, 0x88, 0x00, 0x74, 0x01, 0x34, 0x01, 0x54, 0x01, 0xD8, 0x00, 0x20, // 174
  0x00, 0x00, 0x08, 0x00, 0x08, // 175
  0x08, 0x00, 0x14, 0x00, 0x08, // 176
  0x20, 0x00, 0x20, 0x01, 0x78, 0x01, 0x20, 0x01, // 177
  0x24, 0x00, 0x34, 0x00, 0x28, // 178
  0x24, 0x00, 0x2C, 0x00, 0x14, // 179
  0x00, 0x00, 0x06, // 180
  0x00, 0x00, 0xF0, 0x07, 0x00, 0x01, 0xF0, 0x01, // 181
  0x18, 0x00, 0x3C, 0x00, 0xFC, 0x01, // 182
  0x20, // 183
  0x00, 0x07, 0x00, 0x02, // 184
  0x00, 0x00, 0x3C, // 185
  0x00, 0x00, 0x3C, 0x00, 0x3C, // 186
  0xA0, 0x00, 0xE0, 0x00, 0x40, // 187
  0x00, 0x00, 0xBC, 0x00, 0x60, 0x00, 0xD0, 0x00, 0xE8, 0x01, 0x80, // 188
  0x00, 0x00, 0xBC, 0x00, 0x40, 0x00, 0x30, 0x01, 0xA8, 0x01, 0x40, 0x01, // 189
  0x24, 0x00, 0x2C, 0x00, 0x94, 0x00, 0x60, 0x00, 0xD0, 0x00, 0xE8, 0x01, 0x80, // 190
  0x00, 0x03, 0xD0, 0x04, 0x00, 0x04, 0x00, 0x02, // 191
  0x00, 0x01, 0xF0, 0x00, 0x8D, 0x00, 0xB8, 0x00, 0xC0, 0x00, 0x00, 0x01, // 192
  0x00, 0x01, 0xF0, 0x00, 0x8D, 0x00, 0xB8, 0x00, 0xC0, 0x00, 0x00, 0x01, // 193
  0x00, 0x01, 0xF1, 0x00, 0x8C, 0x00, 0xB9, 0x00, 0xC0, 0x00, 0x00, 0x01, // 194
  0x00, 0x01, 0xF1, 0x00, 0x8C, 0x00, 0xB9, 0x00, 0xC0, 0x00, 0x00, 0x01, // 195
  0x00, 0x01, 0xF2, 0x00, 0x8C, 0x00, 0xBA, 0x00, 0xC0, 0x00, 0x00, 0x01, // 196
  0x00, 0x01, 0xF0, 0x00, 0x8E, 0x00, 0xB8, 0x00, 0xC0, 0x00, 0x00, 0x01, // 197
  0x00, 0x01, 0xC0, 0x00, 0xB0, 0x00, 0x88, 0x00, 0xFC, 0x01, 0x24, 0x01, 0x24, 0x01, 0x04, 0x01, // 198
  0x00, 0x00, 0xF8, 0x00, 0x04, 0x07, 0x04, 0x03, 0xD8, // 199
  0x00, 0x00, 0xFC, 0x01, 0x25, 0x01, 0x24, 0x01, 0x04, 0x01, // 200
  0x00, 0x00, 0xFC, 0x01, 0x25, 0x01, 0x24, 0x01, 0x04, 0x01, // 201
  0x00, 0x00, 0xFD, 0x01, 0x24, 0x01, 0x25, 0x01, 0x04, 0x01, // 202
  0x00, 0x00, 0xFE, 0x01, 0x24, 0x01, 0x26, 0x01, 0x04, 0x01, // 203
  0xFC, 0x01, 0x01, // 204
  0x00, 0x00, 0xFD, 0x01, // 205
  0xFD, 0x01, 0x01, // 206
  0xFE, 0x01, 0x02, // 207
  0xF8, 0x00, 0x24, 0x01, 0x24, 0x01, 0x04, 0x01, 0xF8, // 208
  0xFC, 0x01, 0x09, 0x00, 0x30, 0x00, 0xC1, 0x00, 0xFC, 0x01, // 209
  0x70, 0x00, 0x88, 0x00, 0x05, 0x01, 0x04, 0x01, 0xF8, // 210
  0x70, 0x00, 0x88, 0x00, 0x05, 0x01, 0x04, 0x01, 0xF8, // 211
  0x70, 0x00, 0x89, 0x00, 0x04, 0x01, 0x05, 0x01, 0xF8, // 212
  0x70, 0x00, 0x89, 0x00, 0x04, 0x01, 0x05, 0x01, 0xF8, // 213
  0x70, 0x00, 0x8A, 0x00, 0x04, 0x01, 0x06, 0x01, 0xF8, // 214
  0x20, 0x01, 0xC0, 0x00, 0xC0, 0x00, 0x20, 0x01, // 215
  0xF0, 0x00, 0x88, 0x01, 0x64, 0x01, 0x14, 0x01, 0xFC, 0x00, 0x04, // 216
  0x00, 0x00, 0xFC, 0x01, 0x01, 0x01, 0x00, 0x01, 0xFC, // 217
  0x00, 0x00, 0xFC, 0x01, 0x01, 0x01, 0x00, 0x01, 0xFC, // 218
  0x00, 0x00, 0xFD, 0x01, 0x00, 0x01, 0x01, 0x01, 0xFC, // 219
  0x00, 0x00, 0xFE, 0x01, 0x00, 0x01, 0x02, 0x01, 0xFC, // 220
  0x04, 0x00, 0x18, 0x00, 0xE1, 0x01, 0x18, 0x00, 0x04, // 221
  0x00, 0x00, 0xFC, 0x01, 0x88, 0x00, 0xD8, 0x00, 0x20, // 222
  0x00, 0x00, 0xFC, 0x01, 0x34, 0x01, 0x48, 0x01, 0x80, // 223
  0x00, 0x00, 0xD2, 0x01, 0x54, 0x01, 0xF0, 0x01, // 224
  0x00, 0x00, 0xD0, 0x01, 0x54, 0x01, 0xF2, 0x01, // 225
  0x00, 0x00, 0xD4, 0x01, 0x52, 0x01, 0xF6, 0x01, // 226
  0x00, 0x00, 0xD4, 0x01, 0x52, 0x01, 0xF6, 0x01, // 227
  0x00, 0x00, 0xD4, 0x01, 0x50, 0x01, 0xF4, 0x01, // 228
  0x00, 0x00, 0xD0, 0x01, 0x56, 0x01, 0xF0, 0x01, // 229
  0x00, 0x00, 0xD0, 0x01, 0x50, 0x01, 0xF0, 0x01, 0x50, 0x01, 0x50, 0x01, 0x70, 0x01, // 230
  0x00, 0x00, 0xF0, 0x01, 0x10, 0x07, 0xB0, 0x03, // 231
  0x00, 0x00, 0xF2, 0x01, 0x54, 0x01, 0x70, 0x01, // 232
  0x00, 0x00, 0xF0, 0x01, 0x54, 0x01, 0x72, 0x01, // 233
  0x00, 0x00, 0xF4, 0x01, 0x52, 0x01, 0x76, 0x01, // 234
  0x00, 0x00, 0xF4, 0x01, 0x50, 0x01, 0x74, 0x01, // 235
  0xF4, 0x01, // 236
  0x00, 0x00, 0xF6, 0x01, // 237
  0xF6, 0x01, 0x06, // 238
  0xF4, 0x01, 0x04, // 239
  0x00, 0x00, 0xE4, 0x01, 0x2C, 0x01, 0xBC, 0x01, 0x40, // 240
  0x00, 0x00, 0xF4, 0x01, 0x12, 0x00, 0xF6, 0x01, // 241
  0xE0, 0x00, 0x12, 0x01, 0x14, 0x01, 0xB0, 0x01, 0x40, // 242
  0xE0, 0x00, 0x10, 0x01, 0x14, 0x01, 0xB2, 0x01, 0x40, // 243
  0xE0, 0x00, 0x14, 0x01, 0x12, 0x01, 0xB6, 0x01, 0x40, // 244
  0xE0, 0x00, 0x14, 0x01, 0x12, 0x01, 0xB6, 0x01, 0x40, // 245
  0xE0, 0x00, 0x14, 0x01, 0x10, 0x01, 0xB4, 0x01, 0x40, // 246
  0x40, 0x00, 0x40, 0x00, 0x50, 0x01, 0x40, 0x00, 0x40, // 247
  0xE0, 0x02, 0x90, 0x01, 0x70, 0x01, 0xB0, 0x01, 0x40, // 248
  0x00, 0x00, 0xF2, 0x01, 0x04, 0x01, 0xF0, 0x01, // 249
  0x00, 0x00, 0xF0, 0x01, 0x04, 0x01, 0xF2, 0x01, // 250
  0x00, 0x00, 0xF4, 0x01, 0x02, 0x01, 0xF6, 0x01, // 251
  0x00, 0x00, 0xF4, 0x01, 0x00, 0x01, 0xF4, 0x01, // 252
  0x70, 0x04, 0x84, 0x07, 0xE2, 0x00, 0x10, // 253
  0x00, 0x00, 0xFC, 0x07, 0x10, 0x01, 0xB0, 0x01, 0x40, // 254
  0x74, 0x04, 0x80, 0x07, 0xE4, 0x00, 0x10 // 255
};