# ANAPAW Commands for ROOT
- ROOTのヒストグラムを少ないタイプ数で操作出来るようにします。
- コマンド名(関数名)は基本的にANAPAWに準拠してます。
- ROOTの標準機能とは干渉しないようにしてます。
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

## 既知のバグ
- ファイルをオープンしてないときなど、表示すべきヒストグラムが存在しないときに hn() / hb() を実行すると無限ループに陥る。
- 同じ2次元ヒストグラムに対して複数回 sly() を実行すると同名の1次元ヒストグラムが生成されてしまう。これにより hn() / hb() が正常動作しなくなる。
