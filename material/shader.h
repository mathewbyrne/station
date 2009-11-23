//
// shader.h
//
// A simple shader abstraction for GLSL shaders.
//

#ifndef _SHADER_H_
#define _SHADER_H_

#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl.h>
#include <string>
using std::string;


//
// Shader base class. Is extended to a Vertex or Fragment shader below.
//
class Shader
{

protected:

  string name;
  string source;
  GLuint id;
  GLint  compiled;

  void loadCode( void );
  void compile ( void );

public:

  Shader(const string &filename);
  virtual ~Shader( void );

  virtual GLenum getShaderType( void ) const = 0;

  // Accessor functions.
  const string &getName   ( void ) const { return name; }
  const string &getSource ( void ) const { return source; }
  const GLuint &getId     ( void ) const { return id; }

  const bool isCompiled   ( void ) const { return ( compiled != GL_FALSE ); }
};


//
// Vertex Shader, extends the base Shader class above.
//
class VertexShader : public Shader
{

public:

  VertexShader( const string &filename );

  virtual GLenum getShaderType( void ) const { return GL_VERTEX_SHADER; }

};


//
// Fragment Shader, extends the base Shader class above.
//
class FragmentShader : public Shader
{

public:

  FragmentShader( const string &filename );

  virtual GLenum getShaderType( void ) const { return GL_FRAGMENT_SHADER; }

};


//
// A ShaderProgram encapsulates both Fragment and Vertex shaders into a usable
// shader program.
//
class ShaderProgram
{

private:

  string name;
  GLint  linked;
  GLuint id;

public:

  ShaderProgram( const string& name, const VertexShader& vShader,
    const FragmentShader& fShader );
  ShaderProgram( const string& name, const string& vShaderPath,
    const string& fShaderPath );
  ~ShaderProgram( void );

  void useProgram( void ) const;
  void disableProgram( void ) const;

  // Accessor functions.
  const string &getName ( void ) const { return name; }
  const GLuint &getId   ( void ) const { return id; }
  bool isLinked         ( void ) const { return ( linked != GL_FALSE ); }

  const GLuint getUniformLocation( const string& name ) const;

};


#endif // _SHADER_H_

