# L0 Vol.2
## 執筆方法
* https://sigcoww.herokuapp.com/howto.html
* https://github.com/kmuto/review/blob/master/doc/format.ja.md
* https://github.com/SIGCOWW/L0-2/tree/master/src/articles
  * https://github.com/SIGCOWW/L0-2/blob/master/artifacts/book.pdf

```
$ git clone https://github.com/SIGCOWW/l0-2.git
$ cd l0-2/
$ ./make.sh install kakuyo
$ vim src/articles/kakuyo.re
$ # ./make.sh build ... 実行結果は tmp/ に書かれる
$ git add -A
$ # git commit -m "[ci skip] test"
$ git commit -m "TEST"
$ git push origin master
```


## ディレクトリ配置
```
.
├── artifacts/ ......... 成果物ディレクトリ(人間が突っ込む)
│
├── tmp/ ............... 成果物ディレクトリ(ビルドツールが突っ込む/Gitで管理しない)
│   ├── origin.pdf .... Re:VIEWによる出力
│   ├── body.pdf ...... 入稿用PDF(※電子書籍PDFに比べて上下左右に3mmずつ大きい)
│   ├── book.pdf ...... 電子書籍PDF(※ 未完成)
│   └── book.epub ..... 電子書籍EPUB(※ 未完成)
│
└── src/ ............... ビルド対象ディレクトリ
     ├── articles/ ..... 原稿本体
     ├── images/ ....... 画像ディレクトリ
     └── extensions/ ... Re:VIEW拡張。review-ext.rbも編集する。
```


## 拡張
### `list` 系の wide なやつ
* `listwide` ブロック
* `listnumwide` ブロック
* `emlistwide` ブロック
* `emlistnum` ブロック
* `cmdwide` ブロック

### `nest` ブロック
* 「番号付き箇条書き」"に"「番号付き箇条書き」と「箇条書き」を入れられる
* 深さは2(というか1というのかな)
* 「数値.」で番号付き扱い、「アスタリスク」「+」「-」で箇条書き扱い

```
//nest{
1. abc
2. ABC
   - 箇条書き
   *** 記号の数は問わない
+ スペースも問わない
3. ただし、
   1. 番号つき箇条書きはスペースを問う
  2. 先頭に1つより多いスペースが必要
100. あと数値番号は無視される
  1. ここは、LaTeXでは「a」になるが、数値で書く
//}
```

### `@<author>{おーさー}` 命令
* 「第n章 おーさー著」と書かれる
  * 指定しなければ空欄
* 章(`=`)が始まる前に書く
* Re:VIEW側の都合で、 `@<author>{ほげ}` と `=` との間には1行以上の空行が必要
  * このためにRedPenで(Re:VIEW構文として認識されず)不正とか言われるが...

### `tabletabooularraw` ブロック
* https://github.com/lrks/tabooular
* 名前が長い
  * `table` または `imgtable` が先頭に付いていないと表として処理されない
  * `raw(後述)` も付けたい
  * アンダースコアや数字を付けるとRe:VIEW構文としてみなされない
  * 仕方ない

### `demathraw` ブロック
* LaTeXのMathがそのままdemath
* LaTeX向けはそのまま出力、HTML向けは一度PDFを経由してSVGへ変換する
* インライン数式は無理

### `tabletabooularwideraw` ブロック
* `table` に対する `table*` にあたる
  * 2段組をぶちぬいて1段で表示させるが、今回はmarginparにはみ出させる
* 「本文+欄外」の領域中央に表示される

### `imagewide` ブロック
* `figure` に対する `figure*` にあたる
* 「本文+欄外」の領域中央に表示される
* `scale=` でスケールを指定している場合、「本文+欄外」に対するスケールとなる

## 注意
### ビルド環境は `SIGCOWW/desk`
* 素のRe:VIEWと若干異なる
* 1文字以上の文字と`raw` で構成されるブロックは、Re:VIEW処理系に渡す前に中身がbase64に変換される
  * Re:VIEWで意味を持つ記号が無視されて嬉しい
  * (拡張したメソッド側で復号する)

### （括弧）や「カギ括弧」！？…など約物は全角
* 約物の前後にはスペースを入れない
* ただし、(kakko) の中に和文を含まない場合は半角括弧を使う
  * そして、「開始の前」と「終了の後」に半角スペースを入れる
  * ただし、他の約物と連続する場合はスペースを入れない

### 画像に関する注意
* (PDF)配置がおかしい
  * 章をまたいで表示しないようにしている
  * が、そのせいでおかしな配置になりそう
  * 問題が出たら修正する
* ファイルパスは `src/images/<chapid>/<id>.<ext>`
  * `src/images/latex` や `src/images/html` は使わない
  * ビルドツールで使っているため、Gitで管理されないばかりか、削除される可能性もある
* フォーマットは PDF/PNG/JPG
  * ターゲット (LaTeX/HTML) に合わせて適切に変換される
    * (PDF) ... フォントを埋め込み、画像余白をゼロにする。また、HTML向けはPDFに変換する。
    * (PNG) ... 画像余白をゼロにする。LaTeX向けは背景色を白とし、透過を含まない画像にする。
    * (JPG) ... Exif情報に従い画像を回転後、すべてのExif情報を削除する。
  * その他のフォーマットはRe:VIEWに依存
* ラスタ画像の解像度は 350ppi 以上を推奨
  * B5判いっぱいに貼り付ける場合は「2508px x 3541px」以上
  * 本文領域いっぱいに貼り付ける場合は、「1600px x 2977px」程度
  * (ただし、2値画像の場合は 1200ppi 以上)
* 入稿用PDF・EPUB・公開用PDFで仕様を変える
  * (入稿用PDF) ... モノクロ・解像度の制限なし
  * (EPUB) ........ カラー・300dpiまでに変換
  * (公開用PDF) ... カラー・90dpiまでに変換

### `tmp` `latex` `html` と付くディレクトリ・ファイルは作らない
* 削除される可能性がある


## 変更予定
### 「著者あとがき」
* しっかりさせる
  * 枠で囲ったり
* あと同じファイルに載せないように？
  * `profile` 環境を作って…記事のどこかに書いてもらって…出力は一番最後にまとめて…

### DockerはROでマウントさせる
* make.sh から docker run で src/ をマウント～はRWでやっていて怖い
* コピーして使おう

### `nest` を Pandoc へ
* MarkdownやreSTの構文が使えるように
* 箇条書きに限らない
* `format=markdown/commonmark/rst` とかpandocの引数を指定できる

### センス無い名前の修正
* `ほげほげwide` は `ほげほげ*` にする
* `ほげほげraw` は `ほげほげ` にする
* `tabletabooularraw` は `tabooular` でいけるようにする

### `なんとかwide` の実装
* 逆方向に寄ることがあった気がする

### `@<author>`
* 記事タイトルと一体化させる
* Re:VIEWでは、 `=[toc] 記事タイトル` って表記があるようなので、踏襲する
* `=[toc,author=ほげほげ] 記事タイトル` って感じかなぁ

### 最新版 `layout.tex.erb` の反映
* Re:VIEWリポジトリに入っている変更を反映させる

### 数式を画像として入れるアレ
* Re:VIEW本体に入ったみたいなので、リリースされたら更新する
* `dvipng` を使うが、現状では(muslのせいで)動かない
* 同じ理由で、`ps2pdf` も動かないので大変
* 直す

### インライン `@<cmd>{hoge}`
* Markdownのバッククオートに相当する機能が欲しい…

### 組版
* 余白
* 記事間にクッションページを挟む
* トンボ廃止
* ベタを5mmのつもりで作る
* 行取り