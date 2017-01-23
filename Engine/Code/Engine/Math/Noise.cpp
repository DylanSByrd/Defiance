#include "Engine/Math/Noise.hpp"
#include "Engine/Math/Vector2f.hpp"
#include "Engine/Math/Vector2i.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>

// Based on code provided by Squirrel Eiserloh

//-----------------------------------------------------------------------------------------------
// Computes a random Perlin noise value in the range [-1,1] based on a 2D input <position> and
//	various Perlin noise parameters.
//
//	<perlinNoiseGridCellSize>: Noise density.  Larger values produce longer wavelength noise
//		(e.g. gentle, sweeping hills).
//	<numOctaves>: 0 is flat, 1 is simple smoothed noise. Values of 2+ add one or more additional
//		"octave" harmonics.  Each additional octave has double the frequency/density but only a
//		fraction of the amplitude of the base noise.
//	<persistence>: The fraction of amplitude of each subsequent octave, based on the amplitude of
//		the previous octave.  For example, with a persistence of 0.3, each octave is only 30% as
//		strong as the previous octave.
//
float ComputePerlinNoise2D( const Vector2f& position, float perlinNoiseGridCellSize, float baseAmplitude, int numOctaves, float persistence )
{
   float totalPerlinNoise = 0.f;
   float currentOctaveAmplitude = baseAmplitude;
   float totalMaxAmplitude = 0.f;
   float perlinGridFrequency = (1.f / perlinNoiseGridCellSize);
   for( int octaveNumber = 0; octaveNumber < numOctaves; ++octaveNumber )
   {
      Vector2f perlinPosition( position.x * perlinGridFrequency, position.y * perlinGridFrequency );
      Vector2f perlinPositionFloor( (float) floor( perlinPosition.x ), (float) floor( perlinPosition.y ) );
      Vector2i perlinCell( (int) perlinPositionFloor.x, (int) perlinPositionFloor.y );
      Vector2f perlinPositionUV = perlinPosition - perlinPositionFloor;
      Vector2f perlinPositionAntiUV( perlinPositionUV.x - 1.f, perlinPositionUV.y - 1.f );
      float eastWeight = SmoothStep( perlinPositionUV.x );
      float northWeight = SmoothStep( perlinPositionUV.y );
      float westWeight = 1.f - eastWeight;
      float southWeight = 1.f - northWeight;

      Vector2f southwestNoiseGradient = GetPseudoRandomNoiseDirection2D( perlinCell.x, perlinCell.y );
      Vector2f southeastNoiseGradient = GetPseudoRandomNoiseDirection2D( perlinCell.x + 1, perlinCell.y );
      Vector2f northeastNoiseGradient = GetPseudoRandomNoiseDirection2D( perlinCell.x + 1, perlinCell.y + 1 );
      Vector2f northwestNoiseGradient = GetPseudoRandomNoiseDirection2D( perlinCell.x, perlinCell.y + 1 );

      float southwestDot = Vector2f::DotProduct( southwestNoiseGradient, perlinPositionUV );
      float southeastDot = Vector2f::DotProduct( southeastNoiseGradient, Vector2f( perlinPositionAntiUV.x, perlinPositionUV.y ) );
      float northeastDot = Vector2f::DotProduct( northeastNoiseGradient, perlinPositionAntiUV );
      float northwestDot = Vector2f::DotProduct( northwestNoiseGradient, Vector2f( perlinPositionUV.x, perlinPositionAntiUV.y ) );

      float southBlend = (eastWeight * southeastDot) + (westWeight * southwestDot);
      float northBlend = (eastWeight * northeastDot) + (westWeight * northwestDot);
      float fourWayBlend = (southWeight * southBlend) + (northWeight * northBlend);
      float perlinNoiseForThisOctave = currentOctaveAmplitude * fourWayBlend;

      totalPerlinNoise += perlinNoiseForThisOctave;
      perlinGridFrequency *= 2.f;
      totalMaxAmplitude += currentOctaveAmplitude;
      currentOctaveAmplitude *= persistence;
   }

   if( totalMaxAmplitude != 0.f )
      totalPerlinNoise /= totalMaxAmplitude;

   return totalPerlinNoise;
}


