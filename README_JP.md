# GraphPrinter

![title](https://user-images.githubusercontent.com/51815450/101246117-93b7aa00-3754-11eb-811c-da38fbbd5b64.PNG)

## 概要

このプラグインはUnreal Engineのグラフエディタ(BlueprintやMaterialなど...)を画像ファイルに出力するショートカットキーを追加します。
また、下の画像の様にグラフエディタで表示していない部分も含めて画像ファイルに出力できます。
さらに出力したpng形式の画像ファイルからノードを復元することもできます。  

#### エディタ上での状態  
![grapheditor](https://user-images.githubusercontent.com/51815450/101246223-50117000-3755-11eb-8966-5da5124d420e.PNG)

#### 出力された画像  
![BP_Sky_Sphere-RefreshMaterial](https://user-images.githubusercontent.com/51815450/114880820-60d99d00-9e3d-11eb-92b6-e7ef5b6f4cc3.png)

#### ノードの復元  
![restore](https://user-images.githubusercontent.com/51815450/114888626-61c1fd00-9e44-11eb-96e8-602753fa40bd.gif)

## 動作環境

対象バージョン : UE4.21 ～ 4.26  
対象プラットフォーム : Windows, Mac, Linux 

## インストール

プロジェクトのPluginsフォルダにPlugins/GraphPrinterフォルダを入れてください。  
または、[マーケットプレイス](https://www.unrealengine.com/marketplace/en/product/graph-printer)からインストールしてください。

## 使い方

追加される機能と対応するデフォルトのショートカットキーは以下の通りです。

|**ショートカットキー**|**機能**|
|:---:|---|
|Ctrl + F7|現在開いているグラフエディタのすべてのノードをクリップボードにコピーする。(**Windows限定**)|
|Ctrl + F8|現在開いているグラフエディタの選択中のノードをクリップボードにコピーする。(**Windows限定**)|
|Ctrl + F9|現在開いているグラフエディタのすべてのノードを含む画像を出力します。|
|Ctrl + F10|現在開いているグラフエディタの選択中のノードを含む画像を出力します。|
|Ctrl + F11|現在開いているグラフエディタに選択したpngファイルに含まれるノードを復元します。|
|Ctrl + F12|エディタの環境設定で設定されている出力先のディレクトリを開きます。|

また、ショートカットキーはエディタの環境設定のキーボードショートカットから変更できます。

![keyconfig2](https://user-images.githubusercontent.com/51815450/114881766-43f19980-9e3e-11eb-9e71-da65bdfa5dbb.PNG)

## オプション

![setting2](https://user-images.githubusercontent.com/51815450/114881742-3d632200-9e3e-11eb-8bab-5db67665dee5.PNG)

エディタの環境設定から設定できる項目は以下の通りです。

|**項目**|**説明**|
|---|---|
|Is Include Node Info in Image File|出力する画像ファイルにノードの情報を埋め込み、画像ファイルからノードを復元する機能をオンにします。この機能はpng形式のみサポートしています。|
|Format|出力する画像の形式を設定します。対応している画像フォーマットは*png*、*jpeg*、*bmp*、*exr*です。|
|Compression Quality|出力する画像の圧縮率を設定します。|
|Filtering Mode|出力する際に使用するテクスチャのフィルタリングモードを設定します。|
|Use Gamma|出力する画像にガンマ値を反映するかを設定します。*false*の場合、出力された画像がエディタで見るよりも暗くなる場合があります。|
|Padding|画像に出力する際の余白を設定します。ノードが見切れる際などに値を大きくしてください。|
|Can Overwrite File when Export|画像を出力する際に同名のファイルを上書きするかを設定します。*false*の場合、同名のファイルがあった際にはサフィックスに連番が付きます。|
|Output Directory Path|出力先のディレクトリを設定します。デフォルトでは"[Project]/Saved/GraphPrinter/"以下となっています。|

## ライセンス

[MITライセンス](https://ja.wikipedia.org/wiki/MIT_License)

## 作者

[Naotsun](https://twitter.com/Naotsun_UE)

## 履歴

- (2021/07/11) v1.7   
  グラフエディタの画像をクリップボードにコピーするショートカットキーを追加した(**Windows限定**)  

- (2021/05/28) v1.6   
  リファレンスビューワーを出力した時にファイル名にアセット名が含まれるように変更した  

- (2021/05/05) v1.5   
  エンジンを起動して初めて出力する画像が白みがかる不具合の対処を行った  

- (2021/04/15) v1.4   
  ノード情報を画像ファイルに埋め込み、画像ファイルからノードを復元する機能を追加  

- (2021/03/08) v1.3   
  ~~エンジンを起動して初めて出力する画像が白みがかる不具合の対処を行った~~

- (2021/01/21) v1.2   
  パッケージ化の際にエラーが発生する対策としてモジュールタイプをEditorNoCommandletに変更

- (2021/01/12) v1.1
  巨大なグラフを出力する際にクラッシュするため、出力サイズに制限をかけられる機能を追加

- (2020/12/06) v1.0   
  プラグインを公開
