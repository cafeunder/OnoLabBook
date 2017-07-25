@<author>{野村}

= ベイズ最適化と進化計算
//lead{
本当にベイズ最適化って強いの？
//}

== はじめに
こちらの章では，ベイズ最適化と進化計算という2つの最適化手法を紹介し，実験して挙動を比較してみます．

ベイズ最適化，進化計算はともにBlack-box関数最適化問題に対する手法です．
Black-box関数最適化問題とは，目的関数が数式で与えられず，
勾配情報が得られない問題です．
この問題では，ある解に対する評価値のみが与えられます．まさにBlack-boxです．

機械学習のハイパーパラメータ最適化問題はBlack-box関数最適化問題の1例であり，
近年重要性を増しています．
Black-box関数最適化問題では，目的関数の評価の計算コストが大きいことを前提にしており，
できるだけ評価回数を少なくすることが求められます．
例えば，Deep Learningのハイパーパラメータ最適化では，
1つのハイパーパラメータの組に対する実験が1日かかるということが平気で考えられます．
この場合，最適化手法自体の計算コストより，評価回数をいかに少なくできるかが重要になります．
一般的な勾配法などの最適化が1回の評価がすぐ終わることを前提としているのに対し，
Black-box関数最適化では1回の評価の計算コストが大きいことが前提ということに注意をしてください．

以下では，機械学習のハイパーパラメータ最適化に頻繁に用いられるベイズ最適化と，
state-of-the-artな進化計算手法であるCMA-ESを説明し，
実験により挙動比較を行います．
これまでベイズ最適化を使ったことはあっても，どんな挙動をするのか確認した人はあまり多くないのではないでしょうか．
僕もCMA-ESについては大学院時代にめちゃくちゃ見ていたのですが，ベイズ最適化の挙動を詳細に追ったことはなかったので，
今回の実験でおもしろい性質が見つかればと思います．

なお，本稿では，常に最小化問題を仮定してアルゴリズムを説明するので注意をお願いします．


== ベイズ最適化
=== 概要
ベイズ最適化(Bayesian Optimization; BO)は，Black-box関数最適化問題に対する
有力な手法の一つです．
近年では，機械学習のハイパーパラメータ探索に用いられることが多く，
grid search, random searchと比較して効率良く優れた解を求められることが報告されています@<bib>{bergstra11}．
BOには様々なvariantが存在しますが，今回はシンプルなBOのモデルを説明します．

ベイズ最適化は，以下の操作を繰り返すことで探索を行います．

* Gaussian Process(GP)によって予測した平均，分散を使って計算したacquisition functionを最適化して，解を得る

* 解を評価し，評価値を得る

* 解と評価値の組を保存し，GPを更新する

全体のアルゴリズムを図@<img>{alg_bayes}に示す．

//image[alg_bayes][ベイズ最適化のアルゴリズム][scale=1.0]

=== acquisition function
Black-box関数最適化問題では，目的関数@<m>{f}の評価に時間がかかるため，
代わりとなる関数acquisition functionの最適解を次の探索点とする戦略をとります．
つまり，acquisition functionは，良い評価値を持つ"可能性のある"点の評価値が良くなるように
設計される必要があります．
ある点@<m>{x}に対して，良い評価値の"可能性がある"というのは，

* @<m>{\bm x_s}の近くが探索済みで，おそらく良い評価値を持つ場合

* @<m>{\bm x_s}の近くをまだ探索しておらず，不確実性が高い場合

@<m>{A^{\rm T\}}

のいずれかの場合を指します．
つまり，活用と探索の観点から探索する必要があるかを考えます．

acquisition functionとしては，

* Probability of Improvement (PI)

* Expected Improvement (EI)

* Lower Confidence Bound (LCB)

などが有名であるが，今回はLCBを用いることにする．
@<m>{\bm x}に対するLCBは，以下の式で定義される．
//demathraw{
  {\rm LCB}({\bm x}) = \mathbb{E}[f({\bm x})] - \kappa \sqrt{\mathbb{V}[f({\bm x})]}.
//}
最大化問題の場合は，Lower Confidence Boundの代わりにUpper Confidence Bound (UCB)を用いる．
//demathraw{
  {\rm UCB}({\bm x}) = \mathbb{E}[f({\bm x})] + \kappa \sqrt{\mathbb{V}[f({\bm x})]}.
//}

@<m>{\mathbb{E\}[f({\bm x\})]}, @<m>{\mathbb{V\}[f({\bm x\})]}はGaussian Process (GP)によって求める．


=== Gaussian Process (GP)
@<m>{\mathbb{E\}[f({\bm x\})]}, @<m>{\mathbb{V\}[f({\bm x\})]}をGPで求める方法について説明を行う．
まず，評価値が以下の線形回帰モデルで表されることを仮定する．
@<m>{f}が目的関数，@<m>{y}が観測値である．今回はノイズがない(評価値がそのまま観測値となる)状況を扱う．
//demathraw{
  f({\bm x}) &= {\bm x}^{\rm T}{\bm w},\\
  y &= f({\bm x}).
//}

このとき，
//demathraw{
  \mathbb{E}[f({\bm x})] &= \mathbb{E}[{\bm x}^{\rm T}{\bm w}] = {\bm x}^{\rm T}\mathbb{E}[{\bm w}],\\
  \mathbb{V}[f({\bm x})] &= \mathbb{E}\Bigl[ ({\bm x}^{\rm T}{\bm w} - \mathbb{E}[{\bm x}^{\rm T}{\bm w}])({\bm x}^{\rm T}{\bm w} - \mathbb{E}[{\bm x}^{\rm T}{\bm w}])^{\rm T} \Bigr]\\
  &= {\bm x}^{\rm T} \mathbb{E}[({\bm w} - \mathbb{E}[{\bm w}])({\bm w} - \mathbb{E}[{\bm w}])^{\rm T}] {\bm x}\\
  &= {\bm x}^{\rm T} \mathbb{V}[{\bm w}] {\bm x}.
//}

ここで，事前分布@<m>{p({\bm w\})}が正規分布，尤度@<m>{p(y|{\bf X\}, {\bm w\})}も正規分布と仮定すると，
事後分布@<m>{p({\bm w\}|{\bf X\}, y)}も正規分布となるため，
@<m>{\mathbb{E\}[{\bm w\}], \mathbb{V\}[{\bm w\}]}が求まる．
上記のように，GPでは，探索点と観測値の組@<m>{({\bf X\}, {\bm y\})}を使って，
ある点@<m>{\bm x}に対する評価値の平均@<m>{\mathbb{E\}[f({\bm x\})]}, 
分散@<m>{\mathbb{V\}[f({\bm x\})]}を計算する．
基底関数@<m>{\phi}を使い@<m>{\bm x}を変換することで非線形の場合にも拡張できる．


== CMA-ES
CMA-ES@<bib>{hansen11}はBlack-box関数最適化に対する最も有力な手法の1つである.
CMA-ESの全体のアルゴリズムを以下に示す.


== 実験


== おわりに
