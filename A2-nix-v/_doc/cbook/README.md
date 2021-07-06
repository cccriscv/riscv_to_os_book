# 在 RISCV 上從頭設計 C+UNIX 之路

> 真正的 C 程序員，應該親身體驗當年 K&D 創建 C+UNIX 的過程

xv6 作業系統是美國馬省理工學院 (MIT) 的一個教學用作業系統，該系統被用做下列課程的主要教材。

* [6.S081: Operating System Engineering](https://pdos.csail.mit.edu/6.828/2020/xv6.html)

xv6 修改自 UNIX 第六版 (UNIXv6)，UNIXv6 在作業系統的發展史上具有重要的地位，因為當初 Ken Thompson 與 Dennis Richie 兩人，合力創造出 UNIX+C 這兩個重要的技術之後，一路將 UNIX 從 v1 改到 v6，這時候的 UNIXv6 核心程式碼也才總共八千多行，非常短小精簡。

問題是，當時 UNIX 的執行環境是 PDP-11 電腦，現在已經沒人用了！

後來，各大廠商 (包含 AT&T/昇陽/....) 都把 UNIXv6 拿去修改，於是 UNIX 核心程式碼就一路從一萬，十萬，百萬行直線上升，成為了後來的中大型主機市場的主力作業系統。

1990年，Linus Torvalds 為了在 IBM PC 上使用 UNIX，於是乾脆從 MINIX 這個仿照 UNIX 理念的微核心系統修改過來，最後創造出了 Linux 作業系統。

Linux 基本上承襲了 UNIX ，成為現代最重要的作業系統，但是程式碼已超過千萬行，實在不適合在《作業系統設計課程》上要求同學重新設計一次。

MIT 原本很多年沒開作業系統課程 (我想是因為不知該怎麼教)，2002 年再度開設作業系統課程時，決定從 John Lions 對 UNIXv6 進行大量註解的原始碼開始研讀，然後在 x86 處理器的 IBM PC 上創造了一個稱為 JOS 的小型系統，讓學生透過作業的方式擴充，逐步開始學習作業系統設計。

於是學生們一邊要學 PDP11 ，另一邊又要學 x86 PC，非常的痛苦，所以後來在 2006 年將 UNIXv6 改寫為 x86 處理器的版本，稱為 xv6，用來做為課程教材，這樣就不用學 PDP11 了。2012 年開始這些課程資源放上網路，其課程網址與 github 專案如下：

* 課程 -- https://pdos.csail.mit.edu/6.828/2012/xv6.html
* 程式 -- https://github.com/mit-pdos/xv6-public

雖然 xv6 的出現讓 MIT 的作業系統課程變得更好了，但是老師和學生們能然必須和相對複雜的 x86 處理器奮戰，到了 2018 年，由於開源處理器 RISC-V 逐漸成熟，所以他們將程式移植改寫為 RISC-V 版本，並從 2019 年開始改用 RISC-V 版本的 xv6 教課，其課程網址與 github 專案如下：

* 課程 -- https://pdos.csail.mit.edu/6.828/2020/xv6.html
* 程式 -- https://github.com/mit-pdos/xv6-riscv

我在金門大學教授《計算機結構+系統程式》課程時，總覺得只教理論不太好。於是試圖自己創造《處理器+作業系統》，但難度實在很高，我沒辦法做得很好。

後來我找到了 nand2tetris 這門課程，解決了《教學生自行設計處理器》的問題，但是我仍然沒辦法設計出一個好的《教學型作業系統》。

看到 xv6-rsicv 的出現，讓我認為透過 RISC-V + xv6 來教《計算機結構+系統程式》會是一個很好的方式，所以我開始研讀 RISC-V 的書籍與 xv6 的教材，並且創造了 mini-riscv-os 這個《嵌入式 RISC-V 作業系統》，以便作為 xv6 的前導教材，讓同學們能夠透過 mini-riscv-os 的程式，一步一步地學會《作業系統設計》。

現在，RISC-V 與 xv6 的資源已經愈來愈完備了，我建議大家可以從下列資源開始，一步一步的學習《處理器設計/計算機結構/系統程式/嵌入式系統/作業系統》。

1. https://www.nand2tetris.org/
    * 習題參考 -- https://gitlab.com/ccc109/co
2. https://github.com/cliffordwolf/picorv32
3. https://github.com/cccriscv/mini-riscv-os
4. https://github.com/mit-pdos/xv6-riscv
    * 習題參考 -- https://blog.mky.moe/mit6828/menu/
    * https://github.com/monkey2000/xv6-riscv-fall19


希望透過這些資源，讓學生們學習《計算機結構+作業系統》等課程時，不再只是紙上談兵，而是可以真正動手體驗 DIY 的樂趣！

