#include "MemeField.h"
#include "SpriteCodex.h"
#include <assert.h>
#include <random>


void MemeField::Tile::SpawnMeme()
{
	assert( !HasMeme );
	HasMeme = true;
}

bool MemeField::Tile::GetHasMeme() const
{
	return HasMeme;
}

void MemeField::Tile::Reveal()
{
	assert(state == States::Hidden);
	state = States::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
	return state == States::Revealed;
}

void MemeField::Tile::Draw(const Vei2& screenpos, Graphics& gfx) const
{
	switch (state)
	{
	case States::Hidden:
		SpriteCodex::DrawTileButton(screenpos, gfx);
		break;
	case States::Flagged:
		SpriteCodex::DrawTileButton(screenpos, gfx);
		SpriteCodex::DrawTileFlag(screenpos, gfx);
		break;
	case States::Revealed:
		if (!HasMeme)
		{
			SpriteCodex::DrawTile0(screenpos, gfx);
		}
		else
		{
			SpriteCodex::DrawTileBomb(screenpos, gfx);
		}
		break;
	}
}

MemeField::Tile& MemeField::TileAt(const Vei2& gridpos)
{
	return field[gridpos.y * width + gridpos.x];
}

Vei2 MemeField::ScreenToGrid(const Vei2& gridpos) const
{
	return gridpos / SpriteCodex::tileSize;
}

const MemeField::Tile& MemeField::TileAt(const Vei2& gridpos) const
{
	return field[gridpos.y * width + gridpos.x];
}

MemeField::MemeField(int nMemes)
{
	assert(nMemes > 0 && nMemes < width * height);

	std::random_device rd;
	std::mt19937 rng( rd() );
	std::uniform_int_distribution<int> xDist(0,width - 1);
	std::uniform_int_distribution<int> yDist(0,height - 1);

	for (int MemesSpawned = 0; MemesSpawned < nMemes; MemesSpawned++)
	{
		Vei2 SpawnPoint;
		do
		{
			SpawnPoint = { xDist( rng ) ,yDist( rng ) };
		} while (TileAt(SpawnPoint).GetHasMeme());

		TileAt(SpawnPoint).SpawnMeme();
	}

	//reveal test////////////////////////////////////
	for (int x = 0; x < 120; x++)
	{
		Vei2 gridpos = { xDist(rng) ,yDist(rng) };
		if (!TileAt(gridpos).IsRevealed())
		{
			//TileAt(gridpos).Reveal();
		}
	}
}

void MemeField::Draw(Graphics& gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridpos = { 0,0 }; gridpos.y < height; gridpos.y++)
	{
		for (; gridpos.x < width; gridpos.x++)
		{
			
			TileAt(gridpos).Draw(gridpos * SpriteCodex::tileSize, gfx);
		}
		gridpos.x = 0;
	}
}

RectI MemeField::GetRect() const
{
	return RectI(0,width * SpriteCodex::tileSize,0,(height) * SpriteCodex::tileSize);
}

void MemeField::OnRevealedClick(const Vei2& screenpos)
{
	Vei2 gridpos = ScreenToGrid(screenpos);
	assert(gridpos.x >= 0 && gridpos.x < width && gridpos.y >= 0 && gridpos.y < height);
	Tile& tile = TileAt(gridpos);
	if (!tile.IsRevealed())
	{
		tile.Reveal();
	}
}
