# GraphPrinter

![title](https://user-images.githubusercontent.com/51815450/101246117-93b7aa00-3754-11eb-811c-da38fbbd5b64.PNG)

## Description

This plugin adds shortcut keys for printing the graph editor of Unreal Engine (blueprints, materials, etc.) to image files.
As shown in the image below, you can also output the part that is not displayed in the graph editor to the image file.

#### On the editor
![grapheditor](https://user-images.githubusercontent.com/51815450/101246223-50117000-3755-11eb-8966-5da5124d420e.PNG)

#### Output image
![BP_Sky_Sphere-RefreshMaterial](https://user-images.githubusercontent.com/51815450/101246304-cd3ce500-3755-11eb-8c59-14a8158f73c5.png)

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
|Ctrl + F9|Outputs an image containing all the nodes of the currently open graph editor.|
|Ctrl + F10|Outputs an image containing the selected nodes in the currently open graph editor.|
|Ctrl + F12|Open the output destination directory set in the editor preferences.|

You can also change the shortcut keys from the keyboard shortcuts in the editor preferences.

![keyconfig](https://user-images.githubusercontent.com/51815450/101246571-8b14a300-3757-11eb-992b-d803d5b01cf9.PNG)

## Settings

![settings](https://user-images.githubusercontent.com/51815450/101246626-e5156880-3757-11eb-8cc6-b017afa3a331.PNG)

The items that can be set from the editor preferences are as follows.

|**Item**|**Description**|
|---|---|
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

- (2021/03/08) v1.3   
  Corrected the problem that the image output for the first time after starting the engine becomes whitish

- (2021/01/21) v1.2   
  Changed module type to EditorNoCommandlet as a countermeasure against errors when packaging

- (2021/01/12) v1.1   
  Added the ability to limit the output size as it crashes when outputting a huge graph

- (2020/12/06) v1.0   
  Publish plugin
