/* Simulopolis
 * Copyright (C) 2018 Pierre Vigier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "Network.h"
#include <queue>
#include <stack>

const std::array<sf::Vector2i, 4> Network::sDirections = {sf::Vector2i(-1, 0), sf::Vector2i(1, 0), sf::Vector2i(0, -1), sf::Vector2i(0, 1)};

Network::Network(unsigned int width, unsigned int height) :
    mRoads(height, width, false), mComponents(height, width, 0)
{

}

void Network::reshape(unsigned int width, unsigned int height)
{
    mRoads.reshape(height, width, false);
    mComponents.reshape(height, width, 0);
}

void Network::addRoad(int i, int j)
{
    mRoads.set(i, j, true);
}

void Network::removeRoad(int i, int j)
{
    mRoads.set(i, j, false);
}

void Network::updateComponents()
{
    // Reset components
    std::fill(mComponents.getData().begin(), mComponents.getData().end(), 0);
    mEntryPoints.clear();
    // Flood fill
    int c = 1;
    for (std::size_t i = 0; i < mRoads.getHeight(); ++i)
    {
        for (std::size_t j = 0; j < mRoads.getWidth(); ++j)
        {
            if (mRoads.get(i, j) && mComponents.get(i, j) == 0)
            {
                mEntryPoints[c] = std::vector<sf::Vector2i>();
                floodFill(i, j, c);
                ++c;
            }
        }
    }
}

bool Network::getAdjacentRoad(int i, int j, sf::Vector2i& coords) const
{
    if (i < 0 || j < 0 || i >= static_cast<int>(mRoads.getHeight()) || j >= static_cast<int>(mRoads.getWidth()))
        return false;
    else if (mRoads.get(i, j))
    {
        coords = sf::Vector2i(j, i);
        return true;
    }
    else if (j + 1 < static_cast<int>(mRoads.getWidth()) && mRoads.get(i, j + 1))
    {
        coords = sf::Vector2i(j + 1, i);
        return true;
    }
    else if (i + 1 < static_cast<int>(mRoads.getHeight()) && mRoads.get(i + 1, j))
    {
        coords = sf::Vector2i(j, i + 1);
        return true;
    }
    else if (j > 0 && mRoads.get(i, j - 1))
    {
        coords = sf::Vector2i(j - 1, i);
        return true;
    }
    else if (i > 0 && mRoads.get(i - 1, j))
    {
        coords = sf::Vector2i(j, i - 1);
        return true;
    }
    else
        return false;
}

bool Network::getRandomEntryPoint(int i, int j, sf::Vector2i& coords) const
{
    if (!mRoads.get(i, j))
        return false;
    const std::vector<sf::Vector2i>& entryPoints = mEntryPoints.at(mComponents.get(i, j));
    if (entryPoints.empty())
        return false;
    else
    {
        std::uniform_int_distribution<int> entryPointPdf(0, entryPoints.size() - 1);
        coords = entryPoints[entryPointPdf(mGenerator)];
        return true;
    }
}

bool Network::isReachableFrom(sf::Vector2i start, sf::Vector2i end) const
{
    return mComponents.get(start.y, start.x) == mComponents.get(end.y, end.x);
}

std::vector<sf::Vector2i> Network::getPath(sf::Vector2i start, sf::Vector2i end) const
{
    // Trivial case
    if (start == end)
        return {start};

    // Check if the end is reachable from the start
    if (!isReachableFrom(start, end))
        return {};

    // Inverse the start and the end, because we will find a path from end to start
    std::swap(start, end);

    // BFS
    std::queue<sf::Vector2i> frontier;
    Array2<int> states(mRoads.getHeight(), mRoads.getWidth(), -1);
    frontier.push(start);
    while (!frontier.empty() && frontier.front() != end) {
        sf::Vector2i position = frontier.front();
        frontier.pop();
        for (int i = 0; i < static_cast<int>(sDirections.size()); ++i)
        {
            sf::Vector2i neighbor = position + sDirections[i];
            if (isValid(neighbor.y, neighbor.x) &&
                mRoads.get(neighbor.y, neighbor.x) && states.get(neighbor.y, neighbor.x) == -1)
            {
                frontier.push(neighbor);
                states.set(neighbor.y, neighbor.x, i);
            }
        }
    }

    // Retrieve path
    std::vector<sf::Vector2i> points;
    if (states.get(end.y, end.x) != -1)
    {
        sf::Vector2i point = end;
        while (point != start)
        {
            points.push_back(point);
            point -= sDirections[states.get(point.y, point.x)];
        }
        points.push_back(start);
    }

    return points;
}

void Network::floodFill(int i, int j, int c)
{
    // DFS
    std::stack<sf::Vector2i> frontier;
    frontier.push(sf::Vector2i(j, i));
    mComponents.set(i, j, c);
    if (isEntryPoint(i, j))
        mEntryPoints[c].emplace_back(j, i);
    while (!frontier.empty())
    {
        sf::Vector2i position = frontier.top();
        frontier.pop();
        for (const sf::Vector2i& direction : sDirections)
        {
            sf::Vector2i neighbor = position + direction;
            if (isValid(neighbor.y, neighbor.x) &&
                mRoads.get(neighbor.y, neighbor.x) && mComponents.get(neighbor.y, neighbor.x) != c)
            {
                // Update entry points
                if (isEntryPoint(neighbor.y, neighbor.x))
                    mEntryPoints[c].push_back(neighbor);
                // Update frontier
                frontier.push(neighbor);
                mComponents.set(neighbor.y, neighbor.x, c);
            }
        }
    }
}

bool Network::isValid(int i, int j) const
{
    return (i >= 0 && i < static_cast<int>(mRoads.getHeight()) &&
        j >= 0 && j < static_cast<int>(mRoads.getWidth()));
}

bool Network::isEntryPoint(int i, int j) const
{
    return (i == 0 || i == static_cast<int>(mRoads.getHeight()) - 1 ||
        j == 0 || j == static_cast<int>(mRoads.getWidth()) - 1);
}
