var path = require("path");

var GLTFSDKInclude = path.resolve("../GTLFSDK/Incl").replace(/\\/gm, "\\\\");

var GLBParserLib = path.resolve("../Parser.lib").replace(/\\/gm, "\\\\");
var GLBParserStreamLib = path
  .resolve("../ParserStream.lib")
  .replace(/\\/gm, "\\\\");
var GLBSDKLib = path
  .resolve("../GTLFSDK/lib/GLTFSDK.lib")
  .replace(/\\/gm, "\\\\");

module.exports = {
  GLTFSDKInclude: GLTFSDKInclude,
  GLBParserLib: GLBParserLib,
  GLBParserStreamLib: GLBParserStreamLib,
  GLBSDKLib: GLBSDKLib,
};
