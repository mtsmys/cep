# cep
C library for CEP(Complex Event Processing) in embedded system

## 内部構成，及び特徴
このライブラリでは外部の共有ライブラリとしてSQLite3を利用しています．
そのため，ターゲットデバイス上で動作させる際，libsqlite3が必要です．

CEPではデータ処理のオーバーヘッドを低減させるため，SQLiteを使ってメモリ空間上にデータベースを構築し，このメモリデータベースの上で
