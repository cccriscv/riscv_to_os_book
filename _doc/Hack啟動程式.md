# Hack啟動程式 (BootLoader)

## C 語言的啟動程式

* 用 C 語言寫《嵌入式作業系統》
    * 啟動程式與連結檔 http://misavo.com/view/c0/link.md

## Nand2tetris


## 組合語言部分

* https://github.com/cccbook/sp/blob/master/code/js/07-vm-hack/js/vmh.js

```
function init() {
  comment("init");
  A('256');
  C('D=A');
  compToReg('SP', 'D');    // SP=256
  cmdCall('Sys.init', 0);  // call Sys.init()
}
```

## 接上虛擬機之後 (NextedCall 測試)

* https://github.com/cccbook/sp/blob/master/code/nand2tetris/08/FunctionCalls/NestedCall/Sys.vm

```
// Sys.vm for NestedCall test.
//
// Copyright (C) 2013 Mark A. Armbrust.
// Permission granted for educational use.

// Sys.init() calls Sys.main(), stores the return value in temp 1,
//  and enters an infinite loop.

function Sys.init 0
call Sys.main 0
pop temp 1
label LOOP
goto LOOP

// Sys.main() calls Sys.add12(123) and stores return value (135) in temp 0.
// Returns 456.

function Sys.main 0
push constant 123
call Sys.add12 1
pop temp 0
push constant 246
return

// Sys.add12(int x) returns x+12.
// It allocates 3 words of local storage to test the deallocation of local
// storage during the return.

function Sys.add12 3
push argument 0
push constant 12
add
return

```

## 接上虛擬機之後 (完整)

* 全部 -- https://github.com/havivha/Nand2Tetris/tree/master/12
    * https://github.com/havivha/Nand2Tetris/blob/master/12/Keyboard.jack
    * https://github.com/havivha/Nand2Tetris/blob/master/12/Screen.jack
    * https://github.com/havivha/Nand2Tetris/blob/master/12/Array.jack
    * https://github.com/havivha/Nand2Tetris/blob/master/12/Memory.jack
    * https://github.com/havivha/Nand2Tetris/blob/master/12/Output.jack
    * https://github.com/havivha/Nand2Tetris/blob/master/12/String.jack

* Sys.jack -- https://github.com/havivha/Nand2Tetris/blob/master/12/SysTest/Sys.jack

```
// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/12/Sys.jack

/**
 * A library of basic system services.
 */
class Sys {

    /** Performs all the initializations required by the OS. */
    function void init() {
        do Math.init();
        do Output.init();
        do Screen.init();
        do Keyboard.init();
        do Memory.init();
        do Main.main();
    }

    /** Halts execution. */
    function void halt() {
        while(true){}
    }

    /** Waits approximately duration milliseconds and then returns. */
    function void wait(int duration) {
        var int i, j;
        let i = 0;
        while( i < duration ) {
            let j = 0;
            while( j < 200 ) {
                let j = j + 1;
            }
            let i = i + 1;
        }
    }

    /** Prints the given error code in the form "ERR<errorCode>", and halts. */
    function void error(int errorCode) {
        do Output.printString("Err");
        do Output.printInt(errorCode);
        do halt();
    }
}
```