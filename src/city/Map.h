#pragma once

#include <string>
#include <map>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Tile.h"

class TextureManager;

class Map
{
public:
    Map();
    Map(const std::string& filename, unsigned int width, unsigned int height);

    static void loadTiles(const TextureManager& textureManager);
    static TileAtlas& getTileAtlas();

    void load(const std::string& filename, unsigned int width, unsigned int height);
    void save(const std::string& filename);

    void draw(sf::RenderWindow& window, float dt);

    void findConnectedRegions(std::vector<TileType> whitelist, int regionType = 0);
    void updateDirection(TileType type);

        // Select the tiles within the bounds
    void select(sf::Vector2i start, sf::Vector2i end, std::vector<TileType> blacklist);
    // Deselect all tiles
    void clearSelected();

    unsigned int getWidth() const;
    unsigned int getHeight() const;
    unsigned int getNbTiles() const;
    Tile& getTile(std::size_t position);
    const Tile& getTile(std::size_t position) const;
    void setTile(std::size_t position, Tile tile);
    int getResource(std::size_t position) const;
    void setResource(std::size_t position, int resource);
    TileState getTileState(std::size_t position) const;
    unsigned int getNumSelected() const;

private:
    static TileAtlas sTileAtlas;
    unsigned int mWidth;
    unsigned int mHeight;
    std::vector<Tile> mTiles;
    std::vector<int> mResources;
    unsigned int mSumSelected;
    unsigned int mNumRegions[1];
    std::vector<TileState> mTileStates;
    unsigned int mNumSelected;

    void depthFirstSearch(std::vector<TileType>& whitelist, int x, int y, int label, int regionType);
};