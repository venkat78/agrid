#ifndef DEFS_HPP_
#define DEFS_HPP_

namespace utils
{
	typedef int INT;
	typedef unsigned int UINT;
	typedef double REAL;
	typedef bool BOOL;
	typedef void VOID;
	typedef unsigned long long UINT64;

	struct sTAG
	{
	};
	struct sTRUE
	{
	};
	struct sFALSE
	{
	};

	struct sCOLOR
	{
		REAL r, g, b, alpha;
		sCOLOR(REAL _r = 0.5, REAL _g = 0.5, REAL _b = 0.5, REAL _alpha = 0.5) :
				r(_r), g(_g), b(_b), alpha(_alpha)
		{
		}
	};

	inline VOID Pause() {
	}

	/*
	 *  Returns 50 different colors.
	 *  Aqua* 	0 	255 	255 	#00ffff
	 Aquamarine 	127 	255 	212 	#7fffd4
	 Blue* 	0 	0 	255 	#0000ff
	 Blue-Violet 	138 	43 	226 	#8a2be2
	 Brown 	165 	42 	42 	#a52a2a
	 Cadet Blue 	95 	158 	160 	#5f9ea0
	 Chartreuse 	127 	255 	0 	#7fff00
	 Chocolate 	210 	105 	30 	#d2691e
	 Coral 	255 	127 	80 	#ff7f50
	 Cyan 	0 	255 	255 	#00ffff
	 Dark Blue 	0 	0 	139 	#00008b
	 Dark Cyan 	0 	139 	139 	#008b8b
	 Dark Goldenrod 	184 	134 	11 	#b8860b
	 Dark Green 	0 	100 	0 	#006400
	 Dark Magenta 	139 	0 	139 	#8b008b
	 Dark Orange 	255 	140 	0 	#ff8c00
	 Dark Orchid 	153 	50 	204 	#9932cc
	 Dark Red 	139 	0 	0 	#8b0000
	 Dark Violet 	148 	0 	211 	#9400d3
	 Deep Pink 	255 	20 	147 	#ff1493
	 Firebrick 	178 	34 	34 	#b22222
	 Fuschia* 	255 	0 	255 	#ff00ff
	 Gold 	255 	215 	0 	#ffd700
	 Goldenrod 	218 	165 	32 	#daa520
	 Gray* 	128 	128 	128 	#808080
	 Green* 	0 	128 	0 	#008000
	 Green-Yellow 	173 	255 	47 	#adff2f
	 Lime* 	0 	255 	0 	#00ff00
	 Lime Green 	50 	205 	50 	#32cd32
	 Linen 	250 	240 	230 	#faf0e6
	 Magenta 	255 	0 	255 	#ff00ff
	 Maroon* 	128 	0 	0 	#800000
	 Navy* 	0 	0 	128 	#000080
	 Olive* 	128 	128 	0 	#808000
	 Orange 	255 	165 	0 	#ffa500
	 Orange-Red 	255 	69 	0 	#ff4500
	 Orchid 	218 	112 	214 	#da70d6
	 Purple* 	128 	0 	128 	#800080
	 Red* 	255 	0 	0 	#ff0000
	 Rosy Brown 	188 	143 	143 	#bc8f8f
	 Royal Blue 	65 	105 	225 	#4169e1
	 Saddle Brown 	139 	69 	19 	#8b4513
	 Spring Green 	0 	255 	127 	#00ff7f
	 Steel Blue 	70 	130 	180 	#4682b4
	 Teal* 	0 	128 	128 	#008080
	 Tomato 	255 	99 	71 	#ff6347
	 Turquoise 	64 	224 	208 	#40e0d0
	 Violet-Red 	208 	32 	144 	#d02090
	 Yellow* 	255 	255 	0 	#ffff00
	 */
	inline sCOLOR GetColor(INT i) {
		if (i == 0)
			return sCOLOR(0, 255, 255);
		else if (i == 1)
			return sCOLOR(127, 255, 212);
		else if (i == 2)
			return sCOLOR(0, 0, 255);
		else if (i == 3)
			return sCOLOR(138, 43, 226);
		else if (i == 4)
			return sCOLOR(165, 42, 42);
		else if (i == 5)
			return sCOLOR(95, 158, 160);
		else if (i == 6)
			return sCOLOR(127, 255, 0);
		else if (i == 7)
			return sCOLOR(210, 105, 30);
		else if (i == 8)
			return sCOLOR(255, 127, 80);
		else if (i == 9)
			return sCOLOR(0, 255, 255);
		else if (i == 10)
			return sCOLOR(0, 0, 139);
		else if (i == 11)
			return sCOLOR(0, 139, 139);
		else if (i == 12)
			return sCOLOR(184, 134, 11);
		else if (i == 13)
			return sCOLOR(0, 100, 0);
		else if (i == 14)
			return sCOLOR(139, 0, 139);
		else if (i == 15)
			return sCOLOR(255, 140, 0);
		else if (i == 16)
			return sCOLOR(153, 50, 204);
		else if (i == 17)
			return sCOLOR(139, 0, 0);
		else if (i == 18)
			return sCOLOR(148, 0, 211);
		else if (i == 19)
			return sCOLOR(255, 20, 147);
		else if (i == 20)
			return sCOLOR(178, 34, 34);
		else if (i == 21)
			return sCOLOR(255, 0, 255);
		else if (i == 22)
			return sCOLOR(255, 215, 0);
		else if (i == 23)
			return sCOLOR(218, 165, 32);
		else if (i == 24)
			return sCOLOR(128, 128, 128);
		else if (i == 25)
			return sCOLOR(0, 128, 0);
		else if (i == 26)
			return sCOLOR(173, 255, 47);
		else if (i == 27)
			return sCOLOR(0, 255, 0);
		else if (i == 28)
			return sCOLOR(50, 205, 50);
		else if (i == 29)
			return sCOLOR(250, 240, 230);
		else if (i == 30)
			return sCOLOR(255, 0, 255);
		else if (i == 31)
			return sCOLOR(128, 0, 0);
		else if (i == 32)
			return sCOLOR(0, 0, 128);
		else if (i == 33)
			return sCOLOR(128, 128, 0);
		else if (i == 34)
			return sCOLOR(255, 165, 0);
		else if (i == 35)
			return sCOLOR(255, 69, 0);
		else if (i == 36)
			return sCOLOR(218, 112, 214);
		else if (i == 37)
			return sCOLOR(128, 0, 128);
		else if (i == 38)
			return sCOLOR(255, 0, 0);
		else if (i == 39)
			return sCOLOR(188, 143, 143);
		else if (i == 40)
			return sCOLOR(65, 105, 225);
		else if (i == 41)
			return sCOLOR(139, 69, 19);
		else if (i == 42)
			return sCOLOR(0, 255, 127);
		else if (i == 43)
			return sCOLOR(70, 130, 180);
		else if (i == 44)
			return sCOLOR(0, 128, 128);
		else if (i == 45)
			return sCOLOR(255, 99, 71);
		else if (i == 46)
			return sCOLOR(64, 224, 208);
		else if (i == 47)
			return sCOLOR(208, 32, 144);
		else if (i == 48)
			return sCOLOR(255, 255, 0);
		return sCOLOR(1.0, 1.0, 1.0, 1.0);
	}

	BOOL IsDebugOn();

	VOID SetDebugOn();

	VOID SetDebugOff();

//#ifdef DEBUG
#define ASSERT(cond) assert(cond)
// #else
// #define ASSERT(cond)
// #endif //DEBUG.

}

//#define DEBUG 1

#endif /*DEFS_HPP_*/
