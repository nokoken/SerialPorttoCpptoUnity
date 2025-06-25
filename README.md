## このリポジトリについて
<p>このリポジトリはusbシリアルポートからlocalエリアにてばら撒かれたデータをC++のソースに渡して<br />それをそのままUnityへ渡すサンプルプログラムを保存している物です<br /></p>

## 実行手順

<p>1.まず、usb側のポート番号を調べます<br />
2.次に、何かしらusbを通じてデータを送ります<br />
3.そして、C++のコードをコンパイルして実行します<br />
4.最後に付属のUnityプロジェクトを立ち上げ再生ボタンを押します。<br />
5.ログ部分を確認すると送ったデータを見ることができます。<br /></p>

## 実行環境
<p>Unityのバージョンは2022.3.25f1となっております。その他の環境は以下に貼っておきます</br></p>

```
% zsh --version
zsh 5.9 (x86_64-apple-darwin23.0)
% sw_vers
ProductName:		macOS
ProductVersion:		15.5
BuildVersion:		24F74
% g++ -v
Apple clang version 15.0.0 (clang-1500.3.9.4)
Target: arm64-apple-darwin24.5.0
Thread model: posix
InstalledDir: /Library/Developer/CommandLineTools/usr/bin
```