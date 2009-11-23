//
// texture.h
//
// A simple OpenGL Texture encapsulation.
//

#ifndef _TEXTURE_H_
#define _TEXTURE_H_


#include "../ltypes.h"


class Texture
{

public:

  uint texId;
  uint width;
  uint height;

	Texture( void ) : texId( 0 ), width( 0 ), height( 0 ) { };
	virtual ~Texture( void );

  void loadTexture( const char *filename );
  void bindTexture( void ) const;

  const uint& getId( void ) { return texId; }
};


#endif // _TEXTURE_H_

