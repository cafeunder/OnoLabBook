@<author>{nmasahiro}

= ベイズ最適化と進化計算

== はじめに
本稿では，ベイズ最適化と進化計算という2つの最適化手法を紹介し，実験により挙動比較を行う．
ベイズ最適化，進化計算はともにBlack-box関数最適化問題に対する有力な手法である．

Black-box関数最適化問題とは，目的関数が代数的な表現で与えられず，
勾配情報が得られない問題である．
この問題では，ある解に対する評価値のみが与えられる．

機械学習のハイパーパラメータ最適化問題はBlack-box関数最適化問題の1例であり，
近年重要性を増している．
Black-box関数最適化問題では，目的関数の評価に時間がかかるということを前提にしている．
例えば，Deep Learningのハイパーパラメータ最適化では，
1つのハイパーパラメータの組(例としては，バッチサイズ，各層のユニット数，ドロップアウト率，SGDの学習率など)に対する実験が1日かかるということが容易に考えられる．
一般的な勾配法などの最適化では1回の評価がすぐ終わることを前提としているのに対し，
Black-box関数最適化では1回の評価の計算コストが大きいため，
探索にかかる評価回数をいかに少なくできるかが重要になる．

以下では，機械学習のハイパーパラメータ最適化に頻繁に用いられるベイズ最適化と，
state-of-the-artな進化計算手法の1つであるCMA-ESを説明し，
実験により挙動比較を行う．

なお，本稿では，常に最小化問題を仮定してアルゴリズムを説明することに注意していただきたい．


== ベイズ最適化
=== 概要
ベイズ最適化(Bayesian Optimization; BO)@<bib>{brochu10}は，Black-box関数最適化問題に対する
有力な手法の一つである．
近年では，機械学習のハイパーパラメータ探索に用いられることが多く，
grid search, random searchと比較して効率良く優れた解を求められることが報告されている@<bib>{bergstra11}．
BOには様々なvariantが存在するが，今回はシンプルなBOのモデルを説明する．

BOは，以下の操作を繰り返すことで探索を行う．

 * Gaussian Process(GP)によって予測した平均，分散を使って計算したacquisition functionを最適化して，解を得る

 * 解を評価し，評価値を得る

 * 解と評価値の組を保存し，GPを更新する

全体のアルゴリズムを@<img>{alg_bayes}に示す．

//image[alg_bayes][ベイズ最適化(BO)のアルゴリズム][scale=1.0]

以下では，acquisition functionとGaussian Process(GP)について説明を行う．

=== acquisition function
Black-box関数最適化問題では目的関数@<m>{f}の評価に時間がかかるため，
BOは代わりとなる関数acquisition functionの最適解を次の探索点とする戦略をとる．
つまり，acquisition functionは，良い評価値を持つ"可能性のある"点の評価値が良くなるように
設計される必要がある．
ある点@<m>{x}に対して，良い評価値の"可能性がある"というのは，

 * @<m>{\bm x}の近くをまだ探索しておらず，不確実性が高い場合

 * @<m>{\bm x}の近くが探索済みで，おそらく良い評価値を持つ場合

のいずれかの場合を指す．
つまり，"探索"と"活用"のトレードオフを考慮した最適化を行う必要がある．

acquisition functionとしては，

 * Probability of Improvement (PI)

 * Expected Improvement (EI)

 * Lower Confidence Bound (LCB)

などが有名であるが，今回はLCBを用いる．
@<m>{\bm x}に対するLCBは，以下の式で定義される．@<m>{\kappa}はユーザパラメータである．
//demathraw{
  {\rm LCB}({\bm x}) = \mathbb{E}[f({\bm x})] - \kappa \sqrt{\mathbb{V}[f({\bm x})]}.
//}
最大化問題の場合は，Lower Confidence Boundの代わりにUpper Confidence Bound (UCB)を用いる．
//demathraw{
  {\rm UCB}({\bm x}) = \mathbb{E}[f({\bm x})] + \kappa \sqrt{\mathbb{V}[f({\bm x})]}.
//}

@<m>{\mathbb{E\}[f({\bm x\})]}, @<m>{\mathbb{V\}[f({\bm x\})]}はGaussian Process (GP)によって求める．

なお，今回はacquisition functionの最適化にL-BFGS-B法を用いている．

=== Gaussian Process (GP)
評価関数@<m>{f}の期待値 @<m>{\mathbb{E\}[f({\bm x\})]} と分散 @<m>{\mathbb{V\}[f({\bm x\})]} をGPで予測する方法について説明を行う．
GPでは，探索点と観測値の組を使って，ある点@<m>{\bm x}の評価値の期待値と分散を予測する．

評価値が以下の線形回帰モデルで表されることを仮定する．
@<m>{f}が目的関数，@<m>{y}が観測値である．今回はノイズがない(評価値がそのまま観測値となる)状況を扱う．
//demathraw{
  f({\bm x}) &= {\bm x}^{\rm T}{\bm w},\\
  y &= f({\bm x}).
//}

このとき，@<m>{\mathbb{E\}[f({\bm x\})]}，@<m>{\mathbb{V\}[f({\bm x\})]}はそれぞれ以下のように計算される．
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


== CMA-ES
CMA-ES@<bib>{hansen11}はBlack-box関数最適化に対するstate-of-the-artな手法の1つである．
CMA-ESは

 * 多変量正規分布@<m>{\mathscr{N\}({\bm m\}, \sigma^2{\bf C\})}から解@<m>{\bm x}を生成

 * 生成した解の情報をもとに正規分布を更新

を繰り返すことで，最適解のみに解を生成するような正規分布をつくることが目標になる．
CMA-ESの探索の流れを@<img>{cma}に示す．
//image[cma][CMA-ESの探索の流れ．星が最適解，楕円が多変量正規分布，点が生成された解を表す．][scale=1.0]


全体のアルゴリズムを以下に説明する．

 * Step1. 解生成
 ** 以下の式に従って解を@<m>{\lambda}個生成する．ここで@<m>{{\bm z\}_i}は@<m>{\mathscr{N\}({\bf 0\}, {\bf I\})}に従う乱数である．
//demathraw{
  {\bm x}_i = {\bm m} + \sigma \sqrt{\bf C} {\bm z}_i \sim \mathscr{N}({\bm m}, \sigma^2 {\bf C}).
//}

 * Step2. 進化パスの更新
 ** 進化パス@<m>{{\bm p\}_{\sigma\}, {\bm p\}_C}を以下の式で更新する．
  ここで@<m>{c_{\sigma\}, c_c}はそれぞれ@<m>{{\bm p\}_{\sigma\}, {\bm p\}_C}の学習率であり，
  @<m>{\mu_{\rm eff\} = (\sum_{i=1\}^{\mu\}w_i^2)^{-1\}}，
  @<m>{h_{\sigma\}}は@<m>{t}を世代数として@<m>{\|{\bm p\}_{\sigma\}\|^2/d < (2+4/(d+1))(1-(1-c_{\sigma\})^{2t\})}のとき
  @<m>{1}，そうでないとき@<m>{0}となる関数，
  @<m>{w_i}は@<m>{\sum_{i=1\}^{\mu\}w_i=1}を満たす重み関数，
  @<m>{\mu}はユーザパラメータである．
  また，@<m>{i:\lambda}は@<m>{\lambda}個体中@<m>{i}番目に良い評価値を持つ個体の添え字を表す．
//demathraw{
  {\bm p}_{\sigma} &\leftarrow (1-c_{\sigma}){\bm p}_{\sigma} + \sqrt{c_{\sigma}(2-c_{\sigma})\mu_{\rm eff}}\sum_{i=1}^{\mu}w_i{\bm z}_{i:\lambda},\\
  {\bm p}_{c} &\leftarrow (1-c_c){\bm p}_{c} + \frac{h_{\sigma}\sqrt{c_{\sigma}(2-c_{\sigma})\mu_{\rm eff}}}{\sigma} \sum_{i=1}^{\mu}w_i({\bm x}_{i:\lambda}-{\bm m}).
//}

 * Step3. パラメータの更新
 ** 平均ベクトル，ステップサイズ，共分散行列の更新を以下の式で行う．
  ここで@<m>{c_1, c_{\mu\}}は学習率であり，@<m>{d_{\sigma\}}はユーザパラメータである．
//demathraw{
  {\bm m} &\leftarrow {\bm m} + c_{m}\sum_{i=1}^{\mu}w_i ({\bm x}_{i:\lambda} - {\bm m}),\\
  \sigma &\leftarrow \sigma \exp((c_{\sigma}/d_{\sigma}) (\|{\bm p}_{\sigma}\|/ \mathbb{E}\| \mathscr{N}({\bf 0}, {\bf I}) \| - 1)),\\
  {\bf C} &\leftarrow (1-c_1-c_{\mu}){\bf C} + (1-h_{\sigma})c_1 c_c(2-c_c){\bf C} \\
     &+ c_1 {\bm p}_c {\bm p}_c^{\rm T} + c_{\mu} \sum_{i=1}^{\mu} w_i ({\bm x}_{i:\lambda}-{\bm m})({\bm x}_{i:\lambda}-{\bm m})^{\rm T}.
//}
CMA-ESではStep1からStep3を繰り返すことにより探索を進める．

数式だけ見ると複雑に見えるが，やりたいことはシンプルで，

 * 平均ベクトル @<m>{\bm m} : 最適解の方向に動かす

 * ステップサイズ @<m>{\sigma} : 分布の外側に最適解があるなら分布を拡大，内側にあるなら分布を縮小

 * 共分散行列 @<m>{\bf C} : 評価値がよくなるように分布の形状を更新

というだけである．共分散行列の更新式は自然勾配に由来する@<bib>{akimoto12}．


== 実験
=== 目的
BOとCMA-ESの挙動比較を行うため，ベンチマーク関数に対する挙動をViewerで確認する．

=== ベンチマーク関数
実験では，単峰性関数であるSphere関数と，多峰性関数であるHimmelblau関数を用いる．
Sphere関数は@<m>{f(x, y) = x^2 + y^2}，@<m>{(0, 0)}が最適解である．
Himmelblau関数は@<m>{f(x, y) = (x^2 + y - 11)^2 + (x + y^2 - 7)^2}，
@<m>{(3.0, 2.0), (-2.805118, 3.131312), (-3.779310, -3.283186), (3.584428, -1.848126)}の
4つの最適解を持つ．

Sphere関数，Himmelblau関数の景観をそれぞれ@<img>{contour_sphere}，@<img>{contour_himmelblau}に示す．


=== 実験設定
BO，CMA-ESそれぞれのパラメータ設定を以下に示す．
CMA-ESのそれ以外のパラメータは推奨値@<bib>{hansen11}を用いている．

==== BO

 * 探索区間(Sphere関数) : @<m>{([-1, 1], [-1, 1])}
 * 探索区間(Himmelblau関数) : @<m>{([-4, 4], [-4, 4])}
 * GPのカーネル : Matern
 * GPのカーネルパラメータ@<m>{\nu} : @<m>{1.5}
 * 初期点の数 : @<m>{5}
 * L-BFGS-B法のiteration数 : @<m>{250}
 * LCBの計算に用いるパラメータ@<m>{\kappa} : @<m>{2.576}


==== CMA-ES

 * 初期平均ベクトル@<m>{\bm m} : @<m>{\bm 0}
 * 初期ステップサイズ@<m>{\sigma}(Sphere関数) : @<m>{0.5}
 * 初期ステップサイズ@<m>{\sigma}(Himmelblau関数) : @<m>{2.0}
 * 初期共分散行列@<m>{\bf C} : @<m>{\bf I}

//image[contour_sphere][Sphere関数の景観][scale=0.6]
//image[contour_himmelblau][Himmelblau関数の景観][scale=0.6]

=== 結果
Sphere関数に対するBOの探索描画を@<img>{bo_sphere_pdf}，
Sphere関数に対するCMA-ESの探索描画を@<img>{cma_sphere_pdf}，
Himmelblau関数に対するBOの探索描画を@<img>{bo_himmelblau_pdf}，
Sphere関数に対するCMA-ESの探索描画を@<img>{cma_himmelblau_pdf}に示す．

=== 考察
==== @<img>{bo_sphere_pdf}
BOでは，はじめに探索区間の四隅を探索している．これは周りに解が存在せず，不確実性が高いためである．
探索された解が増えるにつれて，最適解周辺に解が生成されている．
BOでは，序盤に探索，後半に活用というトレードオフを考慮することで，
効率良い探索を行うことができている．

==== @<img>{cma_sphere_pdf}
CMA-ESでは，最適解を囲っていると判断した後は，
最適解に対して素早く多変量正規分布を収束させている．
CMA-ESもBOとは別の方法で探索と活用に対応する操作をしているといえる．

==== @<img>{bo_himmelblau_pdf}
Himmelblau関数に対しても，Sphere関数の場合と同様に四隅を探索していることが観察される．
その後は右上の最適解に向かって活用を行い，探索を効率的に進めている．
右上以外をほぼ探索していない点が気になるが，結果としては1つの最適解に到達している．

==== @<img>{cma_himmelblau_pdf}
CMA-ESでは，序盤から分布が発散傾向にある．
これはCMA-ESのステップサイズの更新方法に理由がある．
CMA-ESでは，有望な方向が上下で複数ある場合，

 * ステップサイズ@<m>{\sigma} : 外に有望領域があると判断し，分布を拡大する

 * 平均ベクトル@<m>{\bm m} : 上の有望領域と下の有望領域が引っ張り合い，平均ベクトルは移動しない

という挙動になる．
これにより，分布は移動せずにステップサイズだけ大きくなり，収束が遅れてしまったと考えられる．

=== 個人的な両手法の良いところ
BO，CMA-ESともにいい手法であるが，
問題に合わせてうまく手法を使い分けることは簡単ではない．
以下に両手法の良いところについて個人的な所感を述べる．

==== BO
 * 探索と活用という直感的にわかりやすい挙動である

 * 多峰性でもそれなりにいい解がそこそこの評価回数で求まる

==== CMA-ES
 * 設定すべきパラメータが少ない
 ** 初期分布の設定以外は，推奨値が与えられている
 ** 初期分布の設定についても，多峰性関数でなければロバスト

 * 高次元(100次元程度まで)にも適用可能
 ** BOでは探索空間全体をモデル化するため必要な計算時間が指数的に増加するが，CMA-ESは必要な部分空間だけ探索すれば良い
 ** ただ，時間計算量が次元数の3乗のため，100次元を越えると厳しいかもしれない

== おわりに
本稿では，ベイズ最適化とCMA-ESの挙動比較を行った．
ベイズ最適化，CMA-ESそれぞれについて説明を行い，ベンチマーク関数に対する挙動をViewerで確認した．
最後に，それぞれの手法の良いところを述べた．

今後の課題として，以下が挙げられる．

 * 高次元の問題に対する挙動比較

 * 異なる性質(変数間依存性など)を有するベンチマーク関数への適用

 * Deep Learningのハイパーパラメータ最適化への適用

=== Source Code
今回の実験で用いたSource CodeをGithub@<fn>{github_nomura}に公開している．
ベイズ最適化，CMA-ESどちらも非常にシンプルに書いてあるので，
興味のある方は是非参照していただきたい．
//footnote[github_nomura][https://github.com/ RosenblockChainers/bayes_vs_cma]

//image[bo_sphere_pdf][Sphere関数に対するBOの探索描画．星が最適解，点が解を表す．(左上)初期点のみで評価回数5， (右上)評価回数10，(左下)評価回数16，(右下)評価回数30．][scale=0.9]
//image[cma_sphere_pdf][Sphere関数に対するCMA-ESの探索描画．星が最適解，点が解を表す．(左上)評価回数6， (右上)評価回数18，(左下)評価回数36，(右下)評価回数48．][scale=0.9]
//image[bo_himmelblau_pdf][Himmelblau関数に対するBOの探索描画．星が最適解，点が解を表す．(左上)初期点のみで評価回数5， (右上)評価回数11，(左下)評価回数20，(右下)評価回数45．][scale=0.9]
//image[cma_himmelblau_pdf][Sphere関数に対するCMA-ESの探索描画．星が最適解，点が解を表す．(左上)評価回数6， (右上)評価回数48，(左下)評価回数120，(右下)評価回数174．][scale=0.9]
