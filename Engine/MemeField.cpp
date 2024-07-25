#include "MemeField.h"
#include "SpriteCodex.h"
#include <assert.h>
#include <random>
#include <algorithm>


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
	if (state != States::Flagged)
	{
		assert(state == States::Hidden);
		state = States::Revealed;
	}
}

bool MemeField::Tile::IsRevealed() const
{
	return state == States::Revealed;
}

void MemeField::Tile::Flag()
{
	assert(!(state == States::Flagged));
	state = States::Flagged;
}

void MemeField::Tile::UnFlag()
{
	assert(state == States::Flagged);
	state = States::Hidden;
}

bool MemeField::Tile::IsFlagged() const
{
	return state == States::Flagged;
}

void MemeField::Tile::SetNeighbourMemeCount(int Memes)
{
	nNeighbourMemes = Memes;
}

void MemeField::Tile::Draw(const Vei2& screenpos,bool fucked, Graphics& gfx) const
{
	if(!fucked)
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
				SpriteCodex::SelectTile(screenpos, nNeighbourMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenpos, gfx);
			}
			break;
		}
	}
	else
	{
		switch (state)
		{
		case States::Hidden:
			if (GetHasMeme())
			{
				SpriteCodex::DrawTileBomb(screenpos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenpos, gfx);
			}
			break;
		case States::Flagged:
			if (GetHasMeme())
			{
				SpriteCodex::DrawTileBomb(screenpos, gfx);
				SpriteCodex::DrawTileFlag(screenpos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenpos, gfx);
				SpriteCodex::DrawTileCross(screenpos, gfx);
			}
			break;
		case States::Revealed:
			if (!HasMeme)
			{
				SpriteCodex::SelectTile(screenpos, nNeighbourMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBombRed(screenpos, gfx);
			}
			break;
		}
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

int MemeField::CountNeighbourMemes(const Vei2& gridpos)
{
	const int xStart = std::max(0,gridpos.x - 1);
	const int yStart = std::max(0,gridpos.y - 1);
	const int xEnd = std::min(width - 1,gridpos.x + 1);
	const int yEnd = std::min(height - 1,gridpos.y + 1);

	int Count = 0;

	for (Vei2 gridpos{xStart,yStart}; gridpos.y <= yEnd; gridpos.y++)
	{
		for (gridpos.x = xStart;gridpos.x <= xEnd;gridpos.x++)
		{
			if (TileAt(gridpos).GetHasMeme())
			{
				Count++;
			}
		}
	}
	return Count;
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


	for (Vei2 gridpos = { 0,0 }; gridpos.y < height; gridpos.y++)
	{
		for (gridpos.x = 0; gridpos.x < width; gridpos.x++)
		{
			TileAt(gridpos).SetNeighbourMemeCount(CountNeighbourMemes(gridpos));
		}
	}
}

void MemeField::Draw(Graphics& gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridpos = { 0,0 }; gridpos.y < height; gridpos.y++)
	{
		for (gridpos.x = 0; gridpos.x < width; gridpos.x++)
		{
			TileAt(gridpos).Draw(gridpos * SpriteCodex::tileSize,IsFucked, gfx);
		}
	}
}

RectI MemeField::GetRect() const
{
	return RectI(0,width * SpriteCodex::tileSize,0,(height) * SpriteCodex::tileSize);
}

void MemeField::OnRevealedClick(const Vei2& screenpos)
{
	if (GetRect().Contains(screenpos) && !IsFucked)
	{
		Vei2 gridpos = ScreenToGrid(screenpos);
		assert(gridpos.x >= 0 && gridpos.x < width && gridpos.y >= 0 && gridpos.y < height);
		Tile& tile = TileAt(gridpos);
		if (!tile.IsRevealed())
		{
			if (tile.GetHasMeme())
			{
				IsFucked = true;
			}
			tile.Reveal();
		}
	}
}

void MemeField::OnFlagClick(const Vei2& screenpos)
{
	if (GetRect().Contains(screenpos) && !IsFucked)
	{
		Vei2 gridpos = ScreenToGrid(screenpos);
		assert(gridpos.x >= 0 && gridpos.x < width && gridpos.y >= 0 && gridpos.y < height);
		Tile& tile = TileAt(gridpos);
		if (!tile.IsRevealed())
		{
			if(tile.IsFlagged())
			{
				tile.UnFlag();
			}
			else
			{
				tile.Flag();
			}
		}
	}
}
