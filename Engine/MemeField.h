#pragma once


#include "Vei2.h"
#include "Graphics.h"

class MemeField
{
private:
	class Tile
	{
	public:
		enum class States
		{
			Hidden,
			Flagged,
			Revealed
		};
	public:
		void SpawnMeme();
		bool GetHasMeme() const;
		void Draw(const Vei2& screenpos, Graphics& gfx) const;
	private:
		States state = States::Hidden;
		bool HasMeme = false;
	};
public:
	MemeField( int nMemes );
	void Draw(Graphics& gfx)const;
	RectI GetRect()const;
private:
	Tile& TileAt(const Vei2& gridpos);
	const Tile& TileAt(const Vei2& gridpos) const;
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[width * height];


};