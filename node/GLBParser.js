var path = require("path");
var fs = require("fs");

/** Custom Error: RuntimeError */
function RuntimeError(message) {
  this.name = "RuntimeError";
  this.message = message || "Error running the current script";
  this.stack = new Error().stack;
}

var _f = function () {};
_f.prototype = Error.prototype;
RuntimeError.prototype = new _f();
RuntimeError.prototype.constructor = RuntimeError;
/** */

var findRuntimeDll = function () {
  var dlls = ["opencv_world470.dll", "Parser.dll", "GLBParser.node"];
  dlls.forEach(function (dll) {
    if (!fs.existsSync(dll)) {
      throw new RuntimeError("dependice DLL: " + dll + " not found");
    }
  });
};

findRuntimeDll();

var nativeBinding = require("./GLBParser.node");

/**
 * Creates a GLBParser
 * @class
 * @param {string} sourceGLBPath - The source GLB file path
 */
function GLBParser(sourceGLBPath) {
  if (!sourceGLBPath || !sourceGLBPath.length) {
    throw new TypeError("Must specific input GLB file");
  }

  /**
   * sourceGLBPath is the source GLB file path
   * @type {string}
   * @public
   */
  this.sourceGLBPath = sourceGLBPath;

  /**
   * uuid is sdk mapping identifier
   * @type {string}
   * @private
   */
  this.uuid = nativeBinding.ParserConstruct(this.sourceGLBPath);

  /**
   * isImageDuplicateCalculated means that it has been calculated and should not be calculated again.
   * @type {boolean}
   * @private
   */
  this.isImageDuplicateCalculated = false;

  /**
   * isImageDownscaleCalculated means that image downscalation has been calculated and should not be calculated again.
   * @type {boolean}
   * @private
   */
  this.isImageDownscaleCalculated = false;

  /**
   * isBufferReConstructed means that buffer reconstruction has been calculated and should not be calculated again.
   * @type {boolean}
   * @private
   */
  this.isBufferReConstructed = false;

  /**
   * isImageOriginalMemCalculated means that origial image memory has been calculated
   * @type {boolean}
   * @private
   */
  this.isImageOriginalMemCalculated = false;

  /**
   * isFileExported means that output GLB is exported
   * @type {boolean}
   * @private
   */
  this.isFileExported = false;

  /**
   * isSDKRelease means that SDK instance is released and deleted
   * @type {boolean}
   * @private
   */
  this.isSDKRelease = false;
}

/**
 * Calculate duplicated images in the GLB binary, it cannot be called twice
 * @memberof GLBParser
 * @returns {Object[]} - duplicated images infomation
 */
GLBParser.prototype.CalculateDulpicatedImage = function () {
  if (this.isImageDuplicateCalculated) {
    throw new Error("CalculateDulpicatedImage cannot be called twice");
  }

  if (this.isSDKRelease) {
    throw new Error("SDK has been released");
  }

  nativeBinding.CalculateDulpicatedImage(this.uuid);
  this.isImageDuplicateCalculated = true;

  var info = nativeBinding.GetDuplicatedInfo(this.uuid);
  info.method = "CalculateDulpicatedImage";
  return info;
};

/**
 * Compress images by downscale the images in the GLB binary, it cannot be called twice
 * @memberof GLBParser
 * @param {Number} ratio - The downscale ratio
 * @returns {Object[]} - compress images infomation
 */
GLBParser.prototype.CompressImage = function (ratio) {
  if (this.isImageDownscaleCalculated) {
    throw new Error("CompressImage cannot be called twice");
  }

  if (this.isSDKRelease) {
    throw new Error("SDK has been released");
  }

  nativeBinding.CompressImage(this.uuid, ratio);
  this.isImageDownscaleCalculated = true;
  var info = nativeBinding.GetDownscaledInfo(this.uuid);

  info.method = "CompressImage";
  return info;
};

/**
 * Compress images by given memory upperbound in bytes, it cannot be called twice
 * @memberof GLBParser
 * @param {Number} upperbound - The memory upperbound in bytes
 * @returns {Object[]} - compress images infomation
 */
GLBParser.prototype.CompressImageByUpperBound = function (upperbound) {
  if (this.isImageDownscaleCalculated) {
    throw new Error("CompressImage cannot be called twice");
  }

  if (this.isSDKRelease) {
    throw new Error("SDK has been released");
  }

  nativeBinding.CompressImageByUpperBound(this.uuid, upperbound);
  this.isImageDownscaleCalculated = true;
  var info = nativeBinding.GetDownscaledInfo(this.uuid);

  info.method = "CompressImage";
  return info;
};

/**
 * Calculate the Images memory usage by OpenCV Mat Object
 * it cannot be called without calling CalculateDulpicatedImage before
 * @memberof GLBParser
 * @returns {Object[]} - images memory usage
 */
GLBParser.prototype.CalculateImageUsage = function (ratio) {
  if (!this.isImageDuplicateCalculated) {
    throw new Error("CalculateDulpicatedImage must be called before");
  }

  if (this.isSDKRelease) {
    throw new Error("SDK has been released");
  }
  var info = [];

  if (this.isImageDownscaleCalculated) {
    var tmpInfo = nativeBinding.GetDownscaledInfo(this.uuid);

    tmpInfo.forEach(function (el) {
      var o = { bufferViewIndex: 0, cvMatSize: 0 };
      o.bufferViewIndex = el.bufferViewIndex;
      o.cvMatSize = el.cvMatResizedSize;
      info.push(o);
    });
  } else {
    if (!this.isImageOriginalMemCalculated) {
      nativeBinding.CalculateImageMemory(this.uuid);
      info = nativeBinding.GetImageMemoryInfo(this.uuid);
      this.isImageOriginalMemCalculated = true;
    } else {
      info = nativeBinding.GetImageMemoryInfo(this.uuid);
    }
  }

  info.method = "CalculateImageUsage";
  return info;
};

/**
 * Re-construct the GLB binary buffer to build the GLB file,
 * it cannot be called without calling CalculateDulpicatedImage before
 * @memberof GLBParser
 */
GLBParser.prototype.ReConstructBuffer = function () {
  if (!this.isImageDuplicateCalculated) {
    throw new Error("CalculateDulpicatedImage must be called before");
  }

  if (this.isSDKRelease) {
    throw new Error("SDK has been released");
  }

  nativeBinding.ReConstructBuffer(this.uuid);

  this.isBufferReConstructed = true;
};

/**
 * Write out the new GLB file,
 * it cannot be called without calling ReConstructBuffer before
 * @memberof GLBParser
 * @param outGLB {string} - Out GLB filepath
 */
GLBParser.prototype.WriteGLBFile = function (outGLB) {
  if (!this.isBufferReConstructed) {
    throw new Error("ReConstructBuffer must be called before");
  }

  if (this.isSDKRelease) {
    throw new Error("SDK has been released");
  }

  nativeBinding.CalculateJsonChunk(this.uuid);
  nativeBinding.WriteGLBFile(this.uuid, outGLB);

  this.isFileExported = true;
};

/**
 * Get the output manifest Object
 * it cannot be called without calling WriteGLBFile before
 * @memberof GLBParser
 * @returns {object} - The output manifest object
 */
GLBParser.prototype.GetOutputManifest = function () {
  if (!this.isFileExported) {
    throw new Error("WriteGLBFile must be called before");
  }

  if (this.isSDKRelease) {
    throw new Error("SDK has been released");
  }

  return nativeBinding.GetOutManifest(this.uuid);
};

/**
 * Release the GLBParser SDK to free the memory
 * @memberof GLBParser
 */
GLBParser.prototype.ReleaseSDK = function () {
  if (this.isSDKRelease) {
    throw new Error("SDK has been released");
  }

  nativeBinding.ReleaseSDK(this.uuid);
  this.isSDKRelease = true;
};

/**
 * Calculate the size detail from method "CalculateDulpicatedImage" and "CompressImage"
 * @memberof GLBParser
 * @static
 * @param toCalculate {Object[]} - the object to calculate
 */
GLBParser.CalculateSize = function (toCalculate) {
  if (toCalculate.method == "CalculateDulpicatedImage") {
    var bufferViewIndex = [];
    var reduceSize = 0;

    toCalculate.forEach(function (info) {
      bufferViewIndex.push(info.bufferViewIndex);
      reduceSize += info.originalSize;
    });

    return {
      bufferViewIndex: bufferViewIndex,
      reduceSize: reduceSize,
    };
  } else if (toCalculate.method == "CompressImage") {
    var originalFileSize = 0,
      compressFileSize = 0,
      originalMemorySize = 0,
      compressMemorySize = 0;

    toCalculate.forEach(function (info) {
      originalFileSize += info.originalSize;
      (compressFileSize += info.compressSize),
        (originalMemorySize += info.cvMatSize);
      compressMemorySize += info.cvMatResizedSize;
    });

    return {
      totalCount: toCalculate.length,
      scaleRatio: toCalculate[0].scaleRatio,
      originalFileSize: originalFileSize,
      compressFileSize: compressFileSize,
      originalMemorySize: originalMemorySize,
      compressMemorySize: compressMemorySize,
    };
  } else if (toCalculate.method == "CalculateImageUsage") {
    var size = 0;
    toCalculate.forEach(function (info) {
      size += info.cvMatSize;
    });
    return {
      memSize: size,
      count: toCalculate.length,
    };
  } else {
    throw new Error("Cannout calulcate objects");
  }
};

module.exports = GLBParser;
