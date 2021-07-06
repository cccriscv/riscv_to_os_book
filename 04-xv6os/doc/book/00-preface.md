# xv6：一個簡單的、類似Unix的教學操作系統。

Russ Cox Frans Kaashoek Robert Morris

2020年8月31日

## 前言和致謝

這是一篇為操作系統課準備的草稿，它通過研究一個名為xv6的內核例子來解釋操作系統的主要概念。xv6以Dennis Ritchie和Ken Thompson的Unix Version 6 (v6)[14]為藍本。xv6大致沿用了v6的結構和風格，但在多核RISC-V[12]中用ANSI C[6]實現。

這篇文章應該和xv6的源代碼一起閱讀，這種方法受到John Lions的UNIX第六版注釋[9]的啟发。參見https://pdos.csail.mit.edu/6.S081，以獲得 v6 和 xv6 的在線資源指針，包括一些使用 xv6 的實驗作業。

我們在麻省理工學院的操作系統課6.828和6.S081中使用過這篇課文。我們感謝這些課程的教師、助教和學生，他們都對xv6做出了直接或間接的貢獻。我們尤其要感謝Adam Belay、Austin Clements和Nickolai Zeldovich。最後，我們要感謝那些通過電子郵件向我們发送文本中的錯誤或改進建議的人。Abutalib Aghayev、Sebastian Boehm、Anton Burtsev、Raphael Carvalho、Tej Chajed、Rasit Eskicioglu、Color Fuzzy、Giuseppe、Tao Guo、Naoki Hayama、Robert Hilderman、Wolfgang Keller、Austin Liew、Pavan Maddamsetti。Jacek Masiulaniec、Michael cConville、m3hm00d、miguelgvieira、Mark Morrissey、Harry Pan、Askar Safin、Salman Shah、Adeodato Simó、Ruslan Savchenko、Pawel Szczurko、Warren Toomey、tyfkda、tzerbib、Xi Wang、鄒昌偉。

如果您发現錯誤或有改進建議，請发郵件給Frans Kaashoek和Robert Morris（kaashoek,rtm@csail.mit.edu）。
