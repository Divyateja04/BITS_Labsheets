%%
abcd printf("\n"); ECHO; yyless(1);
bcd printf("\n"); ECHO; yyless(4);
cd printf("\n"); ECHO; yyless(2);
. { 
    printf("\nEchoing Character: %c", *yytext);
}
%%
int main(){
    yylex();
    return 0;
    // yyless(): trim characters from yytext and puts them back on stdin.
    // yyless(n) returns all but the first n characters of the current token back to
    // the input stream, where they will be rescanned when the scanner looks for
    // the next match. yytext and yyleng are adjusted appropriately (e.g., yyleng will now be equal to n ).
}