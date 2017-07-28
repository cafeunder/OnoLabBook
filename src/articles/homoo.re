<<<<<<< HEAD
= ┌（┌ ＾p＾）┐
//lead{
nintendo switch，スプラトゥーン2が運よく手に入ってしまったがために，進捗を生み出せないものと化したあこんぼです．

スプラ最高！
//}

== はじめに
本章では，フリーの物理エンジンであるode4jを使ってモデリングされた四足歩行ロボットに歩行を学習させます．

最近の流行はやはりディープラーニングとかのニューラルネットワークを使う系なので，そいつを使って学習させようとも考えたのですが，

== ODE( Open Dynamics Engine )
ODE @<bib>{ode} はフリーの物理演算シミュレータです．c++から使えます．

今回の実験に用いたロボットはODEをjava用に拡張したode4j @<bib>{ode4j} を使ってモデリングされています．

== 四足歩行ロボット( HOMOO )
本章では，歩行学習に用いる四足歩行ロボット「HOMOO」の説明をします．

HOMOO は こんな感じ┌（┌ ＾p＾）┐のロボットです．ODE 上でモデリングされており，@<img>{homoo_image}のような形をしています．

@<img>{homoo_image2}のように胴体の部分に四つの肩が接続され，それぞれの肩に腕が接続されています．
肩は地上に垂直な方向を軸として回転でき，腕は肩の面に対して垂直な方向を軸として回転できます．

HOMOO がこんな風 └(└＾o＾)┘ になってしまわないように各関節の可動域を@<m>{[ -\frac{\pi\}{6\}, \frac{\pi\}{6\} ]}[rad]と制限しています．

//image[homoo_image][HOMOO 静止中][scale=1.0]
//image[homoo_image2][HOMOO 関節の設定][scale=1.0]

HOMOO はシミュレーション中の各フレームごとに各関節の角速度を適切に与えることによって，動くようになっています．

今回はこの HOMOO を前進し続けられるようにすることが目的となります．

== HOMOO の歩行学習
本章では，HOMOO に歩行を学習させる手順について説明します．
前章でも述べた通り，今回の目的は HOMOO を前進し続けられるようにすることです．

前提として，歩行という行動は周期的な運動から成り立つと言えます@<fn>{fn:1}．
//footnote[fn:1][たとえば人間の歩行は，「右足を出した後に左足を出す」という行為の繰り返しです．]

そのため，HOMOO の各関節を何かしら周期的に動かすことで，HOMOO を歩かせることが可能だと考えられます．

そこで，今回は100フレームを1周期とし，さらに10フレームごとに各関節の目標の角度を決定し，（目標の角度 - 現在の角度）を角速度として与えることとします．決定すべきパラメータは，10フレームごとの各関節の目標の角度であり，駆動できる関節が8つあることから，全部で80パラメータとなります．

後は，これらの80パラメータの最適化を行い， HOMOO が歩けるようなパラメータを探索します．

== 最適化手法
本章では，HOMOOの歩行学習に用いる最適化手法について説明します．
説明のため，対象とする最適化問題を次のように定式化しておきます．
//demathraw{
{\rm minimize \ } f(x), \\
{\rm subject \  to \ } x \in R^{80} .
//}
ただし，@<m>{f(x)}は評価関数です．具体的な定義は次の章で説明します．


今回は2種類の最適化手法を用いて HOMOO の歩行を学習させます．

=== 自然進化戦略(NES)
1つめの最適化手法は 自然進化戦略(NES)[Wierstra, 2008]を高速化した手法です．
ここではNESについて簡単に説明します．NESは，関数@<m>{f(x)} を直接探索するのではなく，@<m>{x} についての確率分布@<m>{p(x | \theta)} における，@<m>{f(x)} の期待値 @<m>{{\rm E\}[ f(x) ]} を最小化する @<m>{p(x | \theta)} のパラメータ@<m>{\theta}を探索します．確率分布@<m>{p} には基本的に多変量正規分布が用いられ，平均ベクトルと分散共分散行列を最適化するように定式化されます．

紙面の都合や，時間の都合や，その他もろもろの都合によりざっくりとアルゴリズムを説明します．
NES は以下のようにして，@<m>{p}のパラメータ@<m>{\theta}を探索します．

//demathraw{
{\rm 1. \ \ }& \theta の初期値 \theta_0 を与え，確率分布 p を p(x | \theta_0) とし，世代数 t \leftarrow 0 とする．\\
{\rm 2. \ \ }& {\rm E}[f(x)] の勾配を g(\theta_t) に g(\theta_t) \leftarrow \nabla_{\theta} \ {\rm E}[f(x)] として保存する．\\
{\rm 3. \ \ }& フィッシャー情報行列 F(\theta_t) を \\
\ \ \ \ & F(\theta_t) \leftarrow {\rm E}[ \nabla_{\theta} {\rm \ ln\ }p(x | \theta_t) (\nabla_{\theta} {\rm E}[ {\rm \ ln\ }p(x | \theta_t) ] )^{\rm T}] と計算する．\\
{\rm 4. \ \ }& \theta を \theta_{t + 1} \leftarrow \theta_t - \eta F(\theta_t) g(\theta_t) として更新する．\\
{\rm 5. \ \ }& 終了条件を満たしてれば \theta \leftarrow \theta_{t+1} として終了する．\\
\ \ \ \ & さもなければ，t \leftarrow t + 1 として2へ戻る．
//}
ただし，@<m>{\eta} はユーザパラメータです．対象とする問題によって適宜与えましょう．

=== Adam
2つめの最適化手法は Adam[Diederik, 2015] と呼ばれる手法です．
Adam は1次勾配のみを用いる手法でありながら，1次勾配の1次と2次モーメント(平均と分散)を推定し，1次勾配のスケーリングに対しての不偏性を得ることで勾配降下法や確率的勾配降下法を遥かに上回る性能を持つ手法です．

以下のようにして最適な解 @<m>{x} を探索します．
//demathraw{
{\rm 1. \ \ }& x の初期値 x_0 を与え, m_0 \leftarrow 0, v_0 \leftarrow 0, t \leftarrow 0 とする．\\
{\rm 2. \ \ }& 世代数 t を t \leftarrow t + 1 と更新する． \\
{\rm 3. \ \ }& f の勾配を g_t に g_t \leftarrow \nabla_{x} \  f_t(x_{t-1}) として保存する．\\
{\rm 4. \ \ }& 勾配の1次モーメント m_t を m_t \leftarrow \beta_1 \cdot m_{t-1} + (1-\beta_1) \cdot g_t と計算する．\\
{\rm 5. \ \ }& 勾配の2次モーメント v_t を v_t \leftarrow \beta_2 \cdot v_{t-1} + (1-\beta_2) \cdot g_t \odot g_t と計算する．\\
{\rm 6. \ \ }& m_t, v_t の初期化によるバイアスを打ち消すため，\hat{m}_t, \hat{v}_t を \\ 
\ \ \ \ & \hat{m}_t \leftarrow m_t / (1-\beta^t_1) , \hat{v}_t \leftarrow v_t / (1-\beta^t_2) と計算する．\\
{\rm 7. \ \ }& \hat{m_t}, \hat{v_t} を用いて解 x_t を \\
\ \ \ \ & x_t \leftarrow x_{t-1} - \alpha \cdot \hat{m}_t / (\sqrt{\hat{v}_t + \epsilon}) として更新する．\\
{\rm 8. \ \ }& 終了条件を満たしていれば x \leftarrow x_t として終了する．さもなければ 2 へ戻る．
//}
ただし，@<m>{\alpha, \beta_1, \beta_2, \epsilon} はAdamのハイパーパラメータであり，
@<m>{\alpha = 0.001, \beta_1 = 0.9, \beta_2 = 0.999, \epsilon = 10^{-8\}}
として推奨値が与えられています@<fn>{alp}．

//footnote[alp][@<m>{\alpha} はかなり敏感なパラメータとなっているので注意しましょう．]

== 実験設定
本章では実験の設定について説明します．最適化対象とされる@<m>{x}は「HOMOO の歩行学習」の章で説明したように，80パラメータで表される目標角度の集合とします．評価関数の値，すなわち評価値に関しては与えられた目標角度の集合@<m>{x}に対して@<img>{simu}ようにシミュレーションを行うことで計算します．

//image[simu][HOMOO の歩行シミュレーションの流れ][scale=0.85]

== 歩行学習結果
Adam NES の比較

== おわりに
今後の課題とか

== 参考文献
[Diederik, 2015]
Diederik P.Kingma, Jimmy Lei Ba, Adam : A Method for Stochastic Optimization, 3rd International Conference for Learning Representations, San Diego, 2015（arXiv:1412.6980v9, 30 Jan 2017）

[Wierstra, 2008]
Daan Wierstra, Tom Schaul, Jan Peters and Juergen Schmidhuber, Natural Evolution Strategies, 2008 IEEE Congress on Evolutionary Computation (CEC 2008)
=======
= homoo
//lead{
こんちは
//}

== ほげほげ
//demathraw{
\sum_{n = 1}^N f(n) \\ 
\epsilon \\ 
\theta = \theta + \eta F^{-1}(\theta) J(\theta)
//}

@<img>{test/test}と@<img>{test}はちゃんと表示されるかな？？？

//image[test/test][画像A][scale=1.0]
//image[test][画像B][scale=1.0]
>>>>>>> c39b4119a8fd5cfb1fc55df4d7d9ad775fddac35

