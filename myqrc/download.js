"use strict";
require('es6-promise').polyfill();

var fs = require('fs');
var path = require('path');
var http = require('http');
var https = require('https');
var urlParse  = require('url').parse;
var googleTTS = require('google-tts-api');

function downloadFile (url, dest) {
  return new Promise(function (resolve, reject) {
    var info = urlParse(url);
    var httpClient = info.protocol === 'https:' ? https : http;
    var options = {
      host: info.host,
      path: info.path,
      headers: {
        'user-agent': 'WHAT_EVER'
      }
    };

    httpClient.get(options, function(res) {
      // check status code
      if (res.statusCode !== 200) {
        reject(new Error('request to ' + url + ' failed, status code = ' + res.statusCode + ' (' + res.statusMessage + ')'));
        return;
      }

      var file = fs.createWriteStream(dest);
      file.on('finish', function() {
        // close() is async, call resolve after close completes.
        file.close(resolve);
      });
      file.on('error', function (err) {
        // Delete the file async. (But we don't check the result)
        fs.unlink(dest);
        reject(err);
      });

      res.pipe(file);
    })
    .on('error', function(err) {
      reject(err);
    })
    .end();
  });
}


// start
(function() {
    //console.log(process.argv)
    if (process.argv.length <= 2) {
        console.log("try: argv.js 1 2 3 4 ...");
        return;
    }

    console.log("argv: " + process.argv[2]);

    googleTTS(process.argv[2])
    .then(function (url) {
      console.log(url); // https://translate.google.com/translate_tts?...

      var dest = path.resolve(__dirname, 'out.mp3'); // file destination
      console.log('Download to ' + dest + ' ...');

      return downloadFile(url, dest);
    })
    .then(function () {
      console.log('Download success');
    })
    .catch(function (err) {
      console.error(err.stack);
    });

})();


