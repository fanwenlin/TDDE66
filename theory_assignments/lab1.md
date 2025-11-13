1. Regular expressions
```
DIGIT [0-9]
EXP     [eE][+-]?{DIGIT}+
DEC1    {DIGIT}+\.{DIGIT}*
DEC2    \.{DIGIT}+

comments: \{[^\}]*\} 
strings: '([^\n']*('')?)*' 
integers: {DIGIT}+ 

floating points: ({DEC1}|{DEC2})({EXP})?|{DIGIT}+{EXP} 
```

2. DFA

```mermaid
stateDiagram
    direction LR 
    [*] --> begin

    begin --> integerOrFloat: digit[0-9]
    integerOrFloat --> integerOrFloat: digit[0-9]
    integerOrFloat --> float: e / .
    integerOrFloat --> integer: nondigit, not ./e

    begin --> string : '
    begin --> comment : \{

    begin --> begin: blank
    state string <<final>>
    state comment <<final>>
    state float <<final>>

    state integer <<final>>
```


![DFA](./imgs/image.png)