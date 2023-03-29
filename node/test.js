var GLBParser = require("./GLBParser");
var parser = new GLBParser("./test2.glb");

var info = parser.CalculateDulpicatedImage();
var dulpicatedInfo = GLBParser.CalculateSize(info);
console.log("Duplicated Info:");
console.log(
  "There are %d buffer duplicated, and reduce %d bytes",
  dulpicatedInfo.bufferViewIndex.length,
  dulpicatedInfo.reduceSize
);
console.log("*************");
console.log("*************");

info = parser.CalculateImageUsage();
var imageMemInfo = GLBParser.CalculateSize(info);
console.log("Image Memory Info:");
console.log(
  "There are %d images, and total %d bytes",
  imageMemInfo.count,
  imageMemInfo.memSize
);
console.log("*************");
console.log("*************");

//info = parser.CompressImage(0.5);
info = parser.CompressImageByUpperBound(11 * 1024 * 1024);
var compressInfo = GLBParser.CalculateSize(info);
console.log("Compress Info:");
console.log(
  "There are %d images compressed, with %f scale factory",
  compressInfo.totalCount,
  compressInfo.scaleRatio
);
console.log(
  "Original Disk Size: %d bytes, reduce to %d bytes",
  compressInfo.originalFileSize,
  compressInfo.compressFileSize
);
console.log(
  "Original Memory Size: %d bytes, reduce to %d bytes",
  compressInfo.originalMemorySize,
  compressInfo.compressMemorySize
);

console.log("*************");
console.log("*************");

parser.ReConstructBuffer();
parser.WriteGLBFile("./out.glb");

var fs = require("fs");
console.log("Output Manifest");
fs.writeFileSync(
  "manifest.json",
  JSON.stringify(parser.GetOutputManifest(), null, 4)
);

parser.ReleaseSDK();

console.log("Parser GLB Terminated");
