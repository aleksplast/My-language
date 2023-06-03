# My-language

## Overview

The goal of this project is to create my own programming language. It will contain frontend, middleend and backend stages. 

## Main idea

This language is made in style of famous russian streamer - Silvername a.k.a Vladistas. Syntax of programm contain his most famous phrases.

<img align="center" src = "https://github.com/aleksplast/My-language/assets/111467660/78f7b884-20c4-489b-b8d0-213b97baae5b">

<details>
<summary> Programm example </summary>
<img align="center" src = "https://github.com/aleksplast/My-language/assets/111467660/321fcf6e-5d3a-43d0-b244-451fe282253d">
</details>

## Frontend

In this step programm constructs tree from given text. It is constructed according to standart, that was developed by our group. You can find it [here](https://github.com/dodokek/LanguageStandart)
It is made with the help of recursive decent and when work is done, tree looks like this

<details>
<summary> Tree example </summary>
<img align="center" src = "https://github.com/aleksplast/My-language/assets/111467660/bd5a614f-ea40-4794-9697-4466c3e2245e">
</details>

With the construction done, middleend comes to play.

## Middleend

In this step, programm is optimized. Constans arithmetic is reduced, so 2+3 becomes 5, meaningless nodes are deleted, so contructions like $variable * 0$ becomes 0.

## Backend

In this step, programm is translated into code of out assembler-emulator. We should think about variables handling, arithmetic and many other things.
When everything is set to place and wotking, translated programm is executed on out CPU-emulator. 

## Summary

In this project we created our own Programming language. Next step will be creating our own compiler. But this is the stoty for another time.



