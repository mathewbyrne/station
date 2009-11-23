%{
#include "obj.h"
#include <cstdio>
#include <vector>

int yylex();
void yyerror(ObjModel *objModel, ObjLoadData *ld, const char *s);

%}

%parse-param {ObjModel *objModel}
%parse-param {ObjLoadData *ld}

%token V VT VN F USEMTL SLASH STRING REAL INTEGER G

%union {
    char *str;
    float real;
    int integer;
}

%type <str> STRING
%type <real> REAL
%type <integer> INTEGER

%%

objfile             : statements;

statements          : statements statement
                    |
                    ;

statement           : vertex
                    | vertex_texture
                    | vertex_normal
                    | face
                    | use_material
                    ;

vertex              : V REAL REAL REAL 
                      { ld->vertices.push_back(Vec3($2, $3, $4)); }
                    | V REAL REAL REAL REAL
                    ;

vertex_texture      : VT REAL 
                      { ld->texCoords.push_back(Vec3($2, 0.0f, 0.0f)); }
                    | VT REAL REAL
                      { ld->texCoords.push_back(Vec3($2, 1.0f - $3, 0.0f)); }
                    | VT REAL REAL REAL
                      { ld->texCoords.push_back(Vec3($2, 1.0f - $3, $4)); }
                    ;

vertex_normal       : VN REAL REAL REAL
                      { ld->normals.push_back(Vec3( $2, $3, $4)); }
                    ;

face                : F 
                      { objModel->beginFace(); }
                      vertex_triplet vertex_triplet vertex_triplet
                    ;

vertex_triplet      : INTEGER
                        { objModel->addFaceVertex($1, 0, 0, *ld); }
                    | INTEGER SLASH INTEGER 
                        { objModel->addFaceVertex($1, $3, 0, *ld); }
                    | INTEGER SLASH SLASH INTEGER
                        { objModel->addFaceVertex($1, 0, $4, *ld); }
                    | INTEGER SLASH INTEGER SLASH INTEGER
                        { objModel->addFaceVertex($1, $3, $5, *ld); }
                    ;

use_material        : USEMTL STRING
                        { objModel->useTexture($2); }
                    ;
%%


