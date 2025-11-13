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
    [*] --> Begin

    Begin --> Int : digit
    Int   --> Int : digit

    Int   --> ExpStart : e/E

    
    Int         --> DotAfterInt : '.'
    DotAfterInt --> Float1        : digit 
    Float1        --> Float1        : digit

    
    Begin    --> DotStart : '.'
    DotStart --> Float1     : digit

    
    Float1      --> ExpStart : e/E
    DotAfterInt --> ExpStart : e/E 


    ExpStart   --> ExpSign   : +/-
    ExpStart   --> ExpDigits : digit
    ExpSign    --> ExpDigits : digit
    ExpDigits  --> ExpDigits : digit

    Begin --> InString : '
    InString --> InString  : not ' and not newline
    InString --> StringEnd : '

    Begin --> InComment : '{'
    InComment --> InComment  : not '}'
    InComment --> CommentEnd : '}'


    Begin --> Begin : blank


    state Int <<final>>          
    Int --> end
    state DotAfterInt <<final>>  
    DotAfterInt --> end
    state Float1 <<final>>        
    Float1 --> end
    state ExpDigits <<final>>    
    ExpDigits --> end
    state StringEnd <<final>>
    StringEnd --> end
    state CommentEnd <<final>>
    CommentEnd --> end
```


![DFA](./imgs/image.png)