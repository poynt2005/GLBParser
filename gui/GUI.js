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
  var dlls = ["GUI.node", "GUI.dll"];
  dlls.forEach(function (dll) {
    if (!fs.existsSync(dll)) {
      throw new RuntimeError("dependice DLL: " + dll + " not found");
    }
  });
};

findRuntimeDll();

var nativeBinding = require("./GUI.node");
var constants = nativeBinding.GetConstant();

/**
 * Creates a GUI object
 * @class
 * @param {function} onClose - The function to be invoke when the application is close
 */
function GUI(onClose) {
  /**
   * onClose is the function to be invoke when the application is close
   * @type {function}
   * @private
   */
  this.onClose = typeof onClose == "function" ? onClose : function () {};

  /**
   * isCreated used to identify whether the GUI application is created successfully
   * @type {boolean}
   * @private
   */
  this.isCreated = false;
}

/**
 * Create a Graphic User Iterface window by windows Graphics Device Interface
 * @memberof GUI
 * @param {string} title - the window title
 */
GUI.prototype.CreateMainWindow = function (title) {
  if (typeof title != "string") {
    throw new TypeError("Title must be a string");
  }

  if (this.isCreated) {
    throw new TypeError("Cannot create window twice");
  }

  if (!title.length) {
    this.isCreated = nativeBinding.CreateMainWindow("Test_Window");
  } else {
    this.isCreated = nativeBinding.CreateMainWindow(title);
  }

  if (!this.isCreated) {
    throw new RuntimeError("Failed to create the GUI application");
  }
};

/**
 * Set the value of info text block
 * @memberof GUI
 * @param {number} textId - The ID of text block, can access by the static property "Identifier"
 * @param {string} text - The text you want to set
 */
GUI.prototype.SetText = function (textId, text) {
  if (!this.isCreated) {
    throw new TypeError("Cannot call this method before GUI is created");
  }

  nativeBinding.SetText(textId, text);
};

/**
 * Get the text value of edit block
 * @memberof GUI
 * @returns {string} - The text value of the edit block
 */
GUI.prototype.GetText = function () {
  if (!this.isCreated) {
    throw new TypeError("Cannot call this method before GUI is created");
  }
  return nativeBinding.GetText();
};

/**
 * Set the progress bar step
 * @memberof GUI
 * @param {number} progress - the precentage of progress, e.g. 30%, set to 30
 */
GUI.prototype.SetProgress = function (progress) {
  if (!this.isCreated) {
    throw new TypeError("Cannot call this method before GUI is created");
  }
  nativeBinding.SetProgress(progress);
};

/**
 * Set the button callback
 * @memberof GUI
 * @param {number} buttonId - The ID of button, can access by the static property "Identifier"
 * @param {function} callback - The function to be invoked by click the button
 */
GUI.prototype.SetButtonCallback = function (buttonId, callback) {
  if (!this.isCreated) {
    throw new TypeError("Cannot call this method before GUI is created");
  }
  nativeBinding.SetButtonCallback(buttonId, callback);
};

/**
 * Start rendering the GUI application
 * @memberof GUI
 */
GUI.prototype.RenderFrame = function () {
  if (!this.isCreated) {
    throw new TypeError("Cannot call this method before GUI is created");
  }

  var interval = setInterval(
    function () {
      nativeBinding.RenderFrame();

      var messageType = nativeBinding.GetCurrentMessageType();
      if (messageType == "WM_DESTROY" || messageType == "WM_NCDESTROY") {
        clearInterval(interval);
        nativeBinding.ReleaseThreadFunction();
        this.onClose();
      }
    }.bind(this),
    50
  );
};

/**
 * Create a message box waring window
 * @memberof GUI
 * @static
 * @param {string} messageTitle - Message box title
 * @param {string} messageText - Message box text
 */
GUI.ShowWarningMessage = function (messageTitle, messageText) {
  nativeBinding.ShowWarningMessage(messageTitle, messageText);
};

/**
 * Show a "open file" dialog, can just open .glb files
 * @memberof GUI
 * @static
 * @returns {string} - The opened file path of the file
 */
GUI.OpenFileDialog = function () {
  return nativeBinding.OpenFileDialog();
};

/**
 * Retrieve identifier from C++ binding
 * @memberof GLBParser
 * @static
 */
GUI.Identifier = {
  /**
   * Button id of "LoadGLB"
   * @type {number}
   */
  LOAD_GLB_BUTTON_ID: constants.LOAD_GLB_BUTTON_ID,

  /**
   * Button id of "SaveGLB"
   * @type {number}
   */
  SAVE_GLB_BUTTON_ID: constants.SAVE_GLB_BUTTON_ID,

  /**
   * Button id of "Calculate"
   * @type {number}
   */
  CALCUL_GLB_BUTTON_ID: constants.CALCUL_GLB_BUTTON_ID,

  /**
   * Text label id of "Duokicated Info"
   * @type {number}
   */
  DUPLICATED_INFO_TEXT: constants.DUPLICATED_INFO_TEXT,

  /**
   * Text label id of "Image Memory Info"
   * @type {number}
   */
  IMAGE_MEMORY_INFO_TEXT: constants.IMAGE_MEMORY_INFO_TEXT,

  /**
   * Text label id of "Compress Info"
   * @type {number}
   */
  COMPRESS_INFO_TEXT: constants.COMPRESS_INFO_TEXT,

  /**
   * Text label id of "Loaded GLB"
   * @type {number}
   */
  LOADED_GLB_TEXT: constants.LOADED_GLB_TEXT,

  /**
   * Input Text label id
   * @type {number}
   */
  TEXT_EDITOR_ID: constants.TEXT_EDITOR_ID,

  /**
   * Progress bar Id
   * @type {number}
   */
  PROGRESS_PAR_ID: constants.PROGRESS_PAR_ID,
};

module.exports = GUI;
