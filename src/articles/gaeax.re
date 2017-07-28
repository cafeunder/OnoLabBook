@<author>{かふぇした}

= GA-EAXで解くみくにゃんTSP
//lead{
みんな巡れー！
//}

== はじめに
=== この記事のやりたいこと
 * @<b>{遺伝的アルゴリズム (GA) で}
 * @<b>{4万都市みくにゃん巡回セールスマン問題 (TSP) を}
 * @<b>{高精度に解く！}

@<b>{組合せ最適化問題}とは，ある条件の下で最も良い組合せを見つける問題のことを言います．
組合せ最適化問題は，都市計画や業務効率化，製品設計など様々な場面で直面する重要な問題であるため，
その解法も理論的なものから実用的なものまで広く研究されています．
そして，人間の脳が論理的思考に甚だ向いていないことを考えると，
解法（＝アルゴリズム）をプログラムで記述し，コンピュータに解かせる．というのが賢い選択というものです．

このとき，ある問題における解のなかで最も良い解を@<b>{最適解}と言います．
また，得られる解が必ず最適解であるようなアルゴリズムを@<b>{厳密解法}と言い，
厳密解法によって得られた解を@<b>{厳密解}と言います．
できれば，厳密解法を使って組合せ最適化問題を解きたい！...というのが当然の感覚です．
しかし，組合せ最適化問題の中には，コンピュータですら厳密解を得るのが困難なものがあります．
ここでいう困難であるとは，厳密解を得るために膨大な時間がかかったり，膨大な記憶領域が必要となることを言います．

そこで@<b>{近似解法}の登場です．近似解法とは，現実的な実行コストでそこそこ良い解を得る手法のことです．
そもそも，膨大な実行コストをかけてまで最適解を知る必要があるかと聞かれれば，ほとんどの場合はNoでしょう@<fn>{fn_no}．
ということで，「そこそこ速く，そこそこ良い解が得られる近似解法」が現実社会では重要になります．
//footnote[fn_no][実行コストがどこまで許容できるか，というのは応用する場面によります．業務効率化や製品設計などはある程度時間をかけても良いでしょうが，組合せ最適化でいう膨大な時間とは数万年や数億年はザラなので，その場合はやはり近似解法に頼ることになります．]

こういった近似解法や厳密解法は人間が考案するものですが，
組合せ最適化問題に汎用的な解き方というのは存在せず，
実用の場面では人間の勘やノウハウの応用に頼っているのが現実です．
そのため，組合せ最適化問題の汎用的なアルゴリズムを開発することは，
関連する研究者たちの大きな目標でもあります．
@<b>{進化計算}は，そんな汎用的手法のひとつです．

本記事では，組合せ最適化問題を代表する問題である巡回セールスマン問題を，
進化計算の一形態である遺伝的アルゴリズムを使って解く手法について解説します．

=== 巡回セールスマン問題
@<b>{巡回セールスマン問題(Traveling Salesman Problem: TSP)}は困難な組合せ最適化問題のひとつです．
TSPは，@<m>{N}個の都市と各都市間の距離が与えられたとき，
全ての都市を訪問し最初の都市に戻ってくるような経路のうち，
最も移動距離の合計（@<b>{経路長}）が短くなるような経路を求める問題です．
参考として，@<img>{tours}に10都市TSPにおける経路の例を示します．
//image[tours][10都市TSPの例．解の候補として3つの経路を示している．]

TSPは配送問題や基盤穿孔，遺伝子構造解析とか，色々なことに使えるという触れ込みですが@<fn>{fn_truth}，
それよりも@<u>{定義が単純で，得られる解が視覚的に理解できる}という性質から，近似解法のベンチマーク問題としてよく利用されます．
組合せ最適化問題の近似解法は，異なる問題でも考え方や工夫を流用できることが多いため，
定義が単純でノウハウを流用しやすく，現象が理解しやすいTSPが好まれるわけです．
//footnote[fn_truth][実際には，TSPとして捉えることが困難なことも多くあります．]

== 遺伝的アルゴリズム
@<b>{遺伝的アルゴリズム (Genetic Algorithm: GA) }は，生物の進化過程を模倣した，@<b>{汎用的近似解法（メタ戦略）}@<fn>{fn_meta}のひとつです．
この章では，最もナイーブな遺伝的アルゴリズムについて簡単に説明していきます．
//footnote[fn_meta][メタ戦略（メタヒューリスティクスとも）の定義はわりと曖昧で，このような説明は不適切な場合もあるのですが，ここでは気にしないことにします．]

=== 基本的な考え方
GAでは，解候補を自然環境に生息する個体としてみなし，複数の解候補を個体の集団として捉えます．
それぞれの個体には，それがどれほど良い解であるかを表す評価値@<fn>{fn_eval}が与えられ，これを自然環境への適合度として考えます．
そして，環境に生息する個体が世代交代を繰り返すことで，より良い個体（＝近似解）を発見することを期待します．
//footnote[fn_eval][例えば巡回セールスマン問題であれば，経路長が短いほど良い個体＝評価値の高い個体となります．]

=== 遺伝的オペレータ
世代交代を行うために各個体に対して行う操作のことを遺伝的オペレータと呼びます．
GAの主な遺伝的オペレータとして，交叉と選択があげられます．

==== 交叉
交叉は，複数の個体の特徴@<fn>{fn_type}を受け継いだ新たな個体を生成する手順です．
これは自然界における有性生殖に相当し，この操作によって集団に新しい解を追加します．
交叉の際，新たに生成された個体を@<b>{子個体}とよび，子個体の生成に使用した個体を@<b>{親個体}とよびます．
//footnote[fn_type][例えばTSPでは，巡回する都市の順番や，2都市間を結ぶエッジなどが特徴として考えられます．]

より良い個体を発見するためには，それまでに評価をしていない新しい個体を生成する必要があります．
しかし，てんでバラバラに個体を生成したのでは，良い個体が得られるのは完全に運任せになってしまいます．
そこで，集団に存在する個体の特徴を受け継いだ，新たな個体を生成することを考えます．
これにより，それまでの探索経過を活用した新しい個体を生成することができます．

このとき，親個体の特徴を受け継いだ個体をどう生成するかは，手法の設計次第です．
特に交叉はGAの根幹を成すオペレータであるため，交叉をどう設計するかが手法の性能を決めるといっても過言ではありません．

なお，交叉によって集団に属する個体の数（@<b>{集団サイズ}）が増加することが考えられますが，
ほとんどの手法が，次に示す選択の操作によっていくつかの個体を削除し，集団サイズを一定に保ちます．

==== 選択
選択では，評価値の観点で悪い個体を集団から削除し，良い個体が集団に残るようにします．
自然界における自然淘汰@<fn>{fn_select}に相当し，この操作によって集団が良い解へ収束することが保証されます．
//footnote[fn_select][もともとは自然選択と呼ばれていて，「選択」というワードはここから取ったようです．]

このとき，個体の評価値をどのように計算するかは手法ごとに異なります．
例えば，TSPでは経路長が短い個体ほど良い個体なのですが，評価値の計算を経路長だけで行うと，
後述する多様性が失われてしまうことが考えられます．

また，個体の選択方法も，親個体と子個体を合わせて下位を切り捨てる方法や，良いものを確率的に選ぶ（ときどき悪いのも選ぶ）方法，
親個体とそこから生成された子個体間のみで入れ替えを行う方法など，様々あります．

=== 集団の多様性
前述した通り，GAは交叉による新たな個体の生成と，選択による悪い個体の削除によって集団を改善していきます．
これにより，前の世代に存在した悪い特徴は削除され，良い特徴だけが後の世代に引き継がれていくため，
必然的に集団の個体は似通っていきます．
このとき，集団に同じような個体ばかり存在していると，交叉によって生成される子個体も同じような個体ばかりになり，集団の変化が停滞してしまいます．
むろん，その似通った個体が最適解もしくはそれ付近であれば良いのですが，そうでない場合は，良い近似解を得ることは絶望的になります．
例えばTSPをGAで解く際に，経路長だけで個体を評価した場合は，
最適解から遠いのにも関わらず，その集団だけで見ると良い個体ばかりが良い評価値を獲得し，
結果として最適解の発見に失敗することが考えられます．
よって，集団に存在する特徴の多様性を維持することは，GAの性能を高める上で非常に重要となります．

=== GAの良し悪し
GAは，遺伝的オペレータの設計さえ適切に行えれば，いかなる問題にも適用可能であるという，メタ戦略としての利点を持ちます．
また，GAは複数の解候補の相互作用で近似解を導出する@<b>{多点探索}の１手法であり，
高度にチューニングされた実装では，優れた性能を発揮することが知られています．

一方で，GAの性能は遺伝的オペレータの設計によって大きく変化するため，
粗い実装では他の手法と比較してあまり良い性能が出せません．
また，他の手法よりも良い性能を出そうとすると，問題ごとに適切な遺伝的オペレータを設計する必要があるため，
メタ戦略としての利点は薄れていきます．

実際の所，メタ戦略として知られるアルゴリズムのほとんどは枠組みにとどまっており，具体的な実装は設計者に委ねられています．
今回説明するGA-EAXも同様で，あくまでもTSPのためにチューニングされている手法です．

@<b>{だったら汎用手法でもなんでもないじゃないか}という感じなのですが，
先述したとおり，組合せ最適化はアルゴリズムの工夫やノウハウを流用できることがあるため，
GAを用いたソルバを開発することは，他の問題にGAを適用する上でも有用であると考えられます．

== GA-EAX
永田らによって提案されたGA-EAXは，交叉方法をはじめとして，評価関数や選択方法などをTSPのために高度にチューニングしたGAです．
GA-EAXは，特に数万都市以上の大規模インスタンスに対する，@<b>{最も強力なTSPソルバのひとつ}です．
例えば，TSPソルバのベンチマークとして良く利用されるNational TSP（数十都市～数万都市）のほとんどの最適解@<fn>{fn_opt}（または既知最良解）の導出に成功しているほか，
10万都市以上のインスタンスとして知られるTSP Art Instances（10万都市～20万都市）の既知最良解はすべてGA-EAXが発見したものです．
//footnote[fn_opt][通常，TSPの大規模インスタンスに厳密解法は適用不可能なため，人類が見つけた最も良い解であっても，最適解であるとは限りません．ただし，何らかの手法によって最適解が判明しているインスタンスも存在します．]

以下よりGA-EAXの簡単な説明を行いますが，到底書ききれる量ではないため，詳細は論文[Nagata 12]@<fn>{fn_nagata}を参照してください．
//footnote[fn_nagata][Nagata, Y. and Kobayashi, S.: A powerful genetic algorithm using edge assembly crossover for the traveling salesman problem, INFORMS Journal on Computing, Vol.25, No.2, pp.346-363 (2013)]

=== 交叉EAX
交叉EAXは，2つの経路から特徴を受け継いだ子個体を得る交叉手法で，GA-EAXの核となるアルゴリズムです．
交叉EAXでは，1つの交叉ペアから複数の子個体を生成します．このとき，子個体生成数を@<m>{n_{\rm ch\}}とします．

//imagewide[eax][交叉EAXの手順．][scale=0.9]

交叉EAXの手順を図にしたものを@<img>{eax}に示します．
@<img>{eax}は，親個体@<m>{p_{\rm A\},p_{\rm B\}}を受け取り，@<m>{n_{\rm ch\}=2}の子個体を生成している様子です．
以下に，各手順の説明を示します．

==== AB-Cycle構成
まず初めに，@<b>{AB-Cycle}を構成します．AB-Cycleとは，両親のエッジを交互に辿る閉路のことです．
AB-Cycle構成の際，以下に示す2つの特徴に留意する必要があります．

 * 任意の両親に対し，AB-Cycleの構成パターンは複数存在する．
 * どのような構成パターンでも，すべてのエッジをAB-Cycleに分割できる．

@<img>{eax}では，合計5つのAB-Cycleを構成しています（一番下に3つ存在することに注意してください）．

==== E-Set構成
次に，E-Setを@<m>{n_{\rm ch\}}個構成します．E-Setとは，1つ以上の任意のAB-Cycleを重ね合わせたグラフのことを指します．
ただし，2点間を行き来するようなAB-Cycleは選びません．

==== 中間個体生成
次に，構成したE-Setを用いて中間個体を生成します．
この工程では，@<m>{p_{\rm A\}}からE-Setに含まれる@<m>{p_{\rm A\}}のエッジを削除し，
代わりに@<m>{p_{\rm B\}}に含まれるエッジを追加することで，新たな経路を生成します．
この操作によって生成された個体を中間個体と呼びます．

==== 中間個体の修正
E-Setと@<m>{p_{\rm A\}}によっては，生成された中間個体が複数の部分経路に分割されることがあります．
この場合は，修正操作によって1つの経路へ連結されます．修正操作は，修正後の経路がなるべく短くなるように連結を行います．

以上の手順で得られた@<m>{n_{\rm ch\}}個の個体を，子個体として返します．

=== E-Set構成の戦略
E-Set構成の際，AB-Cycleを選択する手順のことを@<b>{戦略}と呼びます．
GA-EAXでは，交叉EAXが取った戦略によって探索の性質が変化することが知られています．
以下で，GA-EAXで用いられている戦略を説明します．

==== Random戦略
Random戦略は，構成されたAB-Cycleをそれぞれ1/2の確率で選択する戦略です．
ただし，少なくとも1つはAB-Cycleが選択されるようにします．
Random戦略は最も単純な戦略ですが，生成される子個体が部分経路に分割されやすく，探索性能が劣化することが知られています．
また，確率的に@<m>{p_{\rm A\}}と@<m>{p_{\rm B\}}を平均した個体が生まれやすく，多様性が低下することも考えられます．
よって，通常は後述するSingle戦略とBlock2戦略が用いられ，Random戦略が用いられることはあまりありません．

==== Single戦略
Single戦略は，AB-Cycleをひとつだけ選択する戦略です．
Single戦略では，必要なAB-Cycleの数＝E-Setの数になるため，
AB-Cycleの構成を@<m>{n_{\rm ch\}}回だけ行えばよいことになり，計算量を削減できます．
また，必然的に入れ替えるエッジの本数が少なくなるため，親@<m>{p_{\rm A\}}の形質が維持されやすく，多様性の観点でも優れていると言えます．

一方で，入れ替えるエッジの本数が少ないため，探索が停滞しやすい欠点があります．
そのため，最初はSingle戦略で探索を行い，
探索が停滞した時点で，入れ替えるエッジの本数が多いRandom戦略やBlock2戦略に切り替える手法が提案されています．

==== Block2戦略
Block2戦略は，中間個体がなるべく部分経路に分割されないように，複数のAB-Cyleを選択する戦略です．
複数のAB-Cycleを選ぶことで，一度に多くのエッジを入れ替え，Single戦略などで停滞した集団を改善することを狙います．
ただし，一度に多くのエッジを入れ替えると，生成された中間個体が部分経路に分割されやすいことが知られています．
部分経路に分断された中間個体の修正は，親が持っていないエッジ，すなわち探索の過程で残らなかったエッジを追加することになり，
特に探索終盤は経路長の改善につながる確率が低くなることが知られています．
そこで，Block2戦略には部分経路の個数が少なくなるような工夫が導入されています@<fn>{fn_block2}．
//footnote[fn_block2][詳しくは論文を参照してください．]

=== 世代交代
==== 交叉ペアの選択
まず，集団をシャッフルし，順にラベリングした@<m>{x_i \ (i=1,2,\dots,n_{\rm pop\})}とします．
このとき，@<m>{n_{\rm pop\}}は集団サイズを表します．
その後，交叉ペア@<m>{(p_{\rm A\}, p_{\rm B\})}を@<m>{(x_i,x_{i+1\}) \ (i=1,2,\dots,n_{\rm pop\}}ただし，@<m>{x_{n_{\rm pop\}+1\}=x_1)}として選択します．

==== 親個体@<m>{p_{\rm A\}}との入れ替え
両親@<m>{p_{\rm A\}, p_{\rm B\}}から@<m>{n_{\rm ch\}}個の子個体を生成し，
その中で最も評価値の良い個体と，親@<m>{p_{\rm A\}}を入れ替えます．
ただし，経路長が親@<m>{p_{\rm A\}}よりも長い子個体は置き換えの対象に含めません．

以上の手順を図で表したものが，@<img>{gg}です．

//image[gg][GA-EAXにおける世代交代の例．]

==== 子個体の評価方法
GA-EAXでは，個体を経路長と多様性指標の観点から評価します．
GA-EAXにおける多様性指標はいくつか提案されていますが，論文[Nagata 12]では
集団に含まれるエッジがどれほど多様性を保っているかを表す@<b>{枝エントロピー (edge entropy)}という指標を用いています．
この指標により，多様性が改善されると判定されれば評価値を大きくし，
逆に多様性が失われていると判定されれば評価値を小さくします．

=== 全体の流れ
全体の流れを以下にまとめます．

//nest{
1. 初期集団を生成する@<fn>{fn_2opt}．
2. 戦略@<m>{strategy}をSingle戦略とする．
3. 集団をシャッフルし，順にラベリングする．
4. 交叉ペア@<m>{(p_{\rm A\}, p_{\rm B\})}を@<m>{(x_i,x_{i+1\})}として，以下の操作を行う．
 * 戦略@<m>{strategy}に従い，交叉EAXにより子個体を@<m>{n_{\rm ch\}}個生成する．
 * 子個体の中で最も評価値が高い個体を@<m>{c_{\rm best\}}とし，@<m>{c_{\rm best\}}の経路長が@<m>{p_{\rm A\}}より短いなら置き換える．
5. 終了条件を満たした場合，以下のようにする．満たさなかった場合はステップ3へ戻る．
 * @<m>{strategy}がSingle戦略なら，@<m>{strategy}をBlock2戦略としてステップ3へ戻る．
 * @<m>{strategy}がBlock2戦略なら，アルゴリズムを終了する．
6. アルゴリズム終了時における最短経路長の個体を解として出力する．
//}
//footnote[fn_2opt][初期集団の生成には，ローコストな近似解法である2-Optを用いるのが一般的です．]

== みくにゃんTSP (mk39393)
本題に入ります．
GA-EAXの性能は論文[Nagata 12]や数々のベンチマークで証明されているため，この記事であえて同じことはしません．
その代わり，@<b>{独自にインスタンスを生成しGA-EAXで解いてみることにします}．
解くインタンスは，とあるバーチャルシンガーをモデルとした39,393万都市TSP，@<b>{mk39393}とします．

=== インスタンスの生成方法
TSPにおける解は，各点を一筆書きで結んだ形になるため，
何かしらの画像を点描に変換し，各点を都市に見立てれば，TSPのインスタンスとして扱うことができます．
例えば，10万都市インスタンスとして良く用いられるmona-lisa100Kは，モナ・リザの画像を10万点の点描に変換したものです．
@<img>{mona}にmona-lisa100Kの経路の例を示します．

//image[mona][TSPの10万都市インスタンスmona-lisa100Kの経路の例．][scale=0.5]

このようなインスタンスの特徴として，ランダムに生成された経路では何を表しているのか全く分からないのに対し，経路長が短くなるにつれて理解できる絵になることがあげられます．

今回は，とあるバーチャルシンガーのイラストとロゴを点描に変換してインスタンスを生成しています．
本記事では，権利的に画像を載せることができませんが，後述するGitHubにて経路の画像を公開しています．

== 実験
=== 目的
mk39393にGA-EAXを適用し，mk39393の近似解を得ることを目的とします．

=== 設定
GA-EAXの設定として，集団サイズ@<m>{n_{\rm pop\}=500}，子個体生成数@<m>{n_{\rm ch\}=30}とします．
また，同様の設定で10試行行うものとします．

=== 結果
@<table>{tab_result}に結果を示します．
参考に，2-Optによる初期集団生成時点の最良経路長も示します．

//tabletabooularraw[tab_result][10試行の実行結果．bestは10試行の最良経路長を，num.はbestが得られた試行数を表す.]{
     |  best & num.
=====|============>&=====
2-Opt | 6,213,697  & 1
GA-EAX | 5,853,137 & 3
//}

@<table>{tab_result}より，現時点@<fn>{fn_best}におけるmk39393の最良解は5,853,137であることが判明しました．
もちろんこれは，今回の実験設定によるもので，インスタンスに合わせてパラメータをチューニングすることで，
更なる既知最良解を発見できる可能性があります．
//footnote[fn_best][2017年7月28日現在．]

== おわりに
本記事で行ったことは以下の通りです．

 * 組合せ最適化問題，特にTSPについて解説した．
 * 組合せ最適化問題の汎用的手法であるGAについて解説した．
 * TSPの強力なソルバであるGA-EAXについて解説した．
 * 独自の39,393都市TSP，mk39393を生成し，GA-EAXによって既知最良解を得た．

本記事でやり残したことは以下の通りです．

 * mk39393に対するGA-EAXのチューニング．それによって得られる既知最良解の発見．
 * 点描によるインスタンス生成方法の説明．
 * GA-EAXの詳細な説明．特に，修正操作，Block2戦略，評価関数，終了条件について．
 * 10万都市以上の大規模インスタンスに対する手法の紹介．

またの機会があれば，上記のテーマで筆を取らせて頂きたいと思います．
ここまで読んでくださってありがとうございました．

== mk39393配布先
インスタンスファイル，最良経路ファイル，最良経路の画像をGitHubで公開しています@<fn>{fn_mk}．
ご興味のある方はどうぞ．
//footnote[fn_mk][@<href>{https://github.com/cafeunder/TSPToolKit/tree/master/mk39393}]

既知最良解は随時更新していきます．
もし，公開中のものより経路長が短くなる解が得られた場合は，
GitHubアカウント，もしくはtwitterアカウント@<fn>{fn_tw}でご連絡ください．
//footnote[fn_tw][@cafeunder]
