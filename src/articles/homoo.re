@<author>{あこもど}

= @<m>{\epsilon}＝@<m>{\epsilon}＝┌（┌ ＾p＾）┐
//lead{
nintendo switch，スプラトゥーン2が運よく手に入ってしまったがために，進捗を生み出せないものと化したあこもどです．

スプラ最高！

というわけで，四足歩行ロボットの歩行学習について書いていきます．
強化学習と見せかけて実は進化計算を使ってます．
//}

== はじめに
本章では，フリーの物理エンジンであるode4jを使ってモデリングされた四足歩行ロボットに2つの最適化手法を使って歩行を学習させてみます．そして，ロボットはきちんと歩行を学習するのか，2つの手法でどのような違いが出るのかについて確認しようと思います．

今回は人間があらかじめ決めた手順に基づいてロボットを動かすこととして，その手順のうちどのくらい動かすかなどのパラメータを最適化していきます．
最近はやりのディープラーニングは使いません@<fn>{fn:c}．
ロボットが周期的に動くことを前提として，1周期の中での各制御部についてあらかじめ動きを決定します@<fn>{fn:a}．

//footnote[fn:c][最近の流行はやはりディープラーニングとかのニューラルネットワークを使う系なので，そいつを使って学習させようとも考えたのですが自分の力及ばずうまく学習できませんでした．理由は多々考えられるのですがここでは省略します]
//footnote[fn:a][ディープラーニングを使って学習するものは，ロボットの状態などから次の動きを決定するフィードバック制御が主ですが，今回の方法はフィードフォワードな制御であるといえます．]

最適化手法には，進化計算の中でも有力な手法である自然進化戦略(NES)@<bib>{nes}を高速化した手法と，ディープラーニングにおけるニューラルネットワークの最適化によく用いられている Adam@<bib>{adam} を使います．

Adam は勾配法ベースの手法です．勾配法ベースの手法は最適化対象として与えられる関数の微分情報が利用できる場合には非常に強力です．しかし，微分情報が利用できない場合に基本的に使用されません@<fn>{fn:b}．そのため，今回は関数の微分情報を数値的に求めることで，Adam を利用しようと思います．

NESを含む進化計算は，関数の微分情報を用いないため，最適化対象として与えられる関数の微分情報が与えられない場合にも強力な手法です．今回のような問題設定ではよい性能を発揮することが期待されます．

本章では，4.2節で四足歩行ロボットのモデルを紹介し，その制御方法について説明します．4.3節で最適化手法である，NES と Adam について説明します．4.4節で四足歩行ロボットの歩行学習の手順とそれぞれの最適化手法における歩行学習結果を示します．4.5節で本章をまとめます．

//footnote[fn:b][1階微分（1次勾配）が必須とされることが多いです．]

== 四足歩行ロボット( HOMOO )
本章では，歩行学習に用いる四足歩行ロボット「HOMOO」の説明をします．
HOMOO は こんな感じ┌（┌ ＾p＾）┐のロボットです．ODE 上でモデリングされており，@<img>{homoo_image}のような形をしています．

//footnote[fn:ode][ODE @<bib>{ode} はフリーの物理演算シミュレータです．c++から使えます．今回の実験に用いたHOMOOはODEをjava用に拡張したode4j @<bib>{ode4j} を使ってモデリングされています．]

@<img>{homo3}のように胴体の部分に四つの肩が接続され，それぞれの肩に腕が接続されています．
@<img>{homo2}の左下の部分に示すように，肩は地上に垂直な方向を軸として回転でき，腕は肩の面に対して垂直な方向を軸として回転できます．
実際に関節を動かした例として，@<img>{homo1}に左下の肩と腕を@<m>{\frac{\pi\}{6\}}だけ回転させた HOMOO を示します．

また，@<img>{homo4}のようにHOMOO がこんな風 └(└＾o＾)┘なってしまうとHOMOO の歩行がうまくいかないことが分かります．そのため，シミュレーション中ではこんな風└(└＾o＾)┘ になってしまわないように各関節の可動域を@<m>{[ -\frac{\pi\}{6\}, \frac{\pi\}{6\} ]}[rad]と制限しています．

//image[homoo_image][HOMOO 静止中][scale=1.0]
//image[homo3][HOMOO の肩と腕][scale=1.0]
//image[homo2][HOMOO の肩と腕それぞれの関節の駆動方向][scale=1.0]
//image[homo1][HOMOO の肩と腕を動かした結果．左下の肩と腕が@<m>{\frac{\pi\}{6\}}だけ回転している．][scale=1.0]
//image[homo4][こんな風 └(└＾o＾)┘になってしまったHOMOO][scale=1.0]

HOMOO はシミュレーション中の各フレームごとに各関節の角速度を適切に与えることによって，動くようになっています．

今回はこの HOMOO を前進し続けられるようにすることが目的となります．

=== HOMOO の制御方法
HOMOO の紹介が終わったところで，HOMOO を歩行させるための制御方法について説明します．
前に述べた通り，今回の目的は HOMOO を前進し続けられるようにすることです．

前提として，歩行という行動は周期的な運動から成り立つと言えます@<fn>{fn:1}．
//footnote[fn:1][たとえば人間の歩行は，「右足を出した後に左足を出す」という行為の繰り返しです．]

そのため，HOMOO の各関節を何かしら周期的に動かすことで，HOMOO が歩くように制御することが可能だと考えられます．

そこで，今回は100フレームを1周期とし，さらに10フレームごとに各関節の目標の角度を決定し，（目標の角度 - 現在の角度）を角速度として与えることとします．決定すべきパラメータは，10フレームごとの各関節の目標の角度であり，駆動できる関節が8つあることから，全部で80パラメータとなります．

後は，これらの80パラメータの最適化を行い， HOMOO が歩けるようなパラメータを探索します．

=== ODE(Open Dynamics Engine)
本節の最後に，モデリングのために用いた物理シミュレータODEを紹介しておきます．

ODE @<bib>{ode} はフリーの物理演算シミュレータです．c++から使えます．今回の実験に用いたHOMOOはODEをjava用に拡張したode4j @<bib>{ode4j} を使ってモデリングされています．


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
1つめの最適化手法は 自然進化戦略(NES)@<bib>{nes}を高速化した手法です．
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
2つめの最適化手法は Adam@<bib>{adam} と呼ばれる手法です．
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

== HOMOO の歩行学習
本章ではHOMOOの歩行学習の手順と，学習結果について説明します．

=== 学習手順
最適化対象とされる@<m>{x}は「HOMOO の歩行学習」の章で説明したように，80パラメータで表される目標角度の集合とします．評価関数の値，すなわち評価値に関しては与えられた目標角度の集合@<m>{x}に対して@<img>{simu}ようにシミュレーションを行うことで計算します．

//image[simu][HOMOO の歩行シミュレーションの流れ][scale=0.95]

=== 学習結果
NES を使って学習した結果と，Adam を使って学習した結果を示します．
NES については，学習に失敗することがなかったのでうまく学習したときのものを，Adam については学習がうまくいっていないものについても載せています．

学習過程も含めて Youtube にアップロードしてあるので見てみてください．

@<b>{NES の学習過程}

　@<href>{https://youtu.be/Pt-8xC4oUx0}

@<b>{Adam の学習過程}

うまくいったもの：@<href>{https://youtu.be/Yckuwz-kipI}

うまくいっていないもの：@<href>{https://youtu.be/c1Acvd98mWA}

NESの学習結果ではHOMOO が歩行を素早く学習していることが分かります．対してAdamの学習結果ではうまくいったほうでは，少しづつ前に進むようになっていることが分かります．うまくいっていないものでは，ほんの少し学習が進むものの，ある程度のところでHOMOO がひっくり返るようなパラメータとなってしまい，そのようなパラメータから抜け出せずに学習が止まってしまうことが分かります．

=== 結果についての考察
今回の学習結果を見ると，NES のほうがよく学習していることが分かります．
NESについては，様々なハイパーパラメータを用いても学習がうまくいかないことが発見できなかったため，安定した手法であるといえます．

そもそも，Adam は1次勾配が解析的に得られることが前提であることから，性能に差が出ることは予想できました．また，HOMOO が └(└＾o＾)┘ になってしまわないように各関節の可動域を@<m>{[ -\frac{\pi\}{6\}, \frac{\pi\}{6\} ]}[rad]と制限していることで，Adam などの基本的に制約なし関数に適用される手法には不利な状況になっていたと考えられます．

今回の学習では，進化計算が持つ最適化対象関数の微分不可能性と，実行可能性に関する制約，ハイパーパラメータの設定の3つに対するロバスト性が大きく反映されることとなり，NESによる最適化のほうが優れる結果となりました．
また，Adam のパラメータチューニングの難しさもこのような結果となる原因であると考えられます．

== おわりに
本章では，ODE を紹介し，HOMOOについて説明した後，2つの最適化手法NES と Adam について説明しました．
そして，四足歩行ロボット HOMOO の歩行を 2つの最適化手法 NES と Adamを用いて学習させてみました．
結果としてNES のほうがよく学習することとなりました．

今回のように，微分情報が得られず，実行可能性に制約がある問題では，勾配法が性能が落ち，進化計算のほうが優れているということが分かりました．
裏を返せば，問題設定を工夫することで問題そのものを微分可能にすることや，制約をなるべく作らないようにすることにも大きく意味があると考えられます．

まあ，NESっていうなかなか便利な最適化手法があるんだよっていうことを知ってもらえると嬉しいです．

=== 今後の課題とか
つーか今回のような実験って，他の人がよくやってね？？？ってところなんですよねー．実際のところ，初めにやろうとしていたことは別で，四足歩行ロボットの行動をニューラルネットワークで決定させたら面白いのでは？？？というものでした@<fn>{fn:n}．
//footnote[fn:n][そもそも，問題そのものを微分可能にするっていうものがニューラルネットワーク使うことですしね．]
まあ，なんでそれやらなかったのかというと，なんかうまくいかなかったという話なんですけどね．これについては現状以下の2つの問題が分かっています．

１．@<b>{ニューラルネットワークの最適化対象変数が非常に多い}

最適化対象変数が多いと，探索空間が広すぎるため学習がほとんど進まなくなってしまう．そのため，学習そのものに膨大な時間を使ってしまい，結果を全然得られなくなってしまう．

２．@<b>{ニューラルネットワークによる行動決定手順が難しい}

行動をニューラルネットワークによって決定させる手順をテキトーにしてしまうと，実行可能性に関する制約を避けづらくなってしまう．そのため，いい感じの行動決定手段を考えなくてはいけない．また，行動決定手段を工夫することで，最適化問題を微分可能な形にすることができるらしい．しかし，知識不足のため現状では扱い方が分からない．

これらの問題について，そのうち解決できそうなものが思いついたら，またチャレンジしてみようかなと思ってます．

ここまで読んでいただきありがとうございました m(_ _)m

