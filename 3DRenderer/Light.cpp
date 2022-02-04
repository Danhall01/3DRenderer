#include "Light.h"

LightData Light::GetLightData()
{
    LightData l;
    l.Position = _Transform * _Position;
    l.Color = _Color;
    l.shininess = _Shininess;
    return l;
}
