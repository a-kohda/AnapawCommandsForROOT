# ANAPAW Commands for ROOT
- まだテスト中です。
- 最低限の機能は出来たので公開します。
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
- ls()
  - ヒストグラムの一覧を表示。表示対象は「.ls」で表示されるもの。HID(ヒストグラムID)、次元、タイトルを表示。

- ht(hid)
  - hidを数値指定して、そのヒストグラムを表示する。第二引数でDraw optionの指定可。

- hn() / hb()
  - 現在表示中のヒストグラムの次の(/前の)ヒストグラムを表示する。引数でDraw optionの指定可。

- lgx() / lgy() / lgz()
  - x / y / z 軸をLogスケールにする

- lnx() / lny() / lnz()
  - x / y / z 軸をリニアスケールにする
