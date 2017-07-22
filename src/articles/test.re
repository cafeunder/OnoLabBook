@<author>{書き方指南}

= ここにタイトルを書きます
//lead{
ここにはちょっとしたコメントを書きます．
//}

== Re:Viewの書き方
@<href>{https://github.com/kmuto/review/blob/master/doc/format.ja.md}を見てください．

== 章はこれ
＝2つです．

=== 節はこれ
＝3つです．

==== これってなんていうの？
＝4つです．
章や節の前は改行してください．

== 文字と脚注
@<b>{これで文字が太字になります}．@<i>{KORE DATO SYATAIになる．和文は斜体になりません．}
脚注はこれ@<fn>{fn:id}で参照し，
//footnote[fn:id][ここに文章を書きます．]

改行はlatexと同じで改行，空白を2回続けます．

== 表の書き方
=== @<tt>{tabletabooularraw}ブロック
表の書き方を@<table>{table-1}と@<table>{table-2}に示します．
表の参照は@<table>{table-1}とします．
SIGCOWWのリーダーが作ったさいきょう@<fn>{diagbox}の命令だそうです．
//footnote[diagbox][さいきょう過ぎて，斜線は（それを入れるためのパッケージを@<tt>{\usepackage}するとエラーになるため）入らない．]

//tabletabooularraw[table-1][たぶん表]{
ID   | @<b>{Name}     & Age & Job
====>|<=============== ====>&=========
A001 | @<u>{Taro Sato}& 12  & Student
A002 | Takashi Suzuki & 24  & Student
---------------------------------------
B001 | Taro Yamada    & 36  & @<tt>{-}
//}

//tabletabooularraw[table-2][たぶん表]{
ここは           | ヘッダーです．     | ヘッダーは       & ＝を3つ以上で       & 区切ります
================|===================|=================&<===================&======>
＆で             | ｜で              & ー3つ以上で     & 区切りで＜＝とすると & ＝＞とすると
------------------------------
セルを区切ります  | 垂直線を引きます   & 水平線を引きます & 左揃え             & 右揃えです
                & >>>> マルチカラムは＞と＜をぶち抜く数だけ書きます <<<< 
                & >> ＞だけ書くと右揃えに           >>   & << ＜だけ書くと左揃えになります <<
＼でマルチロー \ & >>>> 書くことがなくなってきた <<<<
                & >>>> あんまり凝った表は書かない方が良いと思います <<<< 
------------------------------
666 | 777 & 888 & 999 & SSS
//}

凝ったことをやろうと思えばできますが，実際には@<table>{table-3}程度が現実的な範囲だと思います．

//tabletabooularraw[table-3][ありそうな表]{
           |  >> 既存手法  <<          | >> 提案手法 <<         
==========>|============>&===========>|===========>&==========>
@<i>{pop.} | @<i>{best} & @<i>{gen.}  | @<i>{best} & @<i>{gen.}
-----------
#1         | 959296     & 196         | 959296      & 330.4
#2         | 959291     & 213.4       | 959291      & 324
#3         | 959291     & 202         | 959289      & 196.4
//}

== 数式の書き方
=== @<tt>{demathraw}ブロック
LaTeXの数式をそのまま出す．
これです．
//demathraw{
\sum_{i=1}^nf_n(x)
//}
@<tt>{align*}環境で囲っており，array的なこともできます．
//demathraw{
x & = 1 \\
y & = 1 + 2 + 3
//}

それっぽい式も書けます．
//demathraw{
C_1 = {\bm m} + z_1 e_1 + \sum^{n}_{k=2} z_k e_k.
//}

文章中に数式を埋めるときは@<m>{x^{2y\}}とします．
内側に来る中括弧の終わり｝には＼エスケープが必要な点に注意してください．

== 画像の表示
=== 画像ファイルの配置場所
画像は，OnoLabBook/src/images/[記事名]/に配置します．

=== @<tt>{image}ブロック
図の表示方法を@<img>{pdfimage}と@<img>{pngimage}に示します．
表の参照は@<img>{pdfimage}とします．

//image[pdfimage][画像1][scale=0.5]
//image[pngimage][画像2][scale=0.5]

@<b>{画像を横に並べる(minipage的な)ことは出来ません．}
画像を横並びにしたいならば，inkscapeやpowerpointを使って横並びの画像を作ってください．

== 箇条書き
番号付きは，１．２．とします．箇条書きの前は改行して，数字の間にはスペースを入れてください．

 1. いちばんめ
 2. にばんめ
 3. さんばんめ

番号なしは＊アスタリスクとします．

 * ああああ
 * いいいい
 * うううう
