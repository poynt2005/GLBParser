var GUI = require("./GUI");
var GLBParser = require("./GLBParser");
var path = require("path");

var glbParser;

var gui = new GUI(function () {
  console.log("The application has been closed");
});

gui.CreateMainWindow("GLBParser GUI Application");

gui.SetButtonCallback(GUI.Identifier.LOAD_GLB_BUTTON_ID, function () {
  var glbPath = GUI.OpenFileDialog();
  var pathStruct = path.parse(glbPath);
  glbParser = new GLBParser(glbPath);
  gui.SetText(GUI.Identifier.LOADED_GLB_TEXT, pathStruct.base);
});

gui.SetButtonCallback(GUI.Identifier.CALCUL_GLB_BUTTON_ID, function () {
  var inputText = gui.GetText();
  gui.SetProgress(0);

  if (inputText.length && !/[0-9]+/.test(inputText)) {
    GUI.ShowWarningMessage(
      "Size Is Invalid",
      "Input: " + inputText + " is an invalid size value, enter again"
    );
    return;
  }

  var info = glbParser.CalculateDulpicatedImage();
  var dulpicatedInfo = GLBParser.CalculateSize(info);
  gui.SetProgress(33);

  var infoStr =
    "There are " +
    dulpicatedInfo.bufferViewIndex.length +
    " buffer duplicated, and reduce " +
    dulpicatedInfo.reduceSize +
    " bytes";

  gui.SetText(GUI.Identifier.DUPLICATED_INFO_TEXT, infoStr);

  info = glbParser.CalculateImageUsage();
  var imageMemInfo = GLBParser.CalculateSize(info);
  gui.SetProgress(66);
  infoStr =
    "There are " +
    imageMemInfo.count +
    " images, and total " +
    imageMemInfo.memSize +
    " bytes";
  gui.SetText(GUI.Identifier.IMAGE_MEMORY_INFO_TEXT, infoStr);

  if (inputText.length) {
    info = glbParser.CompressImageByUpperBound(+inputText);
    var compressInfo = GLBParser.CalculateSize(info);
    infoStr =
      "Original Memory Size: " +
      compressInfo.originalMemorySize +
      " bytes, reduce to " +
      compressInfo.compressMemorySize +
      " bytes";
    gui.SetText(GUI.Identifier.COMPRESS_INFO_TEXT, infoStr);
  }
  gui.SetProgress(100);
});

gui.SetButtonCallback(GUI.Identifier.SAVE_GLB_BUTTON_ID, function () {
  gui.SetProgress(0);
  glbParser.ReConstructBuffer();
  gui.SetProgress(50);
  glbParser.WriteGLBFile(path.resolve("./out.glb"));
  gui.SetProgress(100);
  GUI.ShowWarningMessage(
    "Save File Success",
    'File is save to "out.glb" to current working directory'
  );
});

gui.RenderFrame();
