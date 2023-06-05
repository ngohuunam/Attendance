var express = require("express");
var router = express.Router();

/* POST cam.jpg. */

var multer = require("multer");
var upload = multer({ dest: "uploads/" });
var path = require("path");
var fs = require("fs");

router.post("/", upload.single("imageFile"), function (req, res, next) {
  // req.file is the `avatar` file
  // req.body will hold the text fields, if there were any

  console.log(req.hostname);
  // res.status(200).contentType("text/plain").end("File uploaded!");
  const tempPath = req.file.path;
  console.log(tempPath);
  const targetPath = path.join(__dirname, "uploads/image-" + new Date().getTime() + ".jpg");
  console.log(targetPath);

  if (path.extname(req.file.originalname).toLowerCase() === ".jpg") {
    fs.rename(tempPath, targetPath, (err) => {
      if (err) return handleError(err, res);

      res.status(200).contentType("text/plain").end("File uploaded!");
    });
  } else {
    fs.unlink(tempPath, (err) => {
      if (err) return handleError(err, res);

      res.status(403).contentType("text/plain").end("Only .jpg files are allowed!");
    });
  }
});

module.exports = router;
