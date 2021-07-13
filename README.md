# GraphPrinter

![title](https://user-images.githubusercontent.com/51815450/101246117-93b7aa00-3754-11eb-811c-da38fbbd5b64.PNG)

## Description

This plugin adds a shortcut key that prints an Unreal Engine graph editor (such as Blueprint or Material ...) to an image file or copies it to the clipboard.
As shown in the image below, you can also output the part that is not displayed in the graph editor to the image file.
You can also restore the node from the output png format image file.  

#### On the editor
![grapheditor](https://user-images.githubusercontent.com/51815450/101246223-50117000-3755-11eb-8966-5da5124d420e.PNG)

#### Output image
![BP_Sky_Sphere-RefreshMaterial](https://user-images.githubusercontent.com/51815450/114880820-60d99d00-9e3d-11eb-92b6-e7ef5b6f4cc3.png)

#### Node restore  
![restore](https://user-images.githubusercontent.com/51815450/114888626-61c1fd00-9e44-11eb-96e8-602753fa40bd.gif)

## Requirement

Target version : UE4.21 ～ 4.26  
Target platform : Windows, Mac, Linux 

## Installation

Put the Plugins/GraphPrinter folder in your project's Plugins folder.  
Or install from the [marketplace](https://www.unrealengine.com/marketplace/en/product/graph-printer).  

## Usage

The default shortcut keys that correspond to the added functions are as follows.

|**Shortcut Key**|**Function**|
|:---:|---|
|Ctrl + F7|Copy all currently open Graph Editor nodes to the clipboard. (**Windows Only / Node information cannot be embedded**)|
|Ctrl + F8|Copy the selected node of the currently open graph editor to the clipboard. (**Windows Only / Node information cannot be embedded**)|
|Ctrl + F9|Outputs an image containing all the nodes of the currently open graph editor.|
|Ctrl + F10|Outputs an image containing the selected nodes in the currently open graph editor.|
|Ctrl + F11|Restores the nodes contained in the selected png file in the currently open graph editor.|
|Ctrl + F12|Open the output destination directory set in the editor preferences.|

You can also change the shortcut keys from the keyboard shortcuts in the editor preferences.

![keyconfig2](https://user-images.githubusercontent.com/51815450/114881766-43f19980-9e3e-11eb-9e71-da65bdfa5dbb.PNG)

## Settings

![setting2](https://user-images.githubusercontent.com/51815450/114881742-3d632200-9e3e-11eb-8bab-5db67665dee5.PNG)

The items that can be set from the editor preferences are as follows.

|**Item**|**Description**|
|---|---|
|Is Include Node Info in Image File|Embed node information in the output image file and turn on the function to restore the node from the image file. This feature only supports png format.|
|Format|Set the format of the output image. Supported image formats are *png*, *jpeg*, *bmp*, *exr*.|
|Compression Quality|Set the compression rate of the output image.|
|Filtering Mode|Sets the texture filtering mode used on output.|
|Use Gamma|Set whether to reflect the gamma value in the output image. If *false*, the output image may be darker than you can see in the editor.|
|Padding|Set the margin when outputting to the image. Increase the value if the node is not included in the output image.|
|MaxImageSize|Set the maximum size of the output image. If it exceeds this size, it cannot be output.|
|Can Overwrite File when Export|Set whether to overwrite the file with the same name when outputting the image. If *false*, if there is a file with the same name, the suffix will be given number.|
|Output Directory Path|Set the output destination directory. By default, it is under "[Project]/Saved/GraphPrinter/".|

## License

[MIT License](https://en.wikipedia.org/wiki/MIT_License)

## Author

[Naotsun](https://twitter.com/Naotsun_UE)

## History  

- (2021/07/11) v1.7   
  Added a shortcut key to copy the image of Graph Editor to the clipboard (**Windows Only**)  

- (2021/05/28) v1.6   
  Changed so that the asset name is included in the file name when the reference viewer is output

- (2021/05/05) v1.5   
  Corrected the problem that the image output for the first time after starting the engine becomes whitish

- (2021/04/15) v1.4   
  Added the function to embed node information in an image file and restore the node from the image file

- (2021/03/08) v1.3   
  ~~Corrected the problem that the image output for the first time after starting the engine becomes whitish~~

- (2021/01/21) v1.2   
  Changed module type to EditorNoCommandlet as a countermeasure against errors when packaging

- (2021/01/12) v1.1   
  Added the ability to limit the output size as it crashes when outputting a huge graph

- (2020/12/06) v1.0   
  Publish plugin
