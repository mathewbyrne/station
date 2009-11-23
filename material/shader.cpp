//
// shader.cpp
//
// Shader abstraction implementation. Contains function definitions for
// VertexShader, FragmentShader and ShaderProgram classes.
//


#include "shader.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;


// ----------------------------------------------------------------------------
// Shader class implementation
// ----------------------------------------------------------------------------


//
// Constructor.
//
Shader::Shader( const string &filename )
  : name( filename ), id( 0 ), compiled( 0 )
{ }


//
// Destroys the shader object.
//
Shader::~Shader( void )
{
  if( glIsShader( id ) )
    glDeleteShader( id );
}


//
// Compile the current shader object.
//
void Shader::compile( void )
{
  const char *sourceChar = source.c_str();

  id = glCreateShader( getShaderType() );
  glShaderSource( id, 1, &sourceChar, NULL );
  glCompileShader( id );
  glGetShaderiv( id, GL_COMPILE_STATUS, &compiled );

  if( compiled == GL_FALSE )
  {
    int length = 0;
    GLchar *log;

    glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length );

    if( length > 0 )
    {
      log = new GLchar[ length ];
      glGetShaderInfoLog( id, length, &length, log );
      std::cerr << log << endl;
      delete [] log;
    }
    
    throw std::runtime_error( "Error compiling shader." );
  }
}


//
// Loads the source code for a shader.
//
void Shader::loadCode( void )
{
  ifstream infile( name.c_str() );

  if( infile.fail() )
    throw std::runtime_error( "Couldn't open shader file" );

  source.assign( std::istreambuf_iterator<char>( infile ),
		std::istreambuf_iterator<char>() );

  infile.close();
}


// ----------------------------------------------------------------------------
// Vertex Shader class implementation
// ----------------------------------------------------------------------------


//
// Constructor
//
VertexShader::VertexShader( const string &filename )
  : Shader( filename )
{
  try
  {
    loadCode();
    compile();
  }
  catch( std::runtime_error &err )
  {
    cerr << "Error: " << err.what () << endl;
  }
}


// ----------------------------------------------------------------------------
// Fragment Shader class implementation
// ----------------------------------------------------------------------------

//
// Constructor.  Read fragment shader code from file and compile it.
//

FragmentShader::FragmentShader( const string &filename )
  : Shader( filename )
{
  try
  {
    loadCode();
    compile();
  }
  catch( std::runtime_error &err )
  {
    cerr << "Error: " << err.what () << endl;
  }
}


// ----------------------------------------------------------------------------
// Shader Program implementation
// ----------------------------------------------------------------------------

//
// Shader program constructor. Attempts to link the programs.
//
ShaderProgram::ShaderProgram( const string &name,
  const VertexShader &vShader, const FragmentShader &fShader )
  : name( name ), linked( false ), id( 0 )
{
  try
  {
    if( !vShader.isCompiled() )
      throw std::runtime_error( "Vertex Shader not compiled." );
    if( !fShader.isCompiled() )
      throw std::runtime_error( "Fragment Shader not compiled." );

    id = glCreateProgram ();
    glAttachShader( id, vShader.getId() );
    glAttachShader( id, fShader.getId() );

    glLinkProgram( id );
    glGetProgramiv( id, GL_LINK_STATUS, &linked );

    glValidateProgram( id );

    if( !linked )
      throw std::runtime_error( "Link stage failed" );
  }
  catch( std::runtime_error &err )
  {
    cout << " failed!" << endl;
    cerr << "Error: " << err.what () << endl;
  }
}


//
// This version of the constructor actully takes the path of the 2 shader files rather than
// Shader objects themselves.
//
ShaderProgram::ShaderProgram( const string& name, const string &vShaderPath,
  const string &fShaderPath )
  : name( name ), linked( false ), id( 0 )
{
  VertexShader   *vShader = NULL;
  FragmentShader *fShader = NULL;

  try
  {
    if( vShaderPath != "" )
    {
      vShader = new VertexShader( vShaderPath );
      if( !vShader->isCompiled() )
        throw std::runtime_error( "Vertex Shader not compiled." );
    }
    if( fShaderPath != "" )
    {
      fShader = new FragmentShader( fShaderPath );
      if( !fShader->isCompiled() )
        throw std::runtime_error( "Fragment Shader not compiled." );
    }


    id = glCreateProgram ();
    if( vShader ) glAttachShader( id, vShader->getId() );
    if( fShader ) glAttachShader( id, fShader->getId() );

    glLinkProgram( id );
    glGetProgramiv( id, GL_LINK_STATUS, &linked );

    glValidateProgram( id );

    if( !linked )
      throw std::runtime_error( "Link stage failed" );

    delete vShader;
    delete fShader;
  }
  catch( std::runtime_error &err )
  {
    if( vShader ) delete vShader;
    if( fShader ) delete fShader;
  
    cout << " failed!" << endl;
    cerr << "Error: " << err.what () << endl;
  }
}


//
// Correctly destroys the shader program depending on the OpenGL version.
//
ShaderProgram::~ShaderProgram ( void )
{
  if( glIsProgram( id ) )
    glDeleteProgram( id );
}


//
// Sets the current ShaderProgram object as the in use shader.
//
void ShaderProgram::useProgram( void ) const
{
  glUseProgram( id );
}


//
// Disables the current shader program. Will clear any program running, not
// just this one.
//
void ShaderProgram::disableProgram( void ) const
{
    glUseProgram( 0 );
}


//
// Returns the location of a shader uniform variable.
//
const GLuint ShaderProgram::getUniformLocation( const string& name ) const
{
  GLuint loc = glGetUniformLocation( id, name.c_str() );

  if( loc == 0 )
    throw std::runtime_error( "Invalid location" );

  return loc;
}

