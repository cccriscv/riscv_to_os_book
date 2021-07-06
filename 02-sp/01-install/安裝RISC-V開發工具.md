## 安裝 RISC-V 開發工具

RISC-V 組織釋出了很多開發工具與範例，我們可以從下列網址中看到這些工具。

* https://riscv.org/exchange/software/

RISC-V 的設計者也知道這樣的開放原始碼處理器，可能有很大的商業潛力，因此他們成立了一家稱為 SiFive 的公司，撰寫 RISC-V 的工具軟體，並且設計生產 RISC-V 晶片與開發板。

SiFive 所釋出的 FreedomStudio 是一整套 RISC-V 開發工具，包含 gcc, qemu 與整合開發環境等等。

在本書中，我們將使用 FreedomStudio 的 Windows 版做為主要示範，使用其中的 gcc 工具鏈與 qemu 虛擬機，但是不使用 FreedomStudio 的整合開發工具，而是改用 Visual Studio Code + git bash 作為開發環境。

所以要執行本書的範例前，請大家先從下列網址註冊下載 FreedomStudio for Windows (很大、1.7G)，然後安裝好 Visual Studio Code 與 git for windows，記得安裝 git 時要勾選安裝 bash。

* https://www.sifive.com/software

安裝好後請將系統路徑 PATH 設到其中的 gcc 與 qemu 所在資料夾中，在我電腦中的安裝路徑如下:

> D:\install\FreedomStudio-2020-06-3-win64\SiFive\riscv64-unknown-elf-gcc-8.3.0-2020.04.1\bin;
> 
> D:\install\FreedomStudio-2020-06-3-win64\SiFive\riscv-qemu-4.2.0-2020.04.0\bin

想在 Visual Studio Code 中使用 git bash ，請先開一個 Terminal 之後，於 Terminal 右上角選 Select Default Shell 後，選擇 GitBash ，之後開的 Terminal 就都會是 git bash 了！

另外、我們會使用到 make 這個專案建置工具，您可以安裝 CodeBlocks ，然後將其中的 make 所在路徑加入 PATH 中。(我電腦中的 make 位於下列位置)

```
$ where make
C:\Program Files (x86)\CodeBlocks\MinGW\bin\make.exe
```

對於 Linux/Mac 的讀者，通常是很熟練的程式人員，請您自行安裝好 FreedomStudio for Linux/Mac，然後設定好路徑，至於使用何種編輯器與開發環境，那就任君選擇了！
