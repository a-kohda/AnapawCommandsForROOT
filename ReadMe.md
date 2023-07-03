# ANAPAW Commands for ROOT
- ROOTのヒストグラムを少ないタイプ数で操作出来るようにします。
- コマンド名(関数名)は基本的にANAPAWに準拠してます。
- ROOTの標準機能とは干渉しないようにしてます。
- まだテスト中です。
- 最低限の機能は出来たので公開します。

## ダウンロード
- ソースコードはGitHubにあります。
  - https://github.com/a-kohda/AnapawCommandsForROOT

## 内容物
- ReadMe.md
  - これ。
- anapawcmd.C
  - 本体。後述のANAPAWモードを使わない場合は、このファイルしか使わない。
- apmode / apmode.cc
  - 括弧なしコマンド(ANAPAWモードでの入力)を使うための実行ファイル。
- Makefile
  - apmode をビルドするために使う。anapawcmd.Cだけ使う人は要らない。

## 使い方
### 設定
- anapawcmd.C をダウンロードして適当なところに設置してください。
- ROOT起動後に .L /path/to/anapawcmd.C とすると読み込まれて、コマンドが使えるようになります。
- 自動読み込みを設定する場合はrootlogon.Cに以下を追記します。
```
gROOT->ProcessLine(".L /path/to/anapawcmd.C");
gROOT->ProcessLine("SetAPStyle();");
```
### 利用可能なコマンド一覧
- 使える関数の一覧はここにあります。 (テスト中)
  - https://a-kohda.github.io/AnapawCommandsForROOT/anapawcmd_8C.html#details
- ls()
  - ヒストグラムの一覧を表示。表示対象は「.ls」で表示されるもの。HID(ヒストグラムID)、次元数、タイトルを表示。

- ht(hid)
  - hidを数値指定して、そのヒストグラムを表示(Draw)する。第二引数でDraw optionの指定可。

- hn() / hb()
  - 現在表示中のヒストグラムの次の(/前の)ヒストグラムを表示する。引数でDraw optionの指定可。

- lgx() / lgy() / lgz()
  - x / y / z 軸をLogスケールにする

- lnx() / lny() / lnz()
  - x / y / z 軸をリニアスケールにする

### 不完全だけど使えるコマンド
- sly()
  - 2次元ヒストグラムをx方向にスライスしてy軸への射影した1次元ヒストグラムを作成する。
    - 本家ANAPAWではスライス数を指定するが、ここでは今の所、元の2次元ヒストグラムのx方向の分割数でしかスライスできない。そして、slx()はまだ出来てない。

- blowx(min, max) / blowy(min, max)
  - ANAROOT準拠。x/y軸の表示範囲を変更する。(本家ANAPAW/ANAROOTでは新しいヒストグラムとして再生成するが、これは単に表示エリアを変えるだけ)

- blowx() / blowy() (引数なし)
  - x/y軸の表示範囲を初期状態に戻す。(unzoomする)

- showstat() / delstat()
  - 独自機能。統計ボックスを表示 (/非表示に) する。

- print()
  - 現在表示中のヒストグラム(gPad)に適当なファイル名をつけてPNG画像として保存する。保存先ディレクトリは、./png, ../png, ./plots, ...と順に探し、最初に見つかったディレクトリに保存する。
    - 注意 : ディレクトリ検索にsystem関数を使っています。Linux以外のOSでは多分動作しないです。

- adl()
  - 独自機能。tree->Draw()等で作成したヒストグラムをリストに登録する。(ヒストグラムIDが与えられて、後からそのIDを使って呼び出せる。)

## ANAPAWモード (apmode, ベータ版)
- ANAPAWみたいに括弧なしでコマンドが使えるようにするためのプログラムです。
  - 基本的に文字列処理で "ht 1" -> "ht(1)" のように整形してるだけです。
- 試してみたい人は、ソースコード内にanapawcmd.Cへのフルパスがベタ書きされてるのでそこを直してからmakeしてください。
- 通常のROOTを起動する代わりにこのファイルを実行すると使えます。
- ROOT本来の機能は問題なく使えるはずです。

## 既知のバグ
- ROOT5で動かない。デフォルト引数が二重定義になってるとか言われる。気が向いたら直す。
  - ~~多分直りました。但し、makeする時のROOTのバージョンと、実行するときのROOTのバージョンが同じでないと落ちます。(2023/3/3)~~
	- ROOT5.34/38 on Ubuntu16.04でテストしたところデフォルト引数が二重定義のエラーが依然として出る。解決策はわかったがまだ修正してない。同じ環境でapmodeをmakeしようとすると、エラーが出る。Makefileの表記順が間違ってるらしい。未修正。(2023/6/8)
- DrawHistで「HistogramID」の表示が、zoneで2行以上にした時に数値ラベルと重なる (2023/3/28)
  - 表示位置設定がx軸からの相対位置になっているのが問題。そのうち直す。

## 更新情報
- 2023/6/28
  - adlを使えるようにした。Makefileを修正した。(ライブラリの指定順が悪くてUbuntuでコンパイルが通らなかったのを修正。)

## リファレンスマニュアル (テスト中)
- Doxygen で自動生成したマニュアル
  - https://a-kohda.github.io/AnapawCommandsForROOT/index.html

