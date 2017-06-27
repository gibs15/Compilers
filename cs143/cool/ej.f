%{
/*Comentarios*/
int nums = 0;
int strs = 0;
%}
%option noyywrap

%%
"fin" {
printf ("NUMS: %d\nSTRS: %d", nums, strs);
return 0;
}
{NUM}+ {
printf ("%s es NUM\n", yytext);
nums++;
}
X{0,3}V?I{0,3} {
printf ("%s es Numero Romano\n", yytext);
strs++;
}
.|\n ECHO;
%%
int main (void) {
yylex();
}

