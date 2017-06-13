@<author>{手酢都太郎}

= テスト用文章
//lead{
これは正式な文章の代わりに入れて使うダミーテキストです。なお、組見本の「組」とは文字組のことです。活字印刷時代の用語だったと思います。これは正式な文章の代わりに入れて使うダミーテキストです。この組見本は自由に複製したり頒布することができます。書体やレイアウトなどを確認するために用います。本文用なので使い方を間違えると不自然に見えることもありますので要注意。
//}


== ビルドツールで行う処理
@<tt>{compile.rb}@<fn>{compile}で行う処理について説明します。
//footnote[compile][Dockerコンテナ@<tt>{lrks/desk}内部で使っているビルドツール]

 1. @<tt>{./catalog.yml}を読む
 2. @<tt>{ - hoge.re}という記載から@<tt>{articles/hoge.re}を読む
 3. 末尾に@<tt>{raw}と付くブロックは内容をbase64でエンコード
 4. @<tt>{./hoge.re}に書く
 5. 画像とか最適化する
 6. @<tt>{review-preproc}に渡す
 7. @<tt>{tmpRANDOMHEX/}的なディレクトリを作って@<tt>{catalog.yml}を保存
 8. コンパイルしてみる
 9. エラーがあれば、そのファイルを除いた@<tt>{catalog.yml}を作って再度コンパイル
 10. 適当に繰り返す

で処理する。


== 拡張について
@<tt>{review-ext.rb}による拡張と、モンキーパッチについて説明します。

=== @<tt>{tabletabooularraw}ブロック
ぼくがかんがえたさいきょう@<fn>{diagbox}の表マークアップを組み込んだ。
@<tt>{raw}とあるので、Re:VIEW構文の影響を受けない。

@<table>{table-1}がそれ、@<table>{table-2}は標準の表。
//footnote[diagbox][さいきょう過ぎて、斜線は（それを入れるためのパッケージを@<tt>{\usepackage}するとエラーになるため）入らない。]

//tabletabooularraw[table-1][たぶん表]{
ID   | @<b>{Name}     & Age & Job
====>|<=============== ====>&=========
A001 | @<u>{Taro Sato}& 12  & Student
A002 | Takashi Suzuki & 24  & Student
---------------------------------------
B001 | Taro Yamada    & 36  & @<tt>{-}
//}

//table[table-2][表かな]{
ID		@<b>{Name}		Age	Job
------------------------------------
A001	@<u>{Taro Sato}	12	Student
A002	Takashi Suzuki	24	Student
B001	Taro Yamada		36	@<tt>{-}
//}

なお、NameのところはBoldを指定している。

=== @<tt>{demathraw}ブロック
LaTeXの数式をそのまま出す。
これです。
//demathraw{
\sum_{i=1}^nf_n(x)
//}
@<tt>{align*}環境で囲っており、array的なこともできます。
//demathraw{
x & = 1 \\
y & = 1 + 2 + 3
//}

ちなみにこれが標準@<tt>{texequation}です。
//texequation{
\sum_{i=1}^nf_n(x)
//}

=== @<tt>{author}命令
なんか著者名が出る。

=== モンキーパッチ
モンキーパッチをしています。

==== 表の配置
表配置を@<tt>{!htbp}にしている。
図配置は@<tt>{layout.erb.tex}でいじれるが、なぜ表は…。

==== 和文Bold
最後が和文となるときに@<tt>{\textbf{これ}}は不適当らしい。
それを修正する。


== 画像とか最適化
@<tt>{README.md}を見てくれ。

//image[pdfimage][はい画像][scale=0.5]
//image[pngimage][はい画像2][scale=0.5]
//image[exifjpg][はい画像3][scale=0.5]

サンプルが悪いんだよね。

== 組版について
これどう？

=== 見出し
==== 章
改行不可避。

==== ちなみに
このレベルまでしか手を入れていません。

===== これ
のレベルは標準。


=== 脚注
脚注 (footnote) から傍注 (marginpar) へ。

ただ、LaTeX側で@<tt>{\renewcommand{\footnote}{...}}としており、Re:VIEW側のソースコードはいじっていない。
@<tt>{footnotemark}やらその辺はオプションで有効にできるらしいが面倒なので無視する。

また、本来LaTeXの傍注は番号なしだが、専用のカウンタを用意して採番している。
さらに、画像の挿入は試していないが、たぶん無理でしょう。
ちなみに頑張れば、図表を本文領域外（傍注の領域）にはみ出して配置できそう@<fn>{minipage}だが、頑張りたくないので無視する。

//footnote[minipage][実際、「目次」と「奥付ページ」は@<tt>{minipage}を使って広げている。]
