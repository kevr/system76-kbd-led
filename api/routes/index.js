var express = require('express');
var exec = require('child_process').exec;
var shellescape = require('shell-escape');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.json({ title: 'Express' });
});

router.get('/colors', (req, res, next) => {
  let command = "system76-kbd-led++";
  exec(command, (error, stdout, stderr) => {
    if(error) {
      console.error(`Error running ${command}!`);
      res.status(500).json({});
    } else {
      const lines = stdout.split("\n");
      const output = {
        left: lines[0],
        center: lines[1],
        right: lines[2],
        extra: lines[3]
      };
      res.json(output);
    }
  });
});

router.put('/colors', (req, res, next) => {
  const color = shellescape([req.query["color"]]);
  console.log(`Color: ${color}`);
  if(!color || color.length !== 6 || !color.match(/[a-fA-F0-9]{6}/g)) {
    res.status(400).json({});
    return;
  }

  let command = "system76-kbd-led++ ";
  command += `-l ${color} -c ${color} -r ${color}`;

  console.log(`Executing '${command}'`);
  exec(command, (error, stdout, stderr) => {
    if(error) {
      console.error("error setting /color!");
    } else {
      console.log("successfully set /color.");
    }
    res.json({ status: 200, message: "OK" });
  });
});

router.put('/colors/:region', (req, res, next) => {
  const color = shellescape([req.query["color"]]);
  console.log(`Color: ${color}`);
  if(!color || color.length !== 6 || !color.match(/[a-fA-F0-9]{6}/g)) {
    res.status(400).json({});
    return;
  }

  let command = "system76-kbd-led++ ";
  const region = req.params["region"];
  if(["left", "center", "right"].indexOf(region) == -1) {
    res.json({ status: 400, message: "Incorrect 'region' parameter." });
    return;
  }
  command += `-${region[0]} ${color}`;

  exec(command, (error, stdout, stderr) => {
    if(error) {
      console.error("error setting /color!");
    } else {
      console.log("successfully set /color.");
    }
    res.json({ status: 200, message: "OK" });
  });
});

module.exports = router;
