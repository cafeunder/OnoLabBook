= 強化学習で自作ゲームを学習させる
//lead{
悪いなのび太，このゲーム学習用なんだ
//}

== はじめに
ゲームをやるのも好きで作るのも好きな私ですが，
会社でスマブラをボコボコにされてから，
「勝手にプレイがうまくなっててほしい」
と切実に思うようになりました．

「じゃあ勝手に学習してくれるAIつくろう」，
そう思い，
学生時代に読んでいた強化学習のバイブル本を手に取るのでした．．．

というわけで，
本稿ではゲームをプレイするAIを強化学習によって学習させてみた過程や結果を徒然なるままに綴っています．
冒頭にスマブラうんぬんとか書いておきながら，
今回は【適当に作った自作ゲーム】を学習させてます．
スマブラとか難しすぎて現段階では無理なので，
まずは単純なものから，
ということで学習させる専用のゲームを作りました．
こっからスタートでだんだんとクオリティをあげていきます，目標は高く．．．

== 強化学習とは
強化学習とは，エージェント@<fn>{agent}が環境@<fn>{env}とのインタラクションを通じて学習し，環境から得られる報酬@<fn>{reward}を最大化するための枠組みです．
//footnote[agent][ゲームをプレイするAIみたいなイメージです．]
//footnote[env][今回の場合はゲームそのものです．]
//footnote[reward][ゲームでいうとスコアです．]

強化学習では，エージェントと環境は以下のようなインタラクションを行うことで学習を進めます．

 1. エージェントは各時刻@<m>{t}において，環境から状態@<m>{{\bm s\}_{t\}}を観測し，それを元に行動@<m>{a_t}を出力する．
 2. エージェントの行動によって，環境の状態は@<m>{{\bm s\}_{t+1\}}へと遷移する．この時，エージェントは状態遷移に応じた報酬@<m>{r_t}を受け取る．
 3. 時刻を@<m>{t+1}に進めてstep1に戻る．
以上の流れを表したイメージが，@<img>{reinforcement}になります．
うーん，なんとも分かりにくいですね．
//image[reinforcement][エージェントと環境のインタラクションのイメージ][scale=1.0]

エージェントは，状態空間から行動空間への写像関数または確率分布によって表される「政策」によって意思決定を行います．
強化学習では，初期状態から最終状態までに得られる報酬の合計が最大となるような政策を，環境から与えられた報酬をもとにして獲得することが目的となります．
特徴的なのは，あらかじめ環境に対する情報や，どのような行動を取ればいいといった事前情報が与えられないことを前提とした場合が一般的であるということです．
人間で言うと，説明書を読まないで知らないゲームをプレイして，適当にガチャガチャとボタンを押しているうちに操作を学んでいく，といったイメージでしょうか．

ちゃんとした数式を書くとキリがないので，こんな感じのふわっとした説明にとどまります．

== 強化学習の手法の大別
強化学習で最も有名な手法と言えば，Q-Learningに尽きるでしょう．
ブロック崩しなどのおなじみのゲームをかなり高いスコアを取れるまで学習することができたDeep-Q-Network@<fn>{dqn}も，Q-Learningをベースとしている手法です．
Q-Learningが強化学習をほとんど知らない人でも知っている世の中になったのは，Deep-Q-Networkによる功績が大きいのでしょうね．
//footnote[dqn][通称DQNと呼ばれます．]

Q-Learingは，価値関数法と言う手法にジャンル分けされます．
価値関数法とは，簡単に言うと，最適な政策@<fn>{value}に従って行動を行ったときに，
将来的にどの程度報酬を得ることができるか（価値関数）を求めるような手法です．
もちろん学習のはじめは，どれだけ報酬が見込めるかなんて分からないので，エージェントは行動を繰り返して「ある状態である行動を取り，ある状態に遷移してその際にある報酬を受け取った」という情報を蓄積して，その情報をもとに価値関数を学習していきます．
//footnote[value][環境から得られる報酬の合計が最大となる政策のことです．]

ここまで価値関数法の話を長々としてきましたが，今回私が学習手法として選んだ手法は価値関数法ではありません．
というのも，私は学生時代に強化学習系の研究をしていましたが，
価値関数法ではない手法を扱っていたのです（それに，今更価値関数法をやるのも二番煎じ感がありますしね…）．

さて，今回私が選んだ手法は，直接政策探索法というジャンルに区別される手法です．
この後の章では，まず直接政策探索法について説明を行った後，今回チョイスした手法について説明を行います．

== 直接政策探索法
直接政策探索法@<fn>{dps}は，価値関数を求める代わりに，最適な政策を直接求めることを行う手法です．
//footnote[dps][英語ではDirect Policy Search（DPS）と呼ばれます．]
直接政策探索法の中にも様々な手法がありますが，共通して言える特徴は，
政策を何らかの数理モデルで表現して，そのモデルのパラメータを学習するという点です．
政策の表現モデルの種類は数多く存在し，
手法によってどのようなモデルが用いられるかは結構異なっています．
もちろん，みんなだいすきニューラルネットワークさんがモデルになることもあります．
また，どのようにしてパラメータを学習させる，つまり更新させるかも手法によって異なります．

政策勾配法という手法では，モデルのパラメータに関する報酬の合計の期待値の勾配を山登りさせるようにして，
パラメータを更新します．
ニューラルネットワークがモデルの場合なんかは微分計算が可能なので，
割とキッチリと勾配を求めることができるので，政策勾配法との相性は良い気がします．
モデルの微分計算が可能でない場合でも，モンテカルロ的に勾配を近似する方法もあるので，
なかなか色々使える手法です．
そういった理由もあってか，直接政策探索法と言えば政策勾配法というイメージが強いです．

政策勾配法のように勾配を計算したりせずに，モデルのパラメータを更新する手法もあります．
遺伝的アルゴリズムや進化戦略を始めとする進化計算手法がその代表例です．
これらの手法は強化学習以外の分野でも広く使われているため，直接政策探索法であると限定することはできませんが，
モデルのパラメータを進化計算手法で更新しているならば，直接政策探索法の一種であると言うことは充分できるでしょう．

== 学習手法
今回私が学習手法として選んだ手法も，直接政策探索法の１つです．
その名をSAP-GAと言います．
呼んで字のごとし，遺伝的アルゴリズム，つまりGAによってモデルのパラメータを更新する手法です．

本章では，SAP-GAの政策のモデル，パラメータの更新方法について，それぞれ解説をします．

=== 政策のモデル
SAP-GAは，「事例ベース政策」と呼ばれる政策をモデルとして用います．
事例ベース政策は，事例と呼ばれるデータの集合によって定義される政策のモデルです．
１つ１つの事例は，状態ベクトルと行動値の対によって表現されます．
これは，「環境の状態がこの状態のときはこの行動を取れば良い！」というような，
いわゆる定石のようなものであると考えるとわかりやすいと思います．
つまり，事例ベース政策は，定石の集まりようなものであるとイメージを持てば良いと思います．

事例ベース政策では，環境からある状態@<m>{\bm{s\}}を観測したとき，
その状態@<m>{\bm{s\}}に最も近い状態を持つ事例の行動を選択します．
これは非常に直感的な行動の決定方法です．
例えば，将棋をやるとき，「たしかこんな盤面の状況ではこの手を打つのが良いと本で読んだことがある！こんかいの盤面の同じような状況だから同じ手を打とう！」なんてことを考えて次の一手を決めることがあると思います．
事例ベース政策の行動の決定方法はまさにそれです．
いろいろな定石を持っていて，状況に最も近い定石に従って行動を決める，そのような行動決定方法を取るのが事例ベース政策です．

=== パラメータの更新方法
前章で説明したように，SAP-GAは，「この状態ならこの行動を取れば良い」という定石の集まりを政策として扱います．
あらかじめ定石が分かっていれば話は早いのですが，強化学習ではそのような事前情報は与えられないのを前提とするのが一般的です．
よって，SAP-GAでは環境とのインタラクションを通して，徐々に定石を学習させます．

具体的な学習方法ですが，前述したように，遺伝的アルゴリズムを用いて行います．
SAP-GAでは，「事例ベース政策」を遺伝的アルゴリズムで言う「個体」と捉えており，複数の個体によって交叉や突然変異を繰り返すことによって，より良い個体を探索します．
以下に，SAP-GAの学習の流れを示します．

 1. 複数個の政策を持つ集団を初期化する．この時，各政策の事例の状態ベクトルと行動値はランダムに決定する．
 2. 集団内の政策から親のペアをランダムに複数組作る．
 3. 各親ペア毎に，交叉を行い，新たな政策を複数個ずつ作り，環境に適用して評価@<fn>{evaluate}を行う．
 4. 評価値が高い政策を残すように，集団内の政策を入れ替える．この時，評価値が高い政策の一部の事例をランダムに初期化しなおすことで突然変異操作を加えた政策も，集団に残るように入れ替える．
 5. step2に戻る．
//footnote[evaluate][今回の場合，ゲームを複数回プレイさせてその平均スコアを，政策の評価値とします．]

政策の親ペアによる交叉は，２つの親の政策が持つ事例をランダムに組み合わせて，新たな子となる政策を作ることによって行います．
以下に，SAP-GAの交叉の流れを示します．

 1. 子の政策を空集合で初期化する．
 2. 片方の親から確率@<m>{r}で，もう片方の親から確率@<m>{1-r}で，親の事例を子に追加する．
 3. 子の政策の事例の数が指定した範囲内に収まっていなければ，step1に戻る．
 4. 決められた個数子を作るまでstep1に戻って子を作る．

SAP-GAが学習を通してやっていることは，「適当に作ってみた定石のうち，ほんとに良さそうな定石だけを集めた政策を作る」ということです．
アルゴリズムのもとになる考え方や全体の流れは非常に直感的でわかりやすいため，Q-Learningなどと同様で実装がしやすいリーズナブルな手法です．

今回は紙面の都合のため，SAP-GAの大枠の流れのみ記載しましたが，詳細が気になる方は元論文@<fn>{sapga}を参照してください．
//footnote[sapga][url]

== 学習させるゲーム
本章では，今回学習させる【適当に作った自作ゲーム】について記述します．

=== ゲーム概要
今回のゲームのコンセプトは，「見るからに学習がさせやすそう」です．
色々考えましたが，結局「玉跳ね返す系」か「玉避ける系」の二択のどちらかにしようということで落ち着きました．
今回は，後者の「玉避ける系」のゲームを作ることを選択しました．
前者にしなかった理由は，ブロック崩しみたいになってしまって，Deep-Q-Networkの二番煎じのように見えるからです．
ええ，それだけです．

今回のゲームのプレイ画面を@<img>{game}に示します．
//image[game][ゲームのプレイ画面（仮）][scale=1.0]
中央に水色の丸がプレイヤーです．
エージェントはこいつを操作することになります．
プレイヤーの周りにいる灰色のヤリは，プレイヤーを襲うウエポンです．
ヤリはプレイヤーめがけて飛んできます．
プレイヤーはヤリに当たってしまうと死んでしまい，ゲームオーバーとなります．
ゲームのスコアは，ゲームスタートしてからゲームオーバーするまでのフレーム数とします．
なので，エージェントはヤリを避けるようにプレイヤーを操作して，
できるかぎり長い時間生き残ることを目指すことになります．

ヤリは複数個がまとめて飛んでくることもありますが，この数の上限数はパラメータで指定できるようにしました．
これは，どの程度の難易度までならいい感じに学習してくれるかを効率的に考察することに役立つからです．
自作のゲームを用いるとこのような利点があって良いです．

=== 強化学習のための環境の定義
このゲームを学習させるためには，

 1. 環境から観測する状態ベクトル
 2. 取ることができる行動
 3. 環境から受け取る報酬

について定義する必要があります．
今回は，以下のように定義しました．

==== 状態ベクトル
プレイヤーとの距離が最も近いヤリとの相対座標，およびヤリの進行方向，さらにプレイヤー自身の絶対座標の要素を並べたベクトルを状態ベクトルとしました．
なので，エージェントには，プレイヤーから一番近くにいるヤリとプレイヤー自身しか見えていない状況でゲームをプレイしていることになります．
事例ベース政策では，状態ベクトルの次元数は学習させるモデルのパラメータ数に直結するため，なるべく状態ベクトルの次元数を低く抑えることで学習をさせやすくしています．

なお，事例ベース政策では，現在の状態と最も近い状態ベクトルを持つ事例の行動を選択するため，
状態ベクトル同士の距離を計算する必要があります．
ユークリッド距離を用いるのが自然ですが，
状態ベクトルの要素毎に取りうる範囲のスケールが異なるため，
一部の要素だけが距離に大きく影響を与える可能性があります．
そこで，状態ベクトルの各要素の値がすべて0から1の範囲に収まるように正規化を行います．
これによってスケールの違いは吸収され，特定の要素のみが距離に大きな影響を与えることを避けることが期待されます．

==== 行動
エージェントが選択できる行動は，以下の５つです．

 1. プレイヤーを上に動かす．
 2. プレイヤーを下に動かす．
 3. プレイヤーを右に動かす．
 4. プレイヤーを左に動かす．
 5. プレイヤーを静止させる（何もしない）．

==== 報酬
ゲームの各フレーム毎に，報酬が1ずつ与えられます．
また，画面外にプレイヤーが逃げていくのを防ぐために，
プレイヤーの絶対座標が画面の中央から500ピクセル以上距離が離れたところにいたら，
その時点でゲームオーバーとし，さらにペナルティとして報酬を-10000与えます．
この報酬設計は，「画面外に出ないようにしながらなるべく生き残ってね」ということを期待してのものです．

ちなみに，画面サイズは(1000, 1000)です．
なんでこんなでかいサイズにしたのかは自分でもわかりません．

== 学習

=== 学習設定
事例ベース政策が持つ事例の数は30個，各親ペアが作る子の数は10個，集団内の政策の数は20個としました．また，交叉では，各親の事例を確率@<m>{0.5}で子の政策に引き継ぐようにします．

政策の評価値は，ゲームを20回プレイさせて，そのスコアの平均とします．
学習中に計100万回プレイを行った場合，そこで学習を打ち切ります．

ヤリがまとめて存在できる最大数は，調整の結果5個となりました．

=== 学習結果

== 多少の考察

== おわりに
