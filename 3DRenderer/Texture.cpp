#include "Texture.h"

void Texture::SetImage(std::string filepath)
{
	if (img != nullptr)
	{
		stbi_image_free(img);
	}
}
