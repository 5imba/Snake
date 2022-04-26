#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class GameEngine : public olc::PixelGameEngine
{
private:
	struct vector
	{
		int x, y;

		inline bool operator==(vector v)
		{
			if (v.x == this->x && v.y == this->y)
				return true;
			else
				return false;
		}
	};
	struct cell
	{
		vector p, v;
		olc::Pixel c;

		cell(vector possition = { 0 }, vector direction = { 0 }, olc::Pixel color = olc::WHITE)
		{
			this->p = possition;
			this->v = direction;
			this->c = color;
		}
		cell(olc::Pixel color = olc::WHITE)
		{
			this->p = { -10 };
			this->v = { -10 };
			this->c = color;
		}
	};

	vector VectorSum(vector p1, vector p2)
	{
		vector result;
		result.x = p1.x + p2.x;
		result.y = p1.y + p2.y;
		return result;
	}
	vector VectorSub(vector p1, vector p2)
	{
		vector result;
		result.x = p1.x - p2.x;
		result.y = p1.y - p2.y;
		return result;
	}

	std::vector<cell> snake;
	std::vector<cell> food;
	std::vector<cell> gameField;
	vector vSnakeDirection;
	vector vSnakeCellNewPos;

	int nScrWidthCenter;
	int nScrHeightCenter;
	int nSnakeStartLength = 8;
	int nSnakeColorGradient = 5;
	int nSnakeAddCellCount = 0;
	int nSnakeRemoveCellCount = 0;

	int nSpeed = 20;
	int nSpeedCurrent = 0;
	int nSpeedStart = 10;
	int nSpeedCount = 0;
	int nSpeedForce = 0;

	int nScore = 0;
	int nScoreCount = 0;
	int nScoreCurrent = 0;

	int textX, textY;
	uint32_t textScale;
	std::string text;

	bool bKeys[4];
	bool bStartGame = true;
	bool bGameOver = false;

	std::string sButtonGuide = "PRESS ARROW TO START\n    ESC TO CLOSE    ";

	void CreateSnake()
	{
		snake.push_back(cell({ 0, nScrHeightCenter}, { 10,0 }, olc::Pixel(0, 255, 0)));
		for (int s = 1; s < nSnakeStartLength; s++)
			SnakeAddCell();
	}

	void CreateFood(int nFood)
	{
		vector newFoodPos = { 0 };
		bool bSnakeContain = false;
			
		do {
			bSnakeContain = false;
			newFoodPos = { RandTen(ScreenWidth()), RandTen(ScreenHeight()) };

			for (auto s : snake)
				if (s.p == newFoodPos)
					bSnakeContain = true;

		} while (bSnakeContain);
		
		food[nFood].p = newFoodPos;
		food[nFood].v = { 0 };
	}

	void SnakeMove(int s)
	{
		snake[s] = (s == 0) ? cell(vSnakeCellNewPos, snake[s].v, snake[s].c) : cell(vSnakeCellNewPos, snake[s - 1].v, snake[s].c);
	}

	void CheckBorder(vector &pos)
	{
		if (pos.x < 0)
			pos.x += ScreenWidth();
		if (pos.x > (ScreenWidth() - 10))
			pos.x -= ScreenWidth();
		if (pos.y < 0)
			pos.y += ScreenHeight();
		if (pos.y > (ScreenHeight() - 10))
			pos.y -= ScreenHeight();
	}

	bool CheckFood(vector pos)
	{
		if (pos == food[0].p)
		{
			CreateFood(0);
			SnakeAddCell();
			return true;
		}
		if (pos == food[1].p)
		{
			food[1].p = { -10 };
			nSnakeAddCellCount = 5;
			return true;
		}
		if (pos == food[2].p)
		{
			food[2].p = { -10 };
			for (int s = snake.size() - 1; s > snake.size() - 6; s--)
				snake[s].c = olc::RED;
			nSnakeRemoveCellCount = 5;
			return true;
		}
		return false;
	}

	bool CheckCollision(vector pos)
	{
		for (auto s : snake)
			if (s.p == pos)
				return true;
		return false;
	}

	void SnakeAddCell()
	{
		cell snakeLastCell = snake.back();
		olc::Pixel newCellColor = olc::WHITE;

		if (snakeLastCell.c.r == 0 && snakeLastCell.c.g == 255 && snakeLastCell.c.b != 255)
			newCellColor = olc::Pixel(snakeLastCell.c.r, snakeLastCell.c.g, snakeLastCell.c.b + nSnakeColorGradient);

		if (snakeLastCell.c.r == 0 && snakeLastCell.c.g != 0 && snakeLastCell.c.b == 255)
			newCellColor = olc::Pixel(snakeLastCell.c.r, snakeLastCell.c.g - nSnakeColorGradient, snakeLastCell.c.b);

		if (snakeLastCell.c.r != 255 && snakeLastCell.c.g == 0 && snakeLastCell.c.b == 255)
			newCellColor = olc::Pixel(snakeLastCell.c.r + nSnakeColorGradient, snakeLastCell.c.g, snakeLastCell.c.b);

		if (snakeLastCell.c.r == 255 && snakeLastCell.c.g == 0 && snakeLastCell.c.b != 0)
			newCellColor = olc::Pixel(snakeLastCell.c.r, snakeLastCell.c.g, snakeLastCell.c.b - nSnakeColorGradient);

		if (snakeLastCell.c.r == 255 && snakeLastCell.c.g != 255 && snakeLastCell.c.b == 0)
			newCellColor = olc::Pixel(snakeLastCell.c.r, snakeLastCell.c.g + nSnakeColorGradient, snakeLastCell.c.b);

		if (snakeLastCell.c.r != 0 && snakeLastCell.c.g == 255 && snakeLastCell.c.b == 0)
			newCellColor = olc::Pixel(snakeLastCell.c.r - nSnakeColorGradient, snakeLastCell.c.g, snakeLastCell.c.b);

		snake.push_back(cell(VectorSub(snakeLastCell.p, snakeLastCell.v), snakeLastCell.v, newCellColor));
	}

	int RandTen(int max)
	{
		int result;
		max /= 10;
		result = (rand() % max) * 10;
		return result;
	}
	
public:
	GameEngine()
	{
		sAppName = "Snake";
	}

public:
	bool OnUserCreate() override
	{
		srand((unsigned int)time(NULL));

		nScrWidthCenter = ScreenWidth() / 2;
		nScrHeightCenter = ScreenHeight() / 2;

		CreateSnake();
		vSnakeDirection = snake[0].v;

		food.push_back(cell(olc::YELLOW));
		food.push_back(cell(olc::MAGENTA));
		food.push_back(cell(olc::RED));

		food[0].p = { nScrWidthCenter, nScrHeightCenter };

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		// GAME TIMING ============================================

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		nSpeedCount++;

		// PLAYER INPUT ===========================================

		if (GetKey(olc::UP).bPressed && snake[0].v.y == 0) vSnakeDirection = { 0,-10 };
		if (GetKey(olc::DOWN).bPressed && snake[0].v.y == 0) vSnakeDirection = { 0, 10 };
		if (GetKey(olc::LEFT).bPressed && snake[0].v.x == 0) vSnakeDirection = { -10, 0 };
		if (GetKey(olc::RIGHT).bPressed && snake[0].v.x == 0) vSnakeDirection = { 10, 0 };

		// GAME LOGIC =============================================

		if (bStartGame)
		{
			// Start Game
			if (GetKey(olc::UP).bPressed || GetKey(olc::DOWN).bPressed || GetKey(olc::LEFT).bPressed || GetKey(olc::RIGHT).bPressed)
			{
				bStartGame = false;
				nSpeedCurrent = nSpeed;
				CreateFood(0);
			}
			if (GetKey(olc::ESCAPE).bPressed)
			{
				olc_Terminate();
			}
			if (nSpeedCount == nSpeedStart)
			{
				for (int s = snake.size() - 1; s >= 0; s--)
				{
					vSnakeCellNewPos = VectorSum(snake[s].p, snake[s].v);

					// Check Food
					if (vSnakeCellNewPos == food[0].p)
					{
						food[0].p = { -10 };
						SnakeAddCell();
					}
					if (vSnakeCellNewPos == food[1].p)
					{
						food[1].p = { -10 };
						nSnakeAddCellCount = 5;
					}
					if (vSnakeCellNewPos == food[2].p)
					{
						food[2].p = { -10 };
						for (int s = snake.size() - 1; s > snake.size() - 6; s--)
							snake[s].c = olc::RED;
						nSnakeRemoveCellCount = 5;
					}

					// Check Border
					if (vSnakeCellNewPos.x > ScreenWidth() + (int)snake.size() * 10)
					{
						snake.clear();
						CreateSnake();
						food[rand() % 3].p = { nScrWidthCenter, nScrHeightCenter };
						break;
					}

					SnakeMove(s);
				}

				if (nSnakeAddCellCount > 0)
				{
					SnakeAddCell();
					nSnakeAddCellCount--;
				}

				if (nSnakeRemoveCellCount > 0)
				{
					if (snake.size() > 0)
						snake.pop_back();
					nSnakeRemoveCellCount--;
				}

				nSpeedCount = 0;
			}
		}
		if (bGameOver)
		{
			// Game Over
			if (GetKey(olc::UP).bPressed || GetKey(olc::DOWN).bPressed || GetKey(olc::LEFT).bPressed || GetKey(olc::RIGHT).bPressed)
			{
				bGameOver = false;
				nScore = 0;
				nSpeedCurrent = nSpeed;
				nSpeedCount = 0;
				nScoreCount = 0;

				snake.clear();
				CreateSnake();
				CreateFood(0);
			}
			if (GetKey(olc::ESCAPE).bPressed)
			{
				olc_Terminate();
			}
		}

		if (!bStartGame && !bGameOver)
		{
			// Main Game
			if (nSpeedCount == nSpeedCurrent)
			{
				nScoreCount++;
				snake[0].v = vSnakeDirection;

				if (!CheckCollision(VectorSum(snake[0].p, snake[0].v)))
				{
					for (int s = snake.size() - 1; s >= 0; s--)
					{
						vSnakeCellNewPos = VectorSum(snake[s].p, snake[s].v);
						CheckBorder(vSnakeCellNewPos);
						if (CheckFood(vSnakeCellNewPos))
						{
							nSpeedForce++;
							nScoreCurrent = ((int)snake.size() * 10) / nScoreCount;
							if (nScoreCurrent > 10)
								CreateFood(1);
							if (nScoreCurrent > 50)
								CreateFood(2);
							if (nSpeedForce >= 20 && nSpeedCurrent > 5)
							{
								nSpeedCurrent--;
								nSpeedForce = 0;
							}
							nScore += nScoreCurrent;
							nScoreCount = 0;
						}
						SnakeMove(s);
					}
				}
				else
					bGameOver = true;

				if (nSnakeAddCellCount > 0)
				{
					SnakeAddCell();
					nSnakeAddCellCount--;
				}

				if (nSnakeRemoveCellCount > 0)
				{
					if (snake.size() > 0)
						snake.pop_back();
					else
						bGameOver = true;
					nSnakeRemoveCellCount--;
				}

				nSpeedCount = 0;
			}
		}

		// RENDER OUTPUT ==========================================

		gameField.clear();	// clear game field

			// Add Snake To Field
		for (int s = 0; s < snake.size(); s++)
			gameField.push_back(snake[s]);

		// Add Food To Field
		for (int f = 0; f < food.size(); f++)
			gameField.push_back(food[f]);

		// Draw Field
		for (int i = 0; i < gameField.size(); i++)
			FillRect(gameField[i].p.x, gameField[i].p.y, 9, 9, gameField[i].c);

		if (bStartGame)
		{
			// Set Text Start Logo
			text = "SNAKE";
			textX = nScrWidthCenter - 96;
			textY = nScrHeightCenter - 100;
			textScale = 5U;

			DrawString(nScrWidthCenter - ((int)sButtonGuide.length() / 4)*8, nScrHeightCenter + 80, sButtonGuide, olc::WHITE, 1U, 20);
		}
		else if (bGameOver)
		{
			// Set Text Game Over
			text = "GAME OVER";
			textX = nScrWidthCenter - 175;
			textY = nScrHeightCenter - 100;
			textScale = 5U;

			std::ostringstream score;
			score << "SCORE: " << nScore << " LENGTH: " << snake.size();

			DrawString(nScrWidthCenter - ((int)score.str().length()/2)*8, nScrHeightCenter + 35, score.str(), olc::WHITE, 1U);
			DrawString(nScrWidthCenter - ((int)sButtonGuide.length() / 4)*8, nScrHeightCenter + 80, sButtonGuide, olc::WHITE, 1U, 20);
		}
		else
		{
			// Set Text Score
			std::ostringstream ss;
			ss << "SCORE: " << nScore << " LENGTH: " << snake.size();
			text = ss.str();
			textX = 5;
			textY = 5;
			textScale = 1U;
		}
		// Draw Text
		DrawString(textX, textY, text, olc::WHITE, textScale);
		
		return true;
	}
};


int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	GameEngine ge;
	if (ge.Construct(400, 600, 1, 1))
		ge.Start();
	return 0;
}
