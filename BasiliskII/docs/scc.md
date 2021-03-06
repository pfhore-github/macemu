<link href="style.css" rel="stylesheet"></link>
<title> M68K macintosh hardware reference SCC</title>

## SCC

[シリアルポート](rs422.html)も見よ

## ピン一覧

![SCC pin](scc_pin.png)

## 外部レジスタ

|アドレス|機能|
|---|---|
|#00| プリンタポートコマンド|
|#02| モデムポートコマンド|
|#04| プリンタポートデータ(WR8/RR8と同じ)|
|#06|モデムポートデータ(WR8/RR8と同じ)|

## 割り込み

割り込みではすべてのポートA（モデムポート）割り込みがポートB（プリンタポート）に優先される。

同じポートでは、以下の順で優先される

### 1. 受信割り込み

すべて、割り込みの起こったデータを読み出すと、割り込みフラグは解除される

|フラグ|割り込み条件|補足|
|---|---|---|
|受信完了|下記参照|他の受信割り込みとはベクタの値が違う|
|オーバーラン|受信FIFOが溢れた||
|フレーミングエラー|非同期モードで正しくデータが終端していない||
|フレーム終了|SDLCのフレームが終端した||
|パリティエラー|パリティビットが一致しない|W2.2が有効の場合のみ|
	
### <a id="trans_int">2. 送信割り込み</a>

送信が完了してバッファが空になるたびに割り込みが発生する。CRC計算完了後にも割り込みが発生する。バッファに値を書き込むか、[Reset Tx Int](#reset_tx_int)を発行すると、割り込みフラグが解除される

### <a id="special_int">3.特殊割り込み</a>

WR15の各ビットが0の場合は、対応する割り込みは発生しない。割り込みが発生すると、RR0のうち、WR15が立っているビットはラッチされ、リセットされるまで、割り込み時の値を保持し続ける。

| 割り込み | 発生     | 解除     | 
|---------------------------|-------------------------|----------------------|
| ブレーク(非同期モードのみ)[^forcelatch1]      | ブレーク(NULL+framing error)を受信   | ビット1を受信         | 
| アボート(SDLCモードのみ)[^forcelatch2]        | アボート(7つ以上のビット1)を受信     | ビット0を受信     |
| 転送アンダーラン・転送終了| 送信バッファが空になった時、送信中止コマンドが送信された時 | リセットコマンド送信時|
| CTS| HSKiA信号がHIGHになった時                | HSKiA信号がLOWになった時|
| Sync(非同期・外部同期モード)[^notimplmented] | SYNC信号がHIGHになった時|SYNC信号がLOWになった時   | 
| Hunt(同期モード)[^hunt]|データ検索開始時（コマンド実行・アボート時）|データ検索完了時|
|DTD|GPi信号がHIGHになった時|GPi信号がLOWになった時|
|ゼロカウント|ボー・ジェネレータカウンタが0になったとき|カウンタが初期化された時|


[^hunt]:フラグ変更時に自動でラッチされる。（解除時も）
[^forcelatch1]:他の割込中でも強制的にラッチされる。
[^forcelatch2]:他の割込中でも強制的にラッチされる。
[^notimplmented]:*Macintoshでは未実装*

## 通信プロトコル

### データ形式

送信時は、ビット数が6以上の場合は、WR8レジスタの下位Nビットが送信される。ビット数が5の場合には、実際のビット数は以下の表で決定される

|データ形式|ビット数|
|--------|--|
|1111000X|1|
|111000XX|2|
|11000XXX|3|
|1000XXXX|4|
|000XXXXX|5|

受信時は、非同期モードの場合、非更新ビットは1で埋められ、同期モードの場合は、未定義である。

以下は各モードで送信されるビット列である。ここで、各データは１〜８ビットのLSBファーストのデータ＋オプションのパリティビットの最大9ビットである

- 非同期モード : [start bit(0)] [data] [ stop bit(1;1/1.5/2 clock) ]
- 同期モード : [sync char(6/8/12/16 bit) ] [data]* [ CRC ] 
- SDLCモード : [ begin flag($7E) ] [ address (8bit) ] [ ctl (8bit) ] [data]* [ CRC ] [ end flag( $7E ) ]

## 内部レジスタ

### <a id="wr0">WR0 コマンドレジスタ</a>

#### BIT 7,6 CRC初期化 (\#A0)

|値|機能|
|---:|:---:| 
| `00`| 何もしない|
| `01`| 受信CRCリセット|
| `10`| 送信CRCリセット|
|`11`| メッセージ終了ラッチリセット|

#### BIT 5,4,3 汎用コマンド (\#38)

|値|機能|
|---:|:---:| 
|`000`|レジスタ番号として0-7を選択する|
|`001`|レジスタ番号として8-15を選択する|
|`010`|外部割り込みのラッチを解放する|
|`011`|SDLCアボートを送信して、メッセージを終了する|
|`100`|<a id="reset_next_char">先頭文字状態初期化</a>|
|`101`|<a id="reset_tx_int">送信終了</a>。これ以降、送信バッファが空になっても割り込みを発生させない|
|`110`|エラーのラッチを解放する</td></tr>|
|`111`|処理中の割り込み状態を初期化して低位の割り込みを検知できるようにする|

#### BIT 2,1,0 レジスタ番号 (\#7)

これを指定する場合には、通常コマンドを`0`か`1`にする

### <a id="wr1">WR1 割り込み有効フラグ</a>

#### BIT 7 W/REQ有効化 (\#80) 

W/REQピンの有効・無効化

#### BIT 6 W/REQ機能 (\#40)

W/REQがLowになるタイミング

|値|タイミング|
|---:|:---:| 
|`0`|準備完了前にデータを送受信しようとするとき|
|`1`|準備完了時|

#### BIT 5 W/REQピン送受信フラグ　(\#20)

|値|送/受信|
|---:|:---:| 
|`0`|送信|
|`1`|受信|

#### BIT 4,3 受信割り込みモード (\#18)

|値|割り込みタイミング|
|---:|:---:| 
|`00`|無効</td></tr>|
|`01`|受信完了以外の特殊状態と[先頭文字](#reset_next_char)受信|
|`10`|受信完了以外の特殊状態と文字受信のたびに|
|`11`|受信完了以外の特殊状態|

#### BIT 2 パリティ異常フラグ (\#4) 

パリティ異常時にデータエラーにするかどうか

#### BIT 1 送信割り込み有効 (\#2) 

[送信割り込み](#trans_int)の有効・無効フラグ

#### BIT 0 外部割り込み有効 (\#1) 

[外部割り込み](#special_int)の有効・無効フラグ

### <a id="wr2">WR2 割り込みベクタ(2ポート共通)</a>

初期割り込みベクタ。両方のポートで共用されている。

### <a id="wr3">WR3 受信制御パラメータ</a>

#### BIT 7,6 キャラクタあたりの受信ビット数 (\#C0)

|値|ビット数|
|---:|---| 
|`00`|5|
|`01`|7|
|`10`|6|
|`11`|8|

#### BIT 5 自動有効化 (\#20)

CTS（送信）/DCD（受信）を受信すると、自動で送受信を開始する。

#### BIT 4 Huntモード起動 (\#10)

値1を書き込むと同期モードの探索が開始する

#### BIT 3 受信CRC有効化 (\#8)

同期モードで受信された文字をCRCチェッカに通す

#### BIT 2 SDLC アドレス検索有効化 (\#4)

SDLCモードでアドレスが不一致のデータを読み飛ばす

#### BIT 1 SYNC文字ロード抑止 (\#2) 

受信された文字がWR6と一致すると、データがスキップされる

#### BIT 0 受信有効化 (\#1)

受信処理を有効化する

### <a id="wr4">WR4 送受信制御パラメータ2</a>

#### BIT 7,6 クロック倍率 (\#C0)

|値|倍率|
|---:|---| 
|`00`|1倍|
|`01`|16倍|
|`10`|32倍|
|`11`|64倍|

#### BIT 5,4 SYNCモード (\#30)

|値|モード|
|---:|---| 
|`00`|8bitSYNC|
|`01`|16bitSYNC|
|`10`|SDLCモード|
|`11`|外部SYNC|

#### BIT 3,2 ASYNCモード (\#C)

|値|モード|
|---:|---| 
|`00`|ASYNC無効(Syncモード)|
|`01`|1クロック停止ビット|
|`10`|1.5クロック停止ビット|
|`11`|2クロック停止ビット|

#### BIT 1 偶数パリティモード (\#2)

|値|パリティモード|
|---:|---| 
|`0`|奇数パリティ|
|`1`|偶数パリティ|

#### BIT 0 パリティチェック有効 (\#1)

パリティの送受信をするかを決定する

### <a id="wr5">WR5 送信制御パラメータ</a>

#### BIT 7 DTRピン送信 (\#80)

#### BIT 6, 5 キャラクタあたりの送信ビット数 (\#60)

|値|ビット数|
|---:|---| 
|`00`|5以下|
|`01`|7|
|`10`|6|
|`11`|8|

#### BIT 4 Break送信 (\#10)

1にするとブレークが送信される

#### BIT 3 送信有効化 (\#8)

0の間は送信が保留される

#### BIT 2 CRC-16使用フラグ (\#4)

0ならばCRC-CTTITが使われる。1ならばCRC-IBMが使われる

#### BIT 1 RTSピン (\#2)

RTSピンにこの値が送られる

#### BIT 0 CRC有効化　(\#1)

1ならば送信の最後にCRCチェックサムが送信される

### <a id="wr6">WR6 Sync文字1 / SDLCアドレス</a>

|Syncモード|値|
|----|:----:|
|6bit Sync|送信用8bit Sync（下位6bitが使われる。上位2bitは下位2bitのコピー|
|8bit Sync|送信用8bit Sync|
|12bit Sync|共用16bit Sync下位(上位4bitのみ、下位4bitは1固定)|
|16bit Sync|共用16bit Sync下位|
|SDLC|アドレスフィールド|

### <a id="wr7">WR7 Sync文字2 / SDLCフラグ</a>

|Syncモード|値|
|----|:----:|
|6bit Sync|受信用8bit Sync（**上位**6bitが使われる。)|
|8bit Sync|受信用8bit Sync|
|12bit/16biyt Sync|共用16bit Sync上位|
|SDLC|`01111110`固定|

### <a id="wr8">WR8 送信バッファ</a>

送信データ

### <a id="wr9">WR9 マスター割り込み制御（2ポート共通)

#### BIT 7,6 ポートリセット (\#C0) 

|値|コマンド|
|----|:----:|
|`00`|何もしない|
|`01`|プリンタポートリセット|
|`10`|モデムポートリセット|
|`11`|SCC全リセット|

#### BIT 5 未使用 (\#20)

#### BIT 4 割り込みベクタ変更位置 (\#10)

[割り込みベクタ](#rr2)の反映位置を決定する

#### BIT 3 割り込みマスター有効化 (\#8)

0の場合、全割り込みが無効になる

#### BIT 2 下位割り込み停止 (\#4) 

1の場合、モデムポート受信以外の割り込みが無効になる

#### BIT 2 割り込み応答なし (\#2)

1の場合、割り込みに応答しない

#### BIT 1 応答結果付与 (\#1)

1の場合、割り込み応答に結果コードを付与する

### <a id="wr10">WR9 雑多制御

#### BIT 7 CRC初期ビット(\#80)

1の場合、CRCの初期ベクタが#FFFFになる

#### BIT 6,5 信号符号化方法 (\#30)

|値|符号化方式|
|----|:----:|
|`00`|NRZ(HIGH=1,LOW=0)|
|`01`|NRZI(stay=1,flip=0)|
|`10`|FM1(edge=1,no-edge=0)|
|`11`|FM0(no-edge=1, edge=0)|

#### BIT 4 SDLCループ、Go-Active-On-Poll制御 (\#10)

TBD

SDLCループモードで$7E送信時にもう1ビット送信する

#### BIT 3 SDLCアイドルビット制御 (\#8)

SDLCがアイドル時に1を流す

TBD

#### BIT 2 SDLCアボート制御 (\#4)

SDLCがバッファアンダーランの時にアボート信号を流す

#### BIT 1 SDLCループモード (\#2)
		
#### BIT 0 SYNC 6bit (\#1)

Syncを6/12bitにする



