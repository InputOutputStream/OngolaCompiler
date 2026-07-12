#ifndef TOKEN_H
#define TOKEN_H


typedef struct TOKEN_STRUCT
{
    enum{
         TOKEN_ID,  // ok
         TOKEN_EQUALS, // ok
         TOKEN_STRING, // ok
         TOKEN_SEMI, // ok
         TOKEN_COMMA, // ok
         TOKEN_LPAREN, // ok
         TOKEN_RPAREN, // ok
         TOKEN_LBRACE,// ok
         TOKEN_RBRACE,// ok
         TOKEN_LSBRACE,// ok
         TOKEN_RSBRACE,// ok
         TOKEN_COLON, // ok
         TOKEN_PLUS, // ok
         TOKEN_MINUS, // ok
         TOKEN_STAR, // ok
         TOKEN_SLASH, // ok
         TOKEN_LT,// ok
         TOKEN_GT,// ok
         TOKEN_EQEQ,// ok
         TOKEN_LTEQ,// ok
         TOKEN_GTEQ,// ok
         TOKEN_NEQ,// ok
         TOKEN_NOT,// ok
         TOKEN_FUNC,

         TOKEN_NUMBER, // ok
         
         TOKEN_IF,// ok
         TOKEN_ELSE,// ok
         TOKEN_WHILE,// ok
         TOKEN_FOR,// ok
         TOKEN_RETURN,
         TOKEN_DATA_TYPE,

         TOKEN_EOF,
    }type;
    
    char *value;
}token_t;


token_t *init_token(int type, char *value);



#endif // TOKEN
